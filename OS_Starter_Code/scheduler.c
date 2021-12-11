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
void STRN();
void RR();
struct PCB IPC();
void Run(struct PCB* processToRun);
void handler1();
FILE* SchedulerLog;
FILE* SchedulerPerf;
int maxCount;
int parentID;
double* WTA ;
double* Wait ;
double* totalRun;
int main(int argc, char * argv[]) {
    
    signal (SIGUSR1,handler1);
    Algo=atoi(argv[1]);
    time_quantum=atoi(argv[2]);
    parentID = atoi(argv[3]);
    maxCount =atoi(argv[4]);
    WTA = calloc(maxCount,sizeof(double));
    Wait = calloc(maxCount,sizeof(double));
    totalRun = calloc(maxCount,sizeof(double));

    initClk();
    __clock_t x=getClk();
    

    SchedulerLog = fopen("scheduler.log", "w");
    switch (Algo) {
    case hpf_Algo:
        HPF();
        break; 
    case strn_Algo:
        STRN();
        break;
    case rr_Algo:
        RR();
        break;
    }

    //printing Log File
    fclose(SchedulerLog);
    double avgWait =0;
    double avgWTA=0;
    double CPUperf=0;
    double std=0.0;
    for (int i=0; i<maxCount ; i++)
    {
        avgWait += Wait[i];
        avgWTA += WTA[i];
        CPUperf+=totalRun[i];
    }

    avgWTA=avgWTA/maxCount;
    avgWait=avgWait/maxCount;
   for (int i=0; i<maxCount ; i++)
    {
        std += pow((WTA[i]-avgWTA),2);
    }
    std =sqrt(std/maxCount);
   // printf("total run time = %f\n",CPUperf);
  //  printf("Last clock equals %d\n",getClk());
    CPUperf= (CPUperf+1)/getClk();
    SchedulerPerf = fopen("scheduler.perf", "w");
    fprintf(SchedulerPerf,"# The running algorithm is : HPF\n");
    fprintf(SchedulerPerf,"CPU utilization = %.2f%%\n",CPUperf*100);
    fprintf(SchedulerPerf,"Avg WTA = %.2f\n",avgWTA);
    fprintf(SchedulerPerf,"Avg waiting = %.2f\n",avgWait);
    fprintf(SchedulerPerf,"Std WTA = %.2f\n",std);
    fclose(SchedulerPerf);

    destroyClk(true);
}



void STRN() {
    
    fprintf(SchedulerLog,"# The running algorithm is : SRTN\n");
    fprintf(SchedulerLog,"# At time x process y started arr z total w remain u wait v \n");

    isRunning=false;
    struct PriorityQueue SRTN_Ready;
    initializeQueue(&SRTN_Ready);
    __clock_t x= getClk();
    int count = maxCount; /// should be the number of processes
    struct PCB tempProcess;
    struct PCBNode processNode;
    struct PCB schProcess;
    schProcess.id = -1;
    int val;
    int c=0;
    int pDone=0;
    while (1) {
        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1){
            perror("error in creat\n");
            exit(-1);
        }
        struct msgBuff processInfo;
        if (c<maxCount){
            val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, !IPC_NOWAIT);   // ...........
            if (c<maxCount && val != -1)
            {
                CopyPCB(&tempProcess,processInfo.process);
                AddAccordingToRemainingTime(&SRTN_Ready,tempProcess);
                c++;
            }
        }

        if (SRTN_Ready.head!=NULL && isRunning == false)  
        {
            
            if(schProcess.id != -1)
            {
                //printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
                fprintf(SchedulerLog,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
                WTA[pDone]=(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime;
                Wait[pDone]=schProcess.WaitTime;
                totalRun[pDone]=schProcess.RunTime;
                pDone++;  
            }
            DeQueue(&SRTN_Ready,&schProcess);
            schProcess.startTime=getClk();
            IncreaseWaitTime(&schProcess,schProcess.startTime - schProcess.ArrTime);
            //printf("At time %d process %d started arr %d total %d remain %d wait %d \n",schProcess.startTime,schProcess.id,schProcess.ArrTime,schProcess.RunTime,schProcess.RunTime,schProcess.WaitTime);
            fprintf(SchedulerLog,"At time %d process %d started arr %d total %d remain %d wait %d \n",schProcess.startTime,schProcess.id,schProcess.ArrTime,schProcess.RunTime,schProcess.RunTime,schProcess.WaitTime);

            Run(&schProcess);  
            isRunning=true;
        }
        if (isRunning==false)
        {
            // aprocess has finished, print its details
           // printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
            fprintf(SchedulerLog,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
            WTA[pDone]=(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime;
            Wait[pDone]=schProcess.WaitTime;
            totalRun[pDone]=schProcess.RunTime;
            pDone++;        
        }
        
        if (c ==maxCount) // all the processes has been recieved
        kill(SIGINT,parentID);
        if (pDone==maxCount)
        {
             // should print the file
            break;
        }
    }
}












void HPF() {  // check the return type of the alogrithms
    fprintf(SchedulerLog,"# The running algorithm is : HPF\n");
    fprintf(SchedulerLog,"# At time x process y started arr z total w remain u wait v \n");

    isRunning=false;
    struct PriorityQueue HPF_Ready;
    initializeQueue(&HPF_Ready);
    __clock_t x= getClk();
    int count = maxCount; /// should be the number of processes
    struct PCB tempProcess;
    struct PCBNode processNode;
    struct PCB schProcess;
    schProcess.id = -1;
    int val;
    int c=0;
    int pDone=0;
    while (1) {
        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1){
            perror("error in creat\n");
            exit(-1);
        }
        struct msgBuff processInfo;
        if (c<maxCount){
            
            val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, !IPC_NOWAIT);   // ...........
            if (c<maxCount && val != -1)
            {
                CopyPCB(&tempProcess,processInfo.process);
                AddAccordingToPriority(&HPF_Ready,tempProcess);
                c++;
            }
        }

        if (HPF_Ready.head!=NULL && isRunning == false)  
        {
            
            if(schProcess.id != -1)
            {
                //printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
                fprintf(SchedulerLog,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
                WTA[pDone]=(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime;
                Wait[pDone]=schProcess.WaitTime;
                totalRun[pDone]=schProcess.RunTime;
                pDone++;  
            }
            DeQueue(&HPF_Ready,&schProcess);
            schProcess.startTime=getClk();
            IncreaseWaitTime(&schProcess,schProcess.startTime - schProcess.ArrTime);
            //printf("At time %d process %d started arr %d total %d remain %d wait %d \n",schProcess.startTime,schProcess.id,schProcess.ArrTime,schProcess.RunTime,schProcess.RunTime,schProcess.WaitTime);
            fprintf(SchedulerLog,"At time %d process %d started arr %d total %d remain %d wait %d \n",schProcess.startTime,schProcess.id,schProcess.ArrTime,schProcess.RunTime,schProcess.RunTime,schProcess.WaitTime);

            Run(&schProcess);  
            isRunning=true;
        }
        if (isRunning==false)
        {
            // aprocess has finished, print its details
           // printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
            fprintf(SchedulerLog,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
            WTA[pDone]=(double)(getClk()-(schProcess.ArrTime))/schProcess.RunTime;
            Wait[pDone]=schProcess.WaitTime;
            totalRun[pDone]=schProcess.RunTime;
            pDone++;        
        }
        
        if (c ==maxCount) // all the processes has been recieved
        kill(SIGINT,parentID);
        if (pDone==maxCount)
        {
             // should print the file
            break;
        }
    }
     
}
void Run(struct PCB* processToRun)
{
    //printf("A process is about to run\n");
    int pid;
    pid= fork();
    if (pid == 0) {
       // printf("process forking...\n");
        char id[10];
        sprintf(id,"%d",processToRun->id);
        char arrTime[10];
        sprintf(arrTime,"%d",processToRun->ArrTime);
        char runTime[10] ;
        sprintf(runTime,"%d",processToRun->RunTime);
        char Priority[10] ;
        sprintf(Priority,"%d",processToRun->Priority);
        char WaitTime[10];
        sprintf(WaitTime,"%d",processToRun->WaitTime);
        char RemainingTime[10] ;
        sprintf(RemainingTime,"%d",processToRun->RemainingTime);
        char startTime[10];
        sprintf(startTime,"%d",processToRun->startTime);
        char endTime[10] ;
        sprintf(endTime,"%d",processToRun->endTime);
        char* process_arg_list[]={"./process.out",id,arrTime,runTime,Priority,WaitTime,RemainingTime,startTime,endTime,runTime,0};
        execve(process_arg_list[0],process_arg_list,NULL); 
    }
    else
    {
         //printf("child pid = %d\n",pid);
         processToRun->PID = pid;
         processToRun->state = Running;
    }
   // IPC_send(processToRun);
    //struct PCB recievedProcess = IPC_recieve();
    //printf("at time =%d process with id %d finished with runtime time %d \n",getClk(),recievedProcess.id,recievedProcess.RunTime);
}
/*
struct PCB IPC() {
    struct PCB recievedProcess;
    int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
    if (pGeneratorToScheduler == -1){
        perror("error in creat\n");
        exit(-1);
    }
    struct msgBuff processInfo;
    int val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, !IPC_NOWAIT);
    if (val == -1)
        printf("Error in recieve");
    CopyPCB(&recievedProcess,processInfo.process);
    return recievedProcess;
}*/
/*
void IPC_send(struct PCB* processToRun)
    {
    int schedulerToProcess = msgget(1357, 0666 | IPC_CREAT);
    if (schedulerToProcess == -1){
        perror("error in creat");
        exit(-1);
    }
    struct msgBuff processInfo;
    processInfo.mtype = (getpid()%10000)+1;
    CopyPCB(&processInfo.process, *processToRun);
    int val = msgsnd(schedulerToProcess, &processInfo, sizeof(processInfo.process), IPC_NOWAIT);
    if (val == -1)
        printf("Error in send");
}

struct PCB IPC_recieve()
{
    int processToScheduler = msgget(2468, 0666 | IPC_CREAT);
    if (processToScheduler == -1){
        perror("error in creat");
        exit(-1);
    }
    struct PCB recievedProcess;
    struct msgBuff processInfo;
    int val = msgrcv(processToScheduler, &processInfo, sizeof(processInfo.process), 0, !IPC_NOWAIT);
    if (val == -1)
        printf("Error in recieve");
    CopyPCB(&recievedProcess,processInfo.process);
    return recievedProcess;
}*/

void RR() {
    printf("The user chose to run RR\n");   
}
void handler1()
{
    isRunning=false;
}