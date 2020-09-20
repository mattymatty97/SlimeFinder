#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
char isSlimeChunk(long seed, long Xcoord, long Zcoord);
void* findChunk(void* args);

char** chunks = 0;

typedef struct {
    long maxX;
    long minX;
    long maxZ;
    long minZ;
} myData;

long seed = -9094455790421298813;
long minX = -1000;
long maxX = 1000;
long minZ = -1000;
long maxZ = 1000;

int main() {

    chunks = malloc((maxX-minX)*sizeof(char*));

    for(long i = 0; i<(maxX-minX); i++)
        chunks[i] = calloc(maxZ-minZ,sizeof(char));

    myData* data;
    pthread_t threads[4] = {};
    data = calloc(1,sizeof(myData));
    data->minZ=minZ;
    data->maxZ=0;
    data->minX=minX;
    data->maxX=0;
    pthread_create(&threads[0],NULL,findChunk,data);

    data = calloc(1,sizeof(myData));
    data->minZ=0;
    data->maxZ=maxZ;
    data->minX=minX;
    data->maxX=0;
    pthread_create(&threads[1],NULL,findChunk,data);

    data = calloc(1,sizeof(myData));
    data->minZ=minZ;
    data->maxZ=0;
    data->minX=0;
    data->maxX=maxX;
    pthread_create(&threads[2],NULL,findChunk,data);

    data = calloc(1,sizeof(myData));
    data->minZ=0;
    data->maxZ=maxZ;
    data->minX=0;
    data->maxX=maxX;
    pthread_create(&threads[3],NULL,findChunk,data);

    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);
    pthread_join(threads[2],NULL);
    pthread_join(threads[3],NULL);

    FILE* file = fopen("./tmp.txt","w+");
    if(file==NULL){
        perror("error:");
        return 1;
    }

    for(long i = 0; i<(maxX-minX); i++) {
        for (long j = 0; j < (maxZ - minZ); j++)
            fprintf(file, "%c", (chunks[i][j]) ? 'X' : ' ');
        fprintf(file,"\n");
    }






    return 0;
}

void* findChunk(void* args){
    myData* data = args;
    printf(" generating - minx: %ld maxX: %ld minZ %ld maxZ: %ld\n",data->minX,data->maxX,data->minZ,data->maxZ);
    for(long i=data->minX;i<data->maxX;i++)
        for(int j = data->minZ; j<data->maxZ; j++)
            chunks[i-minX][j-minZ]=isSlimeChunk(seed,i,j);
}


void* countChunks(void* args){
    myData* data = args;
    printf("searching - minx: %ld maxX: %ld minZ %ld maxZ: %ld\n",data->minX,data->maxX,data->minZ,data->maxZ);
    for(long i=data->minX;i<data->maxX;i++)
        for(int j = minZ + 12; j<maxZ +12; j++)
            chunks[i-minX][j-minZ]=isSlimeChunk(seed,i,j);
}





char isSlimeChunk(long seed, long Xcoord, long Zcoord){
    return (
                   (int) (
                           (unsigned long)(
                                   (
                                           (
                                                   (
                                                           (seed +
                                                            (int) (Xcoord * Xcoord * 0x4C1906) +
                                                            (int) (Xcoord * 0x5AC0DB) +
                                                            (int) (Zcoord * Zcoord) * 0x4307A7 +
                                                            (int) (Zcoord * 0x5F24F) ^ 0x3AD8025F
                                                           ) ^ 0x5DEECE66D
                                                   ) & ((1L << 48) - 1)
                                           ) * 0x5DEECE66D
                                   ) & ((1L << 48) - 1)
                           ) >> 17
                   ) % 10
           ) == 0;
}