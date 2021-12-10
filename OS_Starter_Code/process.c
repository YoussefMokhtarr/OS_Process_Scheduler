#include"headers.h"

/* Modify this file as needed*/
int remainingtime;
/*struct msgBuff {
    long mtype;
    struct PCB process;
};
void IPC_send(struct PCB* processToRun);
struct PCB IPC_recieve();*/

int main(int agrc, char * argv[]) {
    initClk();
    
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    int id =atoi(argv[1]);
    int ArrTime=atoi(argv[2]); 
    int RunTime=atoi(argv[3]); 
    int Priority=atoi(argv[4]); 
    int WaitTime=atoi(argv[5]); 
    int remainingTime=atoi(argv[6]); 
    int startTime=atoi(argv[7]);
    int endTime=atoi(argv[8]);
    remainingtime=RunTime;
    int x=getClk();

    while (remainingtime > 0){
        if (x<getClk())
        {
        x=getClk();
        //remainingtime=RunTime-(getClk()-startTime-WaitTime);
        remainingtime=RunTime-(getClk()-startTime);
        }
    }
    endTime=getClk();
    kill(getppid(),SIGUSR1);
    // do the required modifications to recievedprocess paramaters
    //destroyClk(false);
    
    return 0;
}
/*
void IPC_send(struct PCB* recievedProcess)
{
    int processToScheduler = msgget(2468, 0666 | IPC_CREAT);
    if (processToScheduler == -1){
        perror("error in creat");
        exit(-1);
    }
    struct msgBuff processInfo;
    processInfo.mtype = (getpid()%10000)+1;
    CopyPCB(&processInfo.process, *recievedProcess);
    int val = msgsnd(processToScheduler, &processInfo, sizeof(processInfo.process), IPC_NOWAIT);
    if (val == -1)
        printf("Error in send");
}
struct PCB IPC_recieve()
{
 struct PCB recievedProcess;
    int schedulerToProcess = msgget(1357, 0666 | IPC_CREAT);
    if (schedulerToProcess == -1){
        perror("error in creat");
        exit(-1);
    }
    struct msgBuff processInfo;
    int val = msgrcv(schedulerToProcess, &processInfo, sizeof(processInfo.process), 0, !IPC_NOWAIT);
    if (val == -1)
        printf("Error in recieve");
    CopyPCB(&recievedProcess,processInfo.process);  
    return recievedProcess;
}
*/