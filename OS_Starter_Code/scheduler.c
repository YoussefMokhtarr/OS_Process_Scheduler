#include "headers.h"

//data
#define hpf_Algo 1
#define strn_Algo 2
#define rr_Algo 3
char Algo;
int time_quantum;
struct PCB recievedProcess;

//functions
void HPF();
void STRN();
void RR();
void IPC();

struct msgBuff
{
    long mtype;
    struct PCB process;
};
int main(int argc, char * argv[])
{
    initClk();
    Algo=argv[1][0]-'0';
    time_quantum=argv[2][0]-'0';
    printf("scheduler Arguments %d    %d\n",Algo, time_quantum);
    IPC();
    switch (Algo)
    {
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
    
    //TODO implement the scheduler :)
    //upon termination release the clock resources
    //destroyClk(true);
}

void IPC()
{
    int upQId = msgget(1234, 0666 | IPC_CREAT);
    if (upQId == -1){
        perror("error in creat");
        exit(-1);
    }
    struct msgBuff processInfo;
    int val = msgrcv(upQId, &processInfo, sizeof(processInfo.process), 0, !IPC_NOWAIT);
    if (val == -1)
        printf("Error in recieve");
    CopyPCB(&recievedProcess,processInfo.process);
    printf("The id is %d\n",recievedProcess.id);
     printf("The id is %d\n",processInfo.process.id);
}

void HPF()  // check the return type of the alogrithms
{
    printf("The user chose to run HPF\n");
}
void STRN()
{
    printf("The user chose to run STRN\n");
}
void RR()
{
    printf("The user chose to run RR\n");   
}