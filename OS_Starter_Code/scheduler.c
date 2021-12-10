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
//void IPC_send(struct PCB* processToRun);
//struct PCB IPC_recieve();

int maxCount;
int parentID;
int main(int argc, char * argv[]) {
    signal (SIGUSR1,handler1);
    
    
    //Algo=argv[1][0]-'0';
    //time_quantum=argv[2][0]-'0';
    Algo=atoi(argv[1]);
    time_quantum=atoi(argv[2]);
     parentID = atoi(argv[3]);
    initClk();
    __clock_t x=getClk();
   // int c=0;
    maxCount =atoi(argv[4]);
 /*while (1) {

        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1){
            perror("error in creat\n");
            exit(-1);
        }
        struct msgBuff processInfo;
        int val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 10, !IPC_NOWAIT); 
        if (val==-1)
           printf("error in recieving..\n");
        else
            c++;
        printf("The reiceved process id is %d at time %d\n",processInfo.process.id,getClk());
        if (c ==maxCount)
            break;
        }
        kill(SIGINT,parentID);*/



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
    printf("Scheduler finishes its work\n");
    destroyClk(true);
}



void HPF() {  // check the return type of the alogrithms
    
    isRunning=false;
    struct PriorityQueue HPF_Ready;
    initializeQueue(&HPF_Ready);
    __clock_t x= getClk();
    int count = maxCount; /// should be the number of processes
    struct PCB tempProcess;
    struct PCBNode processNode;
    struct PCB schProcess;
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
           // processNode = GenerateNode(tempProcess);
            AddAccordingToPriority(&HPF_Ready,tempProcess);
            c++;
        }
        
        //tempProcess = IPC();
        //return recievedProcess;
        }
        
       // printf("A process with id %d enqued in Queue with head%d\n",processNode.pcb.id, HPF_Ready.head->pcb.id);

        if (HPF_Ready.head!=NULL) 
        {
            DeQueue(&HPF_Ready,&schProcess);
            schProcess.startTime=getClk();
            // print the process details hereeeeeeeeeeeeeeeeeeeeeeeeeee
            printf("At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,schProcess.RunTime,schProcess.WaitTime);
            Run(&schProcess);  
            //printf("A process with id %d dequed\n",schProcess.id);
            //isRunning=true;
        }
        if (isRunning==false)
        {
            // aprocess has finished, print its details
            printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %d\n",getClk(),schProcess.id,schProcess.ArrTime,schProcess.RunTime,0,schProcess.WaitTime,getClk()-(schProcess.ArrTime),(getClk()-(schProcess.ArrTime))/schProcess.RunTime);
            isRunning=true;
            pDone++;        
            }
        
        if (c ==maxCount) // all the processes has been recieved
        kill(SIGINT,parentID);
        if (pDone==maxCount)
        {
            printf("HPD finished\n"); // should print the file
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
void STRN() {
    printf("The user chose to run STRN\n");
}
void RR() {
    printf("The user chose to run RR\n");   
}
void handler1()
{
    isRunning=false;
}