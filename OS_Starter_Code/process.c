//#include <stdlib.h>
#include"headers.h"
#include <time.h>
//void stophandler(int Sigint);
void Conthandler(int Sigint);
int pauseClk;
clock_t ContinueClk;
bool stoped;
clock_t x;
int main(int argc, char * argv[])
{
    signal(SIGCONT,Conthandler);
    stoped = false;
    pauseClk = 0;
    ContinueClk = 0;
    x = clock();
    while(x - ContinueClk < atoi(argv[1]) * CLOCKS_PER_SEC && !stoped)
    {
        x = clock();
    }
    //printf("I am done at time = %d \n",clock()/CLOCKS_PER_SEC);
    return 0;
}

/*void stophandler(int Sigint)
{
    printf("I will stop\n");
    stoped = true;
    pauseClk = clock();
}*/

void Conthandler(int Sigint)
{
    stoped = false;
    ContinueClk = ContinueClk + clock() - x;
    //printf("I will continue howver i paused for %d\n", ContinueClk / CLOCKS_PER_SEC);
}