#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
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
long minX = 0;
long maxX = 0;
long minZ = 0;
long maxZ = 0;
int threadC = 8;

long mincount = 30;
long medcount = 45;
long largecount = 50;
long extracount = 55;


FILE* min, *med, *large, *extra;

int main() {

    char input[1000] = {};

    char answer = 'e';

    fprintf(stdout,"What do you wanna calculate?\n\r"
                   "1) chunk grid\n\r"
                   "2) chunk count\n\r"
                   "e) exit\n\r");

    do{
        fprintf(stdout,"> ");
        fgets(input,1000,stdin);
        sscanf(input,"%c",&answer);
    }while (answer!='1' && answer!='2' && answer!='e');

    if(answer=='e')
        return 0;


    fprintf(stdout,"what's the world seed ?\n\r");

    do{
        fprintf(stdout,"> ");
        fgets(input,1000,stdin);
    }while (sscanf(input,"%ld",&seed)<=0);

    fprintf(stdout,"what's the minimum X ?\n\r");

    do{
        fprintf(stdout,"> ");
        fgets(input,1000,stdin);
    }while (sscanf(input,"%ld",&minX)<=0);

    fprintf(stdout,"what's the maximum X ?\n\r");

    do{
        fprintf(stdout,"> ");
        fgets(input,1000,stdin);
    }while (sscanf(input,"%ld",&maxX)<=0);

    fprintf(stdout,"what's the minimum Z ?\n\r");

    do{
        fprintf(stdout,"> ");
        fgets(input,1000,stdin);
    }while (sscanf(input,"%ld",&minZ)<=0);

    fprintf(stdout,"what's the maximum Z ?\n\r");

    do{
        fprintf(stdout,"> ");
        fgets(input,1000,stdin);
    }while (sscanf(input,"%ld",&maxZ)<=0);

    fprintf(stdout,"how many threads to start?\n\r");

    do{
        fprintf(stdout,"> ");
        fgets(input,1000,stdin);
    }while (sscanf(input,"%d",&threadC)<=0);


    switch (answer) {
        case '1':{
            chunks = malloc((maxX-minX)*sizeof(char*));

            for(long i = 0; i<(maxX-minX); i++)
                chunks[i] = calloc(maxZ-minZ,sizeof(char));

            pthread_t* threads;

            threads = calloc(threadC,sizeof(pthread_t));

            myData* data;
            for(int i=0 ; i<threadC; i++){
                data = calloc(1,sizeof(myData));
                data->minZ=minZ;
                data->maxZ=maxZ;
                data->minX=minX + i*(maxX-minX)/threadC;
                data->maxX=minX + (i+1)*(maxX-minX)/threadC + 1;
                pthread_create(&threads[i],NULL,findChunk,data);
            }

            for(int i=0 ; i<threadC; i++)
                pthread_join(threads[i],NULL);

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
            fclose(file);
        }
            break;
        case '2':
        {
            min = fopen("./min.txt","w+");
            med = fopen("./med.txt","w+");
            large = fopen("./large.txt","w+");
            extra = fopen("./extra.txt","w+");

            pthread_t* threads;

            threads = calloc(threadC,sizeof(pthread_t));

            myData* data;
            for(int i=0 ; i<threadC; i++){
                data = calloc(1,sizeof(myData));
                data->minX=minX + i*(maxX-minX)/threadC;
                data->maxX=minX + (i+1)*(maxX-minX)/threadC;
                pthread_create(&threads[i],NULL,countChunks,data);
            }

            for(int i=0 ; i<threadC; i++)
                pthread_join(threads[i],NULL);

            fclose(min);
            fclose(med);
            fclose(large);
            fclose(extra);
        }
            break;

    }





    return 0;
}

void* findChunk(void* args){
    myData* data = args;
    printf(" generating - x= [ %ld, %ld ] z= [ %ld , %ld]\n",data->minX,data->maxX,data->minZ,data->maxZ);
    for(long i=data->minX;i<data->maxX;i++)
        for(int j = data->minZ; j<data->maxZ; j++)
            chunks[i-minX][j-minZ]=isSlimeChunk(seed,i,j);
}


void* countChunks(void* args){
    myData* data = args;
    printf("searching - x= [ %ld, %ld ] z= [ %ld , %ld]\n",data->minX,data->maxX,data->minZ,data->maxZ);
    int count = 0;
    for(long i=data->minX; i<data->maxX; i++)
        for(long j = minZ; j < maxZ; j++)
        {
            count = 0;
            for (int dx = -8 ; dx <= 8; dx++){
                int delta = (int) sqrt(8 * 8 - dx * dx);
                for (int dz = -delta ; dz <= delta; dz++){
                    if(isSlimeChunk(seed, i + dx, j + dz))
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