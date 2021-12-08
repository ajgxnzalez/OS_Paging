#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// AJ Gonzalez
// OS - Allen Gottlieb
// Lab 2: Paging

// Setting up queue structure and correspoinding methods
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};

struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(
        sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
 
    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(
        queue->capacity * sizeof(int));
    return queue;
}

int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

int contains(struct Queue* queue, int item){
    for(int i = 0; i < queue->size; i++){
        if(item == queue->array[i]){
            return 1; // true
        }
    }

    return 0; // false
}


void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)
                  % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    //printf("%d enqueued to queue\n", item);
}

int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)
                   % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;
    return queue->array[queue->front];
}
 
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;
    return queue->array[queue->rear];
}

int main(){

    // Part 1 - generate 1,000 random long numbers
    int i;
    int pageRefs[1000];

    for(i = 0; i < 1000; i++){
        int num = random();
        // Reduction #1
        num = num % 100;

        // Reduction #2
        if(num >= 0 && num < pow(1, 2)){
            pageRefs[i] = 0;
        } else if (num >= 1 && num < pow(2, 2)){
            pageRefs[i] = 1;
        } else if (num >= 4 && num < pow(3, 2)){
            pageRefs[i] = 2;
        } else if (num >= 9 && num < pow(4, 2)){
            pageRefs[i] = 3;
        } else if (num >= 16 && num < pow(5, 2)){
            pageRefs[i] = 4;
        } else if (num >= 25 && num < pow(6, 2)){
            pageRefs[i] = 5;
        } else if (num >= 36 && num < pow(7, 2)){
            pageRefs[i] = 6;
        } else if (num >= 49 && num < pow(8, 2)){
            pageRefs[i] = 7;
        } else if (num >= 64 && num < pow(9, 2)){
            pageRefs[i] = 8;
        } else if (num >= 81 && num < pow(10, 2)){
            pageRefs[i] = 9;
        }
        //printf("%d\n", pageRefs[i]);
    }
    
    // Get algorithmic method
    printf("Choose replacement algorithm: 0 - FIFO, 1 - LRU, 2 - Second Chance \n");
    char getAlg[100];
    scanf("%s", getAlg);
    int alg = atoi(getAlg);

    // Get number of frames (k)
    printf("Enter the desired number of frames (k): ");
    int k;
    scanf("%d", &k);
    

    // Set up miss & evict & hit
    int numMisses = 0;
    int numHits = 0;
    int numEvicts = 0;
    int totalFrames;
    int framesLeft = k;
    int framesSimulated;
    int timesRef[10];
    for(int x = 0; x < 10; x++){
        timesRef[x] = 0;
    }

    if (alg == 0){
        // FIFO
        printf("Replacement Method Used: FIFO\n");
        struct Queue * allPages = createQueue(1000);
        struct Queue * allFrames = createQueue(k);

        int j;
        for(j = 0; j < 1000; j++){
            
            int temp = pageRefs[j];
            timesRef[temp] += 1;

            if (contains(allFrames, temp)){
                // Page is inside the current frames
                numHits++;

            } else {
                numMisses++;
                if(framesLeft > 0){
                    // there is still available space
                    enqueue(allFrames, temp);
                    framesLeft--;
                    framesSimulated++;

                } else if (framesLeft == 0){
                    // eviction 
                    dequeue(allFrames);
                    numEvicts++;
                    enqueue(allFrames, temp);
                    framesSimulated++;

                }
            }
        }
        
        printf("Number of hits: %d\n", numHits);
        printf("Number of misses: %d\n", numMisses);
        printf("Number of evicts: %d\n", numEvicts);
        for(int x = 0; x < 10; x++){
            printf("Page num is %d and the number of references is %d\n", x, timesRef[x]);
        }



    }else if (alg == 1){
        // LRU
        printf("Replacement Method Used: LRU\n");

        int allFrames[k]; // array of size k to represent all the empty frames
        int tempHits[k]; // array to keep track of how often each frame is reffed

        // set all frames to -1 to show they're empty
        for(int x = 0; x < k; x++){
            allFrames[x] = -1;
        }

        // set all original refs to 0
        for(int i = 0; i < k; i++){
            tempHits[i] = 0;
        }
    
        // loop through the 1000 page refs
        for(int i = 0; i < 1000; i++){

            int temp = pageRefs[i];
            timesRef[temp] += 1;
            int check = 0;

            int j = 0;
            while (j < k && allFrames[j] != -1){
                
                if(temp == allFrames[j]){
                    numHits++;
                    check = 1;
                    tempHits[j] = 0;
                    for(int y = 0; y < k; y++){
                        if(y != j){
                            tempHits[y] += 1;
                        }
                    }
                    break;
                } else {
                    j++;
                }
    
            }
        
            if(allFrames[j] == -1 && framesLeft > 0){
                numMisses++;
                framesLeft--;
                allFrames[j] = temp;
                
            } else if (framesLeft == 0 && check == 0){
                numEvicts++;
                numMisses++;
                int indexOfEvict = 0;
                int time = 0;
                for(int x = 0; x < k; x++){
                    if(tempHits[x] > time){
                        time = tempHits[x];
                        indexOfEvict = x;
                    }
                }
                tempHits[indexOfEvict] = 0;
                allFrames[indexOfEvict] = temp;

                for(int y = 0; y < k; y++){
                    if(y != indexOfEvict){
                        tempHits[y] += 1;
                    }
                }

            }
        }

        printf("Number of hits: %d\n", numHits);
        printf("Number of misses: %d\n", numMisses);
        printf("Number of evicts: %d\n", numEvicts);
        for(int x = 0; x < 10; x++){
            printf("Page num is %d and the number of references is %d\n", x, timesRef[x]);
        }
    

    }else if (alg == 2){
        // Second Chance
        printf("Replacement Method Used: Second Chance\n");
        int allFrames[k]; // array of size k to represent all the empty frames
        int tempHits[k]; // array to keep track of how often each frame is reffed

        // set all frames to -1 to show they're empty
        for(int x = 0; x < k; x++){
            allFrames[x] = -1;
        }

        // set all original refs to 0
        for(int i = 0; i < k; i++){
            tempHits[i] = 0;
        }
        
        int rbits[10]; // set up R bits per page
        for(int i = 0; i < 10; i++){
            rbits[i] = 1; // 1 = rbit is set, 0 = rbit is cleared
        }

        for(int i = 0; i < 1000; i++){

            if(i % 100 == 0){
                // reset the rbits every 100
                for(int x = 0; x < 10; x++){
                    rbits[x] = 0; // 1 = rbit is set, 0 = rbit is cleared
                }
            }
            
            int temp = pageRefs[i];
            timesRef[temp] += 1;

            // hit, miss, evict logic
            int check = 0;

            int j = 0;
            while (j < k && allFrames[j] != -1){
                
                if(temp == allFrames[j]){
                    numHits++;
                    check = 1;
                    rbits[temp] = 1; // set r bit
                    tempHits[j] = 0;
                    for(int y = 0; y < k; y++){
                        if(y != j){
                            tempHits[y] += 1;
                        }
                    }
                    break;
                } else {
                    j++;
                }
    
            }

            if(allFrames[j] == -1 && framesLeft > 0){
                numMisses++;
                framesLeft--;
                allFrames[j] = temp;
                rbits[temp] = 0; // clear r bit
                
            } else if (framesLeft == 0 && check == 0){
                if(k == 0){
                    numEvicts++;
                    numMisses++;
                }else {

                    for(int y = 0; y < k; y++){

                        // determine which frame hasnt been used in the longest
                        int indexOfEvict = 0;
                        int time = 0;
                        for(int x = 0; x < k; x++){
                            if(tempHits[x] > time){
                                time = tempHits[x];
                                indexOfEvict = x;
                            }
                        }
                        tempHits[indexOfEvict] = 0;
                        for(int z = 0; z < k; z++){
                            if(z != indexOfEvict){
                                tempHits[z] += 1;
                            }
                        }

                        if(rbits[allFrames[indexOfEvict]] == 1){
                            rbits[allFrames[indexOfEvict]] = 0; // clear r bit
                        }else if (rbits[allFrames[indexOfEvict]] == 0){
                            numEvicts++;
                            numMisses++;
                            allFrames[indexOfEvict] = temp;
                            for(int x = 0; x < 10; x++){
                                rbits[x] = 0; // page fault, all r bits get cleared
                            }
                            //rbits[allFrames[indexOfEvict]] = 0; // new page stored, start r bit at 0
                            break;
                        }
                    }
                }

            }

        }

        printf("Number of hits: %d\n", numHits);
        printf("Number of misses: %d\n", numMisses);
        printf("Number of evicts: %d\n", numEvicts);
        for(int x = 0; x < 10; x++){
            printf("Page num is %d and the number of references is %d\n", x, timesRef[x]);
        }

    }


    return 1;
}


