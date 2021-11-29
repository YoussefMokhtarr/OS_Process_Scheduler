#include "headers.h"


//data
#define hpf_Algo 1
#define strn_Algo 2
#define rr_Algo 3
int Algo;
int time_quantum;
//functions
void HPF();
void STRN();
void RR();
//functions
int main(int argc, char * argv[])
{
    initClk();
    Algo=argv[1][0]-'0';
    time_quantum=argv[2][0]-'0';
    //char* time_quantum=argv[1];
    printf("scheduler Arguments%d    %d\n",Algo, time_quantum);
    // conver the arv[] to integers
    if (Algo==1)
    printf("Algo is integer");
    else 
    printf("there is an error");
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
    
    destroyClk(true);
}



void HPF()  // check the return type of the alogrithms
{
    printf("The user chose to run HPF");
}
void STRN()
{
    printf("The user chose to run STRN");
}
void RR()
{
    printf("The user chose to run RR");   
}