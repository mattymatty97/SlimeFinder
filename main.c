#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
char isSlimeChunk(long seed, long Xcoord, long Zcoord);
void* findChunk(void* args);
void* countChunks(void* args);

char** chunks = 0;

typedef struct {
    long maxX;
    long minX;
    long maxZ;
    long minZ;
} myData;

long seed = -9094455790421298813;
long minX = -10000;
long maxX = 10000;
long minZ = -10000;
long maxZ = 10000;

long mincount = 30;
long medcount = 45;
long largecount = 50;
long extracount = 55;


FILE* min, *med, *large, *extra;

int main() {
/*
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

*/

    min = fopen("./min.txt","w+");
    med = fopen("./med.txt","w+");
    large = fopen("./large.txt","w+");
    extra = fopen("./extra.txt","w+");


    myData* data;
    pthread_t threads[8] = {};

    data = calloc(1,sizeof(myData));
    data->minX=minX;
    data->maxX=minX + (maxX-minX)/8;
    pthread_create(&threads[0],NULL,countChunks,data);

    data = calloc(1,sizeof(myData));
    data->minX=minX + ((maxX-minX)/8) +1;
    data->maxX=minX + 2*((maxX-minX)/8);
    pthread_create(&threads[1],NULL,countChunks,data);

    data = calloc(1,sizeof(myData));
    data->minX=minX + 2*((maxX-minX)/8) +1;
    data->maxX=minX + 3*((maxX-minX)/8);
    pthread_create(&threads[2],NULL,countChunks,data);

    data = calloc(1,sizeof(myData));
    data->minX=minX + 3*((maxX-minX)/8) +1;
    data->maxX=minX + 4*((maxX-minX)/8);
    pthread_create(&threads[3],NULL,countChunks,data);

    data = calloc(1,sizeof(myData));
    data->minX=minX + 4*((maxX-minX)/8) +1;
    data->maxX=minX + 5*((maxX-minX)/8);
    pthread_create(&threads[4],NULL,countChunks,data);

    data = calloc(1,sizeof(myData));
    data->minX=minX + 5*((maxX-minX)/8) +1;
    data->maxX=minX + 6*((maxX-minX)/8);
    pthread_create(&threads[5],NULL,countChunks,data);

    data = calloc(1,sizeof(myData));
    data->minX=minX + 6*((maxX-minX)/8) +1;
    data->maxX=minX + 7*((maxX-minX)/8);
    pthread_create(&threads[6],NULL,countChunks,data);

    data = calloc(1,sizeof(myData));
    data->minX=minX + 7*((maxX-minX)/8) +1;
    data->maxX=maxX;
    pthread_create(&threads[7],NULL,countChunks,data);


    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);
    pthread_join(threads[2],NULL);
    pthread_join(threads[3],NULL);
    pthread_join(threads[4],NULL);
    pthread_join(threads[5],NULL);
    pthread_join(threads[6],NULL);
    pthread_join(threads[7],NULL);

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
    int count = 0;
    for(long i=data->minX; i<data->maxX; i++)
        for(long j = minZ; j < maxZ; j++)
        {
            count = 0;
            for (long k = i-8 ; k < i+8 ; ++k) {
                for (long l = j-8 ; l < j+8 ; ++l) {
                    if(isSlimeChunk(seed,k,l))
                        ++count;
                }
            }
            if( count > mincount ) {
                printf("count is %d , center chunk X= %ld Z= %ld\n", count, i, j);
                fprintf(min,"%ld %ld count: %d\n",i,j,count);
            }
            if(count > medcount){
                fprintf(med,"%ld %ld count: %d\n",i,j,count);
            }
            if(count > largecount){
                fprintf(large,"%ld %ld count: %d\n",i,j,count);
            }
            if(count > extracount){
                fprintf(extra,"%ld %ld count: %d\n",i,j,count);
            }
        }
}





char isSlimeChunk(long seed, long Xcoord, long Zcoord){
    return (
                   (int) (
                           (unsigned long)(
                                   (
                                           (
                                                   (
                                                           (seed +
                                                            (int) (Xcoord * Xcoord * (long)0x4C1906) +
                                                            (int) (Xcoord * (long)0x5AC0DB) +
                                                            (int) (Zcoord * Zcoord) * (long)0x4307A7 +
                                                            (int) (Zcoord * (long)0x5F24F) ^ (long)0x3AD8025F
                                                           ) ^ (long)0x5DEECE66D
                                                   ) & ((1L << 48) - 1)
                                           ) * (long)0x5DEECE66D
                                   ) & ((1L << 48) - 1)
                           ) >> 17
                   ) % 10
           ) == 0;
}