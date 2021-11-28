#include "headers.h"


int main(int argc, char * argv[])
{
    initClk();
    
    if (argc==1)
    {
        //HPF
    }
    else if (argc==2)
    {
        //STRN
    }
    else 
    {
        //RR
    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources
    
    destroyClk(true);
}
