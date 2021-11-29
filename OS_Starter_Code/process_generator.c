#include "headers.h"


//variables 
int Algo; 
int time_quantum;

//functions
void clearResources(int);
void getAlgorithm();
void Start_Clk_Scheduler();




int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    getAlgorithm();   
    // 3. Initiate and create the scheduler and clock processes.
    Start_Clk_Scheduler();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    //sleep(10);
    int x = getClk();
    printf("current time is %d\n", x);

       
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    while(1)
    {
   
    }
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}

void getAlgorithm()
{
    printf("Choose the prefered Algorithm....\n");
    printf("1. HPF \n2. SRTN \n3. RR\n");
    scanf("%d", &Algo);

    while (!(Algo==1||Algo==2||Algo==3))
    {
        printf("Choose A valid Number....\n");
        scanf("%d", &Algo);
        sleep(3);
    }
    if (Algo==3)
    {
        printf("Enter the quantum time....\n");
        scanf("%d", &time_quantum);
    }
    
    //char* Algo_String; 
    //char* time_quantum_String; 
   // sprintf(Algo_String, "%d", Algo);
   // sprintf(time_quantum_String, "%d", time_quantum);

}

void Start_Clk_Scheduler()
{
 int pid;
 for (int i=0;i<2;i++)
 {
     pid= fork();
     if (pid==0)
     {
        if (i==0)
        {
            printf("CLK forking...\n");
            execv("./clk.out",NULL);
            //sleep(10);
        }
        else
        {
            printf("scheduler forking...\n");
            char SendAlgo =Algo + '0';
            char * cSendAlgo = &SendAlgo;
            char SendTime_quantum =time_quantum + '0';
            char * cSendTime_quantum = &SendTime_quantum;
            char* scheduler_arg_list[]={"./scheduler.out",cSendAlgo,cSendTime_quantum,0};
            execve(scheduler_arg_list[0],scheduler_arg_list,NULL);
        }
     }
 }
 
}

// notes
// we should have IO class that is resposnsible for all the IO processes

