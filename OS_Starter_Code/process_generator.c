#include "headers.h"
//#include "PCB.h"

//variables 
int Algo; 
int time_quantum;
int count=0;
//temp for input
int a,b,c,d;
struct PCB processArr[3];
//functions
void clearResources(int);
struct PriorityQueue que;
struct PCB processToBeSent;
void getAlgorithm();
void Start_Clk_Scheduler();
void ReadFile();
void IPC(struct PCB processToBeSent);
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
    __clock_t  x = getClk();
    int i = 0;
   // __clock_t  y;
    
    while(que.head !=NULL) {  // should loop on the input queue
      __clock_t  y= getClk();
        processToBeSent=que.head->pcb;
        if (processToBeSent.ArrTime <= y-x) {
            //printf("current time is %d\n", getClk());
           // printf("current ID of the process to be sent%d at time %d\n",processToBeSent.id, getClk());
           struct PriorityQueue newQue;
           initializeQueue(&newQue);
           while(que.head!= NULL && que.head->pcb.ArrTime <= y - x)
           { 
                DeQueue(&que,&processToBeSent);
                if(Algo == 1)
                {
                    AddAccordingToPriority(&newQue,processToBeSent);
                }
                else if(Algo == 2)
                {
                    AddAccordingToRemainingTime(&newQue,processToBeSent);
                }
                else
                {
                    Add(&newQue,processToBeSent);
                }
           }
          //  printf("current ID of the process after%d\n",processToBeSent.id);
            while(newQue.head != NULL)
            {
                DeQueue(&newQue,&processToBeSent);
                IPC(processToBeSent);
            }
            
            //sleep(5);
        }
         
       // printf("current time is %d     i=%d\n", y-x,i);
        //sleep(1);
    }
    //printf("Process generator finishes its work\n");
    while (1)
    {

    }
    
}


void ReadFile()
{
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    FILE *process = fopen("process.txt", "r");
    if (process == NULL)
    {
        printf("Error! File cannot be opened.");
        exit(1);
    }
    while (1)
    {
        char ignoredCharacter[1000];
        fscanf(process, "%s", ignoredCharacter);
        {
            if (*ignoredCharacter == '#')
            {
                fgets(ignoredCharacter, sizeof(ignoredCharacter), process);
                continue;
            }
            else
            {
                
                id = atoi(ignoredCharacter);
                fscanf(process, "%d %d %d", &arrivalTime, &runningTime, &priority);
                if (feof(process))
                    break;
                setPCB(&processToBeSent, id, arrivalTime, runningTime, priority);
                //printf("id %d, Arr time %d, Running time %d, Priority %d \n", processToBeSent.id, processToBeSent.ArrTime, processToBeSent.RunTime, processToBeSent.Priority);
                AddAccordingToArrivalTime(&que,processToBeSent);
                count++;
            }
        }
    }
    fclose(process);
}
void IPC(struct PCB processToBeSent) {
    int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
    if (pGeneratorToScheduler == -1){
        perror("error in creat");
        exit(-1);
    }
    struct msgBuff processInfo;
    processInfo.mtype = 10;
    CopyPCB(&processInfo.process, processToBeSent);
    int val = msgsnd(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), IPC_NOWAIT);
    if (val == -1)
        printf("Error in send process\n");
   // printf("at time %d A process with id %d has been sent\n",t,processInfo.process.id);
}
void clearResources(int signum) {
    //TODO Clears all resources in case of interruption
    
    destroyClk(true);
    exit(0);
}

void getAlgorithm() {
    printf("Choose the prefered Algorithm....\n");
    printf("1. HPF \n2. SRTN \n3. RR\n");
    scanf("%d", &Algo);

    while (!(Algo==1||Algo==2||Algo==3)) {
        printf("Choose A valid Number....\n");
        scanf("%d", &Algo);
        //sleep(3);
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
            printf("CLK forking...\n");
            execv("./clk.out",NULL);
            //sleep(10);
        }
        else{
            printf("scheduler forking...\n");
            char cSendAlgo[10];
            char cSendTime_quantum[10];
            char parentID[10];
            char Pcount[10];
            sprintf(cSendAlgo,"%d",Algo);
            sprintf(cSendTime_quantum,"%d",time_quantum);
            sprintf(parentID,"%d",getppid());
            sprintf(Pcount,"%d",count);
            char* scheduler_arg_list[]={"./scheduler.out",cSendAlgo,cSendTime_quantum,parentID,Pcount,0};
            execve(scheduler_arg_list[0],scheduler_arg_list,NULL); // all the processes should be sent by execve
        }
     }
 }
 
}

// notes
// we should have IO class that is resposnsible for all the IO processes

