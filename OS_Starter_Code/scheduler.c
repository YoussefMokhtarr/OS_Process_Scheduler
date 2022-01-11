#include "headers.h"
//#include "PCB.h"

//data
#define hpf_Algo 1
#define strn_Algo 2
#define rr_Algo 3
#define memSize 1024
char Algo;
int time_quantum;
bool isRunning;
//functions
void HPF();
void SRTN();
void RR();
struct PCB IPC();
void Run(struct PCB *processToRun);
void handler1();
void almHandeler(int);
void alocate(struct PCB *process);
void dealocate(struct PCB *process);
FILE *SchedulerLog;
FILE *SchedulerPerf;
FILE *memlog;
int maxCount;
int parentID;
double *WTA;
double *Wait;
double *totalRun;
int endTime;
int startTime;
int memory[memSize];
struct space
{
    int start, size;
};

struct space freeSlots[memSize];

void mergeSlots()
{
    int foundany = 1;
    //int index = 0;
    while (foundany == 1)
    {
        foundany = 0;
        for (int index = 0; index < 1024; index++)
        {
            if (freeSlots[index].size != -1)
            {
                //printf("first if\n");
                int factor = freeSlots[index].start / freeSlots[index].size;
                if (factor % 2 == 0)
                {
                    //printf(" if 2nd\n");
                    if (freeSlots[freeSlots[index].start + freeSlots[index].size].size == freeSlots[index].size)
                    {
                        //printf("last if\n");
                        foundany = 1;
                        freeSlots[freeSlots[index].start + freeSlots[index].size].start = -1;
                        freeSlots[freeSlots[index].start + freeSlots[index].size].size = -1;
                        freeSlots[index].size = freeSlots[index].size * 2;
                    }
                }
            }
        }
    }
}

void dealloc(int id)
{
    int found = -1;
    int start = -1;
    int size = -1;
    for (int i = 0; i < memSize; i++)
    {
        if (memory[i] == id)
        {
            found = i;
            start = i;
            //printf("id found at location %d\n", start);
            break;
        }
    }
    if (found == -1)
        printf("No allocated mem found for this process\n");
    else
    {
        while (found >= 0)
        {
            if (memory[found] == id)
            {
                memory[found] = -1;
                if (found < memSize)
                    found++;
            }
            if (memory[found] != id)
            {

                size = found - start;
                fprintf(memlog,"At time %d mem freed %d bytes from process %d from %d to %d\n", getClk(), size, id, start, found - 1);
                found = -1;
            }
        }
        // struct space FreedLoc;
        //FreedLoc.start=start;
        //FreedLoc.size=found-start;
        freeSlots[start].size = size;
        freeSlots[start].start = start;
        // merge slots
        int i = 0;
        //for (i = 0; i < memSize; i++)
        while (i < memSize && i > 0)
        {
            if (freeSlots[i].size != -1)
            {
                int targetLoc = freeSlots[i].size + freeSlots[i].start;
                if (targetLoc < memSize && freeSlots[targetLoc].size == freeSlots[i].size)
                {
                    if (log2(freeSlots[targetLoc].start + freeSlots[targetLoc].size) == ceil(log2(freeSlots[targetLoc].start + freeSlots[targetLoc].size)))
                    {
                        freeSlots[targetLoc].size = -1;
                        freeSlots[targetLoc].start = -1;
                        freeSlots[i].size = 2 * freeSlots[i].size;
                        i = i - freeSlots[i].size - 1;
                    }
                }
            }
            i++;
        }
    }

    mergeSlots();

    /*for (int i = 0; i < 1024; i++)
    {
        if (freeSlots[i].size != -1)
        {
            printf("%d --> %d \t", freeSlots[i].start, freeSlots[i].size);
        }
    }*/
    //printf("\n");
    ///TODO: merge the adjecnt free slots in the freeSlots arr
    //Note: you might fing them in sperate locations not just adjecnt
}

void allocate(int pSize, int id)
{
    int size = 0;
    int neededsize;
    int startSize = memSize;

    // find  if the smallest location size
    neededsize = pow(2, ceil(log(pSize) / log(2)));
    //printf("The needed size for this process is %d\n",neededsize);
    int smallestSizeFound = memSize + 1;
    int startOFsamallestSize = -1;
    int indexOfFreeLoc = -1;
    for (int i = 0; i < memSize; i++)
    {
        if (freeSlots[i].size >= neededsize && freeSlots[i].size < smallestSizeFound)
        {
            smallestSizeFound = freeSlots[i].size;
            startOFsamallestSize = freeSlots[i].start;
            indexOfFreeLoc = i;
        }
    }
    if (startOFsamallestSize == -1)
    {
        // no place to allocate
        printf("No free location found\n");
    }
    else
    {
        // check if the size found is need to be divided or not
        // if yes
        //divide
        while (smallestSizeFound > neededsize)
        {
            // divide the greater size and place it in the free slots
            // change the parameters of the current index
            freeSlots[indexOfFreeLoc].size = freeSlots[indexOfFreeLoc].size / 2;
            // and add the other half to the free Slot index
            struct space temp;
            temp.size = freeSlots[indexOfFreeLoc].size;
            temp.start = freeSlots[indexOfFreeLoc].start + temp.size;
            // add the new slot to the FreeSlots arr
            freeSlots[temp.start].size = temp.size;
            freeSlots[temp.start].start = temp.start;

            smallestSizeFound = freeSlots[indexOfFreeLoc].size;
        }
        // allocate the mem and remove the allocated place from the free slots list
        //actual allocation
        for (int i = freeSlots[indexOfFreeLoc].start; i < freeSlots[indexOfFreeLoc].size + freeSlots[indexOfFreeLoc].start; i++)
        {
            memory[i] = id;
        }
        fprintf(memlog,"At time %d allocated %d bytes for proccess %d from %d to %d\n", getClk(), freeSlots[indexOfFreeLoc].size, id, freeSlots[indexOfFreeLoc].start, freeSlots[indexOfFreeLoc].size + freeSlots[indexOfFreeLoc].start - 1);
        // remove it from the freeSlots list
        freeSlots[indexOfFreeLoc].size = -1;
        freeSlots[indexOfFreeLoc].start = -1;
    }
}

int main(int argc, char *argv[])
{
    freeSlots[0].start = 0;
    freeSlots[0].size = memSize;
    for (int i = 1; i < memSize; i++)
    {
        freeSlots[i].start = -1;
        freeSlots[i].size = -1;
    }
    // intalizing of free arr;
    for (int i = 0; i < memSize; i++)
    {
        memory[i] = -1;
    }

    //signal(SIGCHLD, SIG_IGN);
    signal(SIGALRM, almHandeler);
    Algo = atoi(argv[1]);
    time_quantum = atoi(argv[2]);
    parentID = atoi(argv[3]);
    maxCount = atoi(argv[4]);
    WTA = calloc(maxCount, sizeof(double));
    Wait = calloc(maxCount, sizeof(double));
    totalRun = calloc(maxCount, sizeof(double));

    initClk();
    __clock_t x = getClk();

    SchedulerLog = fopen("scheduler.log", "w");
    memlog = fopen("memory.log", "w");
    switch (Algo)
    {
    case hpf_Algo:
        HPF();
        break;
    case strn_Algo:
        signal(SIGCHLD, handler1);
        SRTN();
        break;
    case rr_Algo:
        RR();
        break;
    }

    //printing Log File
    fclose(SchedulerLog);
    fclose(memlog);
    double avgWait = 0;
    double avgWTA = 0;
    double CPUperf = 0;
    double std = 0.0;
    for (int i = 0; i < maxCount; i++)
    {
        avgWait += Wait[i];
        avgWTA += WTA[i];
        CPUperf += totalRun[i];
    }

    avgWTA = avgWTA / maxCount;
    avgWait = avgWait / maxCount;
    for (int i = 0; i < maxCount; i++)
    {
        std += pow((WTA[i] - avgWTA), 2);
    }
    std = sqrt(std / maxCount);
    // printf("total run time = %f\n",CPUperf);
    printf("Last clock equals %d\n", getClk());
    CPUperf = (CPUperf) / (endTime - startTime);

    SchedulerPerf = fopen("scheduler.perf", "w");
    if (Algo == 1)
        fprintf(SchedulerPerf, "# The running algorithm is : HPF\n");
    else if (Algo == 2)
        fprintf(SchedulerPerf, "# The running algorithm is : SRTN\n");
    else
        fprintf(SchedulerPerf, "# The running algorithm is : RR\n");
    fprintf(SchedulerPerf, "CPU utilization = %.2f%%\n", CPUperf * 100);
    fprintf(SchedulerPerf, "Avg WTA = %.2f\n", avgWTA);
    fprintf(SchedulerPerf, "Avg waiting = %.2f\n", avgWait);
    fprintf(SchedulerPerf, "Std WTA = %.2f\n", std);
    fclose(SchedulerPerf);
    //printf("finish clock:%d\n",getClk());
    destroyClk(true);
}

struct PCB tempProcess;
struct PCBNode processNode;
struct PCB schProcess;

int val;
int c = 0;
int pDone = 0;
void HPF()
{ // check the return type of the alogrithms

    fprintf(SchedulerLog, "# The running algorithm is : HPF\n");
    fprintf(SchedulerLog, "# At time x process y started arr z total w remain u wait v \n");
    int count = maxCount; /// should be the number of processes
    isRunning = false;
    schProcess.id = -1;
    struct PriorityQueue HPF_Ready;
    initializeQueue(&HPF_Ready);
    __clock_t x = getClk();
    int s = 0;
    while (1)
    {
        //printf("I entered\n");
        struct msgBuff processInfo;
        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1)
        {
            perror("error in creat\n");
            exit(-1);
        }
        // if (c < maxCount )
        {
            val = 1;
            while (val != -1)
            {
                val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, IPC_NOWAIT); // ...........
                if (val == -1)
                    break;
                CopyPCB(&tempProcess, processInfo.process);
                AddAccordingToPriority(&HPF_Ready, tempProcess);
            }
            //
            if (HPF_Ready.head != NULL)
            {
                DeQueue(&HPF_Ready, &schProcess);
                schProcess.startTime = getClk();
                IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", schProcess.startTime, schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RunTime, schProcess.WaitTime);
                allocate(schProcess.size, schProcess.id);
                if (s == 0)
                {
                    startTime = schProcess.startTime;
                    s++;
                }
                //Run(&schProcess);
                isRunning = true;
                int pid;
                pid = fork();
                if (pid == 0)
                {
                    // printf("process forking...\n");
                    char runTime[10];
                    sprintf(runTime, "%d", schProcess.RunTime);
                    char *process_arg_list[] = {"./process.out", runTime, 0};
                    //RunningProcessID = getpid();
                    execve(process_arg_list[0], process_arg_list, NULL);
                }
                else
                {
                    int status;
                    int y = wait(&status);
                    WIFEXITED(status);
                    schProcess.PID = pid;
                }
                fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                dealloc(schProcess.id);
                //printf("I entered\n");
                endTime = getClk();
                WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                Wait[pDone] = schProcess.WaitTime;
                totalRun[pDone] = schProcess.RunTime;
                schProcess.state = Terminated;
                pDone++;
            }
        }

        if (pDone == maxCount)
        {
            kill(getppid(), SIGTERM);
            break;
        }
    }
}
int RunningProcessID;
void Run(struct PCB *processToRun)
{
    //printf("A process is about to run\n");
    if (processToRun->state == Stopped && Algo == 2)
    {
        processToRun->state = Running;
        kill(processToRun->PID, SIGCONT);
        fprintf(SchedulerLog, "At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
        return;
    }
    int pid;
    pid = fork();
    if (pid == 0)
    {
        // printf("process forking...\n");
        char runTime[10];
        sprintf(runTime, "%d", processToRun->RunTime);
        char *process_arg_list[] = {"./process.out", runTime, 0};
        RunningProcessID = getpid();
        execve(process_arg_list[0], process_arg_list, NULL);
    }
    else
    {

        processToRun->PID = pid;
        processToRun->state = Running;
    }
}

void RR()
{
    fprintf(SchedulerLog, "# The running algorithm is : RR\n");
    fprintf(SchedulerLog, "# At time x process y started arr z total w remain u wait v \n");
    int count = maxCount; /// should be the number of processes
    isRunning = false;
    //schProcess.id = -1;
    struct PriorityQueue RR_Ready;
    initializeQueue(&RR_Ready);
    struct PriorityQueue Stopped_RR_Ready;
    initializeQueue(&Stopped_RR_Ready);
    __clock_t x = getClk();
    //printf("clock at the begining of RR:%d\n",getClk());
    int s = 0;
    while (1)
    {
        struct msgBuff processInfo;
        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1)
        {
            perror("error in creat\n");
            exit(-1);
        }
        //if (c < maxCount)
        {

            int rc;
            struct msqid_ds buf;
            int num_messages;

            rc = msgctl(pGeneratorToScheduler, IPC_STAT, &buf);
            num_messages = buf.msg_qnum;
            //printf("at time %d the message in Q=%d\n",getClk(),num_messages);
            for (int i = 0; i < num_messages; i++)
            {
                val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, !IPC_NOWAIT); // ...........
                CopyPCB(&tempProcess, processInfo.process);
                Add(&RR_Ready, tempProcess);
            }

            //if (schProcess.state == Stopped)
            //    AddAccordingToInverseArrivalTime(&RR_Ready, schProcess); //TODO: we need a function that adds according to inverse arrivaltime
            if (RR_Ready.head != NULL)
            {
                DeQueue(&RR_Ready, &schProcess);
                schProcess.startTime = getClk();
                IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                if (schProcess.RemainingTime > time_quantum)
                {
                    //printf("At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    allocate(schProcess.size, schProcess.id);
                    if (s == 0)
                    {
                        startTime = schProcess.startTime;
                        s++;
                    }
                    Run(&schProcess);
                    //printf("Quantum is:%d and clock now is:%d\n", time_quantum, getClk());
                    alarm(time_quantum + getClk() - x);
                    pause();

                    //  printf("Iam awakeeeeeeeeeeeeeeeeeeeeeeeeee Quantum is:%d and clock now is:%d\n",time_quantum,getClk());
                    //TODO:stop th process
                    kill(schProcess.PID, SIGSTOP); // look at RUN function
                    //decrement the remaining time
                    schProcess.RemainingTime = schProcess.RemainingTime - time_quantum;
                    // printf("At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    fprintf(SchedulerLog, "At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    schProcess.state = Stopped;
                    IncreaseWaitTime(&schProcess, -getClk());
                    Add(&Stopped_RR_Ready, schProcess);
                }
                else
                {
                    fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    allocate(schProcess.size, schProcess.id);
                    if (s == 0)
                    {
                        startTime = schProcess.startTime;
                        s++;
                    }
                    //printf("At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    Run(&schProcess);

                    // alarm(schProcess.RunTime + getClk() - x);
                    // pause();
                    int status;
                    int x;
                    x = wait(&status);
                    WIFEXITED(status);
                    // printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    dealloc(schProcess.id);
                    endTime = getClk();
                    WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                    Wait[pDone] = schProcess.WaitTime;
                    totalRun[pDone] = schProcess.RunTime;
                    schProcess.state = Terminated;
                    pDone++;
                }
            }
            else if (Stopped_RR_Ready.head != NULL)
            {
                DeQueue(&Stopped_RR_Ready, &schProcess);
                IncreaseWaitTime(&schProcess, getClk());
                if (schProcess.RemainingTime > time_quantum)
                {
                    kill(schProcess.PID, SIGCONT);
                    // printf("At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    fprintf(SchedulerLog, "At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);

                    //sleep(time_quantum);
                    alarm(time_quantum + getClk() - x);
                    pause();

                    //TODO:stop th process
                    kill(schProcess.PID, SIGSTOP); // look at RUN function
                    //decrement the remaining time
                    schProcess.RemainingTime = schProcess.RemainingTime - time_quantum;
                    //printf("At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    fprintf(SchedulerLog, "At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    schProcess.state = Stopped;
                    IncreaseWaitTime(&schProcess, -getClk());
                    Add(&Stopped_RR_Ready, schProcess);
                }
                else
                {
                    kill(schProcess.PID, SIGCONT);
                    // printf("At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    fprintf(SchedulerLog, "At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);

                    //alarm(schProcess.RemainingTime + getClk() - x);
                    //pause();
                    int status;
                    int x;
                    x = wait(&status);
                    WIFEXITED(status);
                    //printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    dealloc(schProcess.id);
                    endTime = getClk();
                    WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                    Wait[pDone] = schProcess.WaitTime;
                    totalRun[pDone] = schProcess.RunTime;
                    schProcess.state = Terminated;
                    pDone++;
                }
            }
        }
        x = getClk();
        if (pDone == maxCount)
        {
            kill(getppid(), SIGTERM);
            break;
        }
    }
}

void handler1() // from sigchild
{
    raise(SIGALRM);
}
void SRTN()
{
    //signal(SIGCHLD, handler1);
    fprintf(SchedulerLog, "# The running algorithm is : SRTN\n");
    fprintf(SchedulerLog, "# At time x process y started arr z total w remain u wait v \n");
    int count = maxCount; /// should be the number of processes
    isRunning = false;
    schProcess.id = -1;
    struct PriorityQueue SRTN_Ready;
    initializeQueue(&SRTN_Ready);
    initClk();
    __clock_t x = getClk();
    int s = 0;
    while (1)
    {
        struct msgBuff processInfo;
        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1)
        {
            perror("error in creat\n");
            exit(-1);
        }
        if (c < maxCount)
        {
            val = 1;
            while (val != -1)
            {
                val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, IPC_NOWAIT); // ...........
                if (val == -1)
                    break;
                // printf("I will add elements to the queue\n");
                CopyPCB(&tempProcess, processInfo.process);
                AddAccordingToRemainingTime(&SRTN_Ready, tempProcess);
            }

            if (schProcess.id != -1 && isRunning == true)
            {
                //printf("running process existing checked\n");
                schProcess.RemainingTime = schProcess.RemainingTime - (getClk() - schProcess.startTime);
                if (schProcess.RemainingTime <= 0 && schProcess.state != Terminated)
                {
                    // schProcess.WaitTime = getClk() - schProcess.RunTime - schProcess.startTime;
                    // printf("the wait time equ: %d\n",getClk() - schProcess.RunTime - schProcess.startTime);
                    schProcess.state = Terminated;
                    isRunning = false;
                    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    dealloc(schProcess.id);
                    endTime = getClk();
                    WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                    Wait[pDone] = schProcess.WaitTime;
                    totalRun[pDone] = schProcess.RunTime;
                    pDone++;
                    int status;
                    int y = wait(&status);
                    WIFEXITED(status);
                }
                if (SRTN_Ready.head != NULL)
                {
                    //printf("making sure that a process might be better to run\n");
                    if (SRTN_Ready.head->pcb.RemainingTime < schProcess.RemainingTime)
                    {
                        // printf("process %d should run instead of process %d at time = %d\n", SRTN_Ready.head->pcb.id, schProcess.id, getClk());
                        schProcess.state = Stopped;
                        //schProcess.WaitTime=
                        isRunning = false;
                        kill(schProcess.PID, SIGSTOP);
                        fprintf(SchedulerLog, "At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                        IncreaseWaitTime(&schProcess, -getClk());
                        AddAccordingToRemainingTime(&SRTN_Ready, schProcess);
                    }
                }
            }
            //
            if (SRTN_Ready.head != NULL && isRunning == false)
            {
                DeQueue(&SRTN_Ready, &schProcess);
                // printf("process %d will either start or resume\n", schProcess.id);
                schProcess.startTime = getClk();

                if (schProcess.state != Stopped)
                {
                    IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                    fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", schProcess.startTime, schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RunTime, schProcess.WaitTime);
                    allocate(schProcess.size,schProcess.id);
                    if (s == 0)
                    {
                        startTime = schProcess.startTime;
                        s++;
                    }
                    //schProcess.startTime = getClk();
                    //IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                }
                if (schProcess.state == Stopped)
                {
                    IncreaseWaitTime(&schProcess, getClk());
                }
                Run(&schProcess);
                isRunning = true;
                //pause();
            }
        }

        if (pDone == maxCount)
        {
            break;
        }
        pause();
    }
}
void almHandeler(int x) //
{
    //printf("I received an alarm at %d\n", getClk());
}