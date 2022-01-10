#include <stdio.h>
#include <math.h>

#define memSize 1024

int memory[memSize];

struct space
{
    int start, size;
};
struct space freeSlots[memSize];

void dealloc(int id)
{
    int found = -1;
    int start = -1;
    int size = -1;
    for (int i = 0; i < memSize; i++)
    {
        if (memory[i] == id)
        {
            found = i;
            start = i;
            //printf("id found at location %d\n", start);
            break;
        }
    }
    if (found == -1)
        printf("No allocated mem found for this process\n");
    else
    {
        while (found >= 0)
        {
            if (memory[found] == id)
            {
                memory[found] = -1;
                if (found < memSize)
                    found++;
            }
            if (memory[found] != id)
            {

                size = found - start;
                printf("At time %d mem freed %d bytes from process %d from %d to %d\n", 3, size, id, start, found - 1);
                found = -1;
            }
        }
        // struct space FreedLoc;
        //FreedLoc.start=start;
        //FreedLoc.size=found-start;
        freeSlots[start].size = size;
        freeSlots[start].start = start;
        // merge slots
        int i = 0;
        //for (i = 0; i < memSize; i++)
        while (i < memSize)
        {
            if (freeSlots[i].size != -1)
            {
                int targetLoc = freeSlots[i].size + freeSlots[i].start;
                if (targetLoc < memSize && freeSlots[targetLoc].size == freeSlots[i].size)
                {
                    if (log2(freeSlots[targetLoc].start + freeSlots[targetLoc].size) == ceil(log2(freeSlots[targetLoc].start + freeSlots[targetLoc].size)))
                    {
                        freeSlots[targetLoc].size = -1;
                        freeSlots[targetLoc].start = -1;
                        freeSlots[i].size = 2 * freeSlots[i].size;
                        i = i - freeSlots[i].size - 1;
                    }
                }
            }
            i++;
        }
    }
    ///TODO: merge the adjecnt free slots in the freeSlots arr
    //Note: you might fing them in sperate locations not just adjecnt
}

void allocate(int pSize, int id)
{
    int size = 0;
    int neededsize;
    int startSize = memSize;

    // find  if the smallest location size
    neededsize = pow(2, ceil(log(pSize) / log(2)));
    //printf("The needed size for this process is %d\n",neededsize);
    int smallestSizeFound = memSize + 1;
    int startOFsamallestSize = -1;
    int indexOfFreeLoc = -1;
    for (int i = 0; i < memSize; i++)
    {
        if (freeSlots[i].size >= neededsize && freeSlots[i].size < smallestSizeFound)
        {
            smallestSizeFound = freeSlots[i].size;
            startOFsamallestSize = freeSlots[i].start;
            indexOfFreeLoc = i;
        }
    }
    if (startOFsamallestSize == -1)
    {
        // no place to allocate
        printf("No free location found\n");
    }
    else
    {
        // check if the size found is need to be divided or not
        // if yes
        //divide
        while (smallestSizeFound > neededsize)
        {
            // divide the greater size and place it in the free slots
            // change the parameters of the current index
            freeSlots[indexOfFreeLoc].size = freeSlots[indexOfFreeLoc].size / 2;
            // and add the other half to the free Slot index
            struct space temp;
            temp.size = freeSlots[indexOfFreeLoc].size;
            temp.start = freeSlots[indexOfFreeLoc].start + temp.size;
            // add the new slot to the FreeSlots arr
            freeSlots[temp.start].size = temp.size;
            freeSlots[temp.start].start = temp.start;

            smallestSizeFound = freeSlots[indexOfFreeLoc].size;
        }
        // allocate the mem and remove the allocated place from the free slots list
        //actual allocation
        for (int i = freeSlots[indexOfFreeLoc].start; i < freeSlots[indexOfFreeLoc].size + freeSlots[indexOfFreeLoc].start; i++)
        {
            memory[i] = id;
        }
        printf("At time %d allocated %d bytes for proccess %d from %d to %d\n", 4, freeSlots[indexOfFreeLoc].size, id, freeSlots[indexOfFreeLoc].start, freeSlots[indexOfFreeLoc].size + freeSlots[indexOfFreeLoc].start - 1);
        // remove it from the freeSlots list
        freeSlots[indexOfFreeLoc].size = -1;
        freeSlots[indexOfFreeLoc].start = -1;
    }
}
void printFreeSlots()
{
    for (int i = 0; i < memSize; i++)
    {
        if (freeSlots[i].size > 0)
            printf("free location at start: %d  and size %d\n", freeSlots[i].start, freeSlots[i].size);
    }
}
int main()
{
    freeSlots[0].start = 0;
    freeSlots[0].size = memSize;
    for (int i = 1; i < memSize; i++)
    {
        freeSlots[i].start = -1;
        freeSlots[i].size = -1;
    }
    // intalizing of free arr;
    for (int i = 0; i < memSize; i++)
    {
        memory[i] = -1;
    }

    // for test only
    // allocate mem loca from 0 to 127 with id 1

    int x = 0;
    allocate(10, 1);
    printFreeSlots();
    allocate(15, 2);
    printFreeSlots();
    allocate(10, 3);
    printFreeSlots();
    allocate(25, 4);
    printFreeSlots();

    dealloc(2);
    printFreeSlots();
    dealloc(3);
    printFreeSlots();
  /*printf("************************************\n");
    allocate(8, 5);
    printFreeSlots();
    allocate(30, 6);
    printFreeSlots();
    dealloc(2);
    printFreeSlots();
    allocate(15, 7);
    printFreeSlots();
    dealloc(5);
    printFreeSlots();
    dealloc(6);
    printFreeSlots();
    dealloc(7);
    printFreeSlots();
*/
    //dealloc(3);
    //dealloc(4);
    return 0;
}
