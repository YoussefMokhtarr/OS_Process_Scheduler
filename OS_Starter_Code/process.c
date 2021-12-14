
/* Modify this file as needed*/
<<<<<<< HEAD


#include <stdlib.h>
#include <time.h>
int main(int argc, char * argv[])
{
    while(clock() < atoi(argv[1]) * CLOCKS_PER_SEC);
    return 0;
}










//int remainingtime;
=======
int remainingtime;
__clock_t x;
int ContinueTime;
int PauseTime;
>>>>>>> ce97b1d07942dfd689a553cdb5cde8379933f800
/*struct msgBuff {
    long mtype;
    struct PCB process;
};
void IPC_send(struct PCB* processToRun);
struct PCB IPC_recieve();*/
<<<<<<< HEAD

/*int main(int agrc, char * argv[]) {
=======
void ContinueHandler();
int main(int agrc, char * argv[]) {
    signal(SIGCONT,ContinueHandler);
>>>>>>> ce97b1d07942dfd689a553cdb5cde8379933f800
    initClk();
    //printf("mypid = %d\n",getpid());
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
    ContinueTime = 0;
    PauseTime = 0;
    x=getClk();


    while (remainingtime + PauseTime > 0){
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
    //destroyClk(true);
    
    return 0;
<<<<<<< HEAD
}*/
=======
}

void ContinueHandler()
{
    ContinueTime = getClk();
    PauseTime += ContinueTime - x;
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
>>>>>>> ce97b1d07942dfd689a553cdb5cde8379933f800
