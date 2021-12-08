#include"PriorityQueue.h"

void enQueueAndGenerate();

int main()
{
    printf("hello\n");
    struct PCB a;
    setPCB(&a, 1,9,2,1);
    struct PCB b;
    setPCB(&b, 2,6,1,5);
    struct PCB c;
    setPCB(&c, 3,3,3,0);

    struct PriorityQueue que;

    initializeQueue(&que);

    struct PCBNode New = GenerateNode(a);
    enQueue(&que,&New);
    struct PCBNode Newb = GenerateNode(b);
    enQueue(&que,&Newb);
    struct PCBNode Newc = GenerateNode(c);
    enQueue(&que,&Newc);

    struct PCB d;
    DeQueue(&que,&d);
    printf("%d\t",d.id); 
    DeQueue(&que,&d);
    printf("%d\t",d.id);
    DeQueue(&que,&d);
    printf("%d\n",d.id);

    printf("\n\n");

    struct PriorityQueue que2;

    New.next = NULL;
    Newb.next = NULL;
    Newc.next = NULL;
    InsertAccordingToPriority(&que2,&New);
    InsertAccordingToPriority(&que2,&Newb);
    InsertAccordingToPriority(&que2,&Newc);
    
    DeQueue(&que2,&d);
    printf("%d\t",d.id); 
    DeQueue(&que2,&d);
    printf("%d\t",d.id);
    DeQueue(&que2,&d);
    printf("%d\n",d.id);

    printf("\n\n");

    struct PriorityQueue que3;

    New.next = NULL;
    Newb.next = NULL;
    Newc.next = NULL;
    InsertAccordingToArrivalTime(&que3,&New);
    InsertAccordingToArrivalTime(&que3,&Newb);
    InsertAccordingToArrivalTime(&que3,&Newc);
    
    DeQueue(&que3,&d);
    printf("%d\t",d.id); 
    DeQueue(&que3,&d);
    printf("%d\t",d.id);
    DeQueue(&que3,&d);
    printf("%d\n",d.id);

    printf("\n\n");

    struct PriorityQueue que4;

    New.next = NULL;
    Newb.next = NULL;
    Newc.next = NULL;
    InsertAccordingToReaminingTime(&que4,&New);
    InsertAccordingToReaminingTime(&que4,&Newb);
    InsertAccordingToReaminingTime(&que4,&Newc);
    
    DeQueue(&que4,&d);
    printf("%d\t",d.id); 
    DeQueue(&que4,&d);
    printf("%d\t",d.id);
    DeQueue(&que4,&d);
    printf("%d\n",d.id);
    

}