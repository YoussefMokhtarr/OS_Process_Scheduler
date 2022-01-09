#include "headers.h"
//#include "PCB.h"

//data
#define hpf_Algo 1
#define strn_Algo 2
#define rr_Algo 3
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
FILE *SchedulerLog;
FILE *SchedulerPerf;
int maxCount;
int parentID;
double *WTA;
double *Wait;
double *totalRun;
int main(int argc, char *argv[])
{
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
      printf("Last clock equals %d\n",getClk());
    CPUperf = (CPUperf) / (getClk() - x);
 
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

    while (1)
    {
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
/*
void SRTN()
{
    struct PriorityQueue SRTN_Ready;
    initializeQueue(&SRTN_Ready);
    __clock_t x = getClk();
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
        val = 1;
        int Receivedcounter = 0;
        while (val != -1)
        {
            val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, IPC_NOWAIT); // ...........
            if (val == -1)
                break;
            Receivedcounter++;
            CopyPCB(&tempProcess, processInfo.process);
            AddAccordingToRemainingTime(&SRTN_Ready, tempProcess);
        }

        if (schProcess.id != -1 && Receivedcounter > 0)
        {
            kill(schProcess.PID, SIGSTOP); // look at RUN function
            schProcess.RemainingTime = schProcess.RemainingTime - (getClk() - schProcess.startTime);
            //printf("At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
            fprintf(SchedulerLog, "At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
            schProcess.state = Stopped;
            //IncreaseWaitTime(&schProcess, -getClk());
            AddAccordingToRemainingTime(&SRTN_Ready, tempProcess);
        }
        //FIXME: I need to know if the process I just stopped has finished
        // or any other process finihsed to print the finish stat
        // ic ould check stopped status by (if remaintime=0)), but generally i can't
        int status;
        int x;
        if (WIFEXITED(status))
        {
            x = wait(&status);
        }
        if (SRTN_Ready.head != NULL)
        {
            DeQueue(&SRTN_Ready, &schProcess);
            //IncreaseWaitTime(&schProcess, getClk());
            if (schProcess.state == Stopped)
            {
                kill(schProcess.PID, SIGCONT);
                // printf("At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                fprintf(SchedulerLog, "At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
            }
            else if (schProcess.state == NotStarted)
            {
                schProcess.startTime = getClk();
                fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                Run(&schProcess);
            }
        }
        Receivedcounter = 0;
        if (pDone == maxCount)
            break;
    }
}
*/
void handler1() // from sigchild
{               /*
    if (Algo == 1)
    {
        int wstatus = -2;
        waitpid(-1, &wstatus, 0);
        isRunning = false;
        fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
        WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
        Wait[pDone] = schProcess.WaitTime;
        totalRun[pDone] = schProcess.RunTime;
        schProcess.state = Terminated;
    }
    pDone++;*/
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
                    WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                    Wait[pDone] = schProcess.WaitTime;
                    totalRun[pDone] = schProcess.RunTime;
                    pDone++;
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
                    schProcess.startTime = getClk();
                    IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
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
