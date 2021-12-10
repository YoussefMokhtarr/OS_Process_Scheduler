#include "PriorityQueue.h"
//#include "PCB.h"

//variables 
int Algo; 
int time_quantum;

//temp for input
int a,b,c,d;
struct PCB processArr[3];
//functions
void clearResources(int);
void getAlgorithm();
void Start_Clk_Scheduler();
void ReadFile();
void IPC(struct PCB processToBeSent,int y);
struct PriorityQueue sendingQueue;


int main(int argc, char * argv[]) {
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    ReadFile();
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    getAlgorithm();   
    // 3. Initiate and create the scheduler and clock processes.
    Start_Clk_Scheduler();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    
    // To get time use this
    //sleep(10);
   
       
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    int x = getClk();
    int i = 0;
    int y;
    struct PCB processToBeSent;
    while(sendingQueue.head) {  // should loop on the input queue
        y= getClk();
        DeQueue(&sendingQueue,&processToBeSent);
        if (processToBeSent.ArrTime <= y-x) {
            IPC(processToBeSent,y);
            printf("current time is %d\n", getClk());
        }
         
       // printf("current time is %d     i=%d\n", y-x,i);
        //sleep(1);
    }
    printf("Process generator finishes its work\n");
    destroyClk(true);
}

 
void ReadFile() {
    //array of process
    initializeQueue(&sendingQueue);
    struct PCB p1;
    struct PCB p2;
    struct PCB p3;
    struct PCBNode processNode1;
    struct PCBNode processNode2;
    struct PCBNode processNode3;
    setPCB(&p1,0,1,2,1);
    processNode1 = GenerateNode(p1);
    enQueue(&sendingQueue,&processNode1);
    setPCB(&p2,1,3,4,2);
    processNode2 = GenerateNode(p2);
    enQueue(&sendingQueue,&processNode2);
    setPCB(&p3,2,5,6,3);
    processNode3 = GenerateNode(p3);
    enQueue(&sendingQueue,&processNode3);

}
void IPC(struct PCB processToBeSent, int t) {
    int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
    if (pGeneratorToScheduler == -1){
        perror("error in creat");
        exit(-1);
    }
    struct msgBuff processInfo;
    processInfo.mtype = getpid()%10000;
    CopyPCB(&processInfo.process, processToBeSent);
    int val = msgsnd(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), IPC_NOWAIT);
    if (val == -1)
        printf("Error in send");
   // printf("at time %d A process with id %d has been sent\n",t,processInfo.process.id);
}
void clearResources(int signum) {
    //TODO Clears all resources in case of interruption
}

void getAlgorithm() {
    printf("Choose the prefered Algorithm....\n");
    printf("1. HPF \n2. SRTN \n3. RR\n");
    scanf("%d", &Algo);

    while (!(Algo==1||Algo==2||Algo==3)) {
        printf("Choose A valid Number....\n");
        scanf("%d", &Algo);
        sleep(3);
    }
    if (Algo==3) {
        printf("Enter the quantum time....\n");
        scanf("%d", &time_quantum);
    }
    
    //char* Algo_String; 
    //char* time_quantum_String; 
   // sprintf(Algo_String, "%d", Algo);
   // sprintf(time_quantum_String, "%d", time_quantum);

}

void Start_Clk_Scheduler() {
 int pid;
 for (int i=0;i<2;i++) {
     pid= fork();
     if (pid==0) {
        if (i==0) {
           // printf("CLK forking...\n");
            execv("./clk.out",NULL);
            //sleep(10);
        }
        else{
            char cSendAlgo[10];
            char cSendTime_quantum[10];
            sprintf(cSendAlgo,"%d",Algo);
            sprintf(cSendTime_quantum,"%d",time_quantum);
            char* scheduler_arg_list[]={"./scheduler.out",cSendAlgo,cSendTime_quantum,0};
            execve(scheduler_arg_list[0],scheduler_arg_list,NULL); // all the processes should be sent by execve
        }
     }
 }
 
}

// notes
// we should have IO class that is resposnsible for all the IO processes

