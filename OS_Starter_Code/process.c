//#include <stdlib.h>
#include"headers.h"
#include <time.h>
void stophandler(int Sigint);
void Conthandler(int Sigint);
int pauseClk;
int ContinueClk;
bool stoped;
int main(int argc, char * argv[])
{
    /*signal(SIGSTOP,stophandler);
    signal(SIGCONT,Conthandler);*/
    stoped = false;
    pauseClk = 0;
    ContinueClk = 0;
    while(clock() - ContinueClk < atoi(argv[1]) * CLOCKS_PER_SEC && !stoped);
    //printf("I am done at time = %d \n",clock()/CLOCKS_PER_SEC);
    return 0;
}

void stophandler(int Sigint)
{
    printf("I will stop\n");
    stoped = true;
    pauseClk = clock();
}

void Conthandler(int Sigint)
{
    stoped = false;
    ContinueClk = ContinueClk + clock() - pauseClk;
    printf("I will continue howver i paused for %d\n", ContinueClk / CLOCKS_PER_SEC);
}