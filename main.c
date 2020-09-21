#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

char isSlimeChunk(long Xcoord, long Zcoord);
void* findChunk(void* args);
void* countChunks(void* args);
void signalHandler(int signum);

char** chunks = 0;

typedef struct {
    long maxX;
    long minX;
    long maxZ;
    long minZ;
    long x;
    long z;
    int me;
} myData;

long seed = -9094455790421298813;
long minX = 0;
long maxX = 0;
long minZ = 0;
long maxZ = 0;
int threadC = 8;
char savepath[1000] = ".";

const long mincount = 30;
const long medcount = 45;
const long largecount = 50;
const long extracount = 55;

long total = 0;
long actual = 0;


FILE* min, *med, *large, *extra, *resume;

int interrupt = -1;

int main() {

    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        perror("error: ");
        return -3 ;
    }
    if (signal(SIGTERM, signalHandler) == SIG_ERR) {
        perror("error: ");
        return -3 ;
    }

    char input[1000] = {};

    char answer = 'e';

    char mode[] = "w+";

    if ((resume = fopen("resume.slime", "r")))
    {
        fprintf(stdout,"found an incomplete task, do yo want to resume it? [y/n]\n\r");

        do{
            fprintf(stdout,"> ");
            fgets(input,1000,stdin);
            fflush(stdin);
            sscanf(input,"%c",&answer);
        }while (answer!='y' && answer!='n');

        if( answer == 'y'){
            fgets(input,1000,resume);
            sscanf(input,"seed= %ld",&seed);
            fgets(input,1000,resume);
            sscanf(input, "x= [ %ld , %ld ] z= [ %ld , %ld ]\n", &minX, &maxX, &minZ, &maxZ);
            fgets(input,1000,resume);
            sscanf(input, "threads= %d\n", &threadC);
            fgets(input,1000,resume);
            sscanf(input, "path= %s\n", savepath);
            strncpy(savepath,savepath,strlen(savepath)-1);
            answer = '2';
            mode[0] = 'a';
        }else{
            fclose(resume);
            resume = NULL;
            remove("resume.slime");
        }
    }

    if(!resume) {

        fprintf(stdout, "What do you wanna calculate?\n\r"
                        "1) chunk grid\n\r"
                        "2) chunk count\n\r"
                        "e) exit\n\r");

        do {
            fprintf(stdout, "> ");
            fgets(input, 1000, stdin);
            fflush(stdin);
            sscanf(input, "%c", &answer);
        } while (answer != '1' && answer != '2' && answer != 'e');

        if (answer == 'e')
            return 0;


        fprintf(stdout, "what's the world seed ?\n\r");

        do {
            fprintf(stdout, "> ");
            fgets(input, 1000, stdin);
            fflush(stdin);
        } while (sscanf(input, "%ld", &seed) <= 0);

        fprintf(stdout, "what's the minimum X ?\n\r");

        do {
            fprintf(stdout, "> ");
            fgets(input, 1000, stdin);
            fflush(stdin);
        } while (sscanf(input, "%ld", &minX) <= 0);

        fprintf(stdout, "what's the maximum X ?\n\r");

        do {
            fprintf(stdout, "> ");
            fgets(input, 1000, stdin);
            fflush(stdin);
        } while (sscanf(input, "%ld", &maxX) <= 0);

        fprintf(stdout, "what's the minimum Z ?\n\r");

        do {
            fprintf(stdout, "> ");
            fgets(input, 1000, stdin);
            fflush(stdin);
        } while (sscanf(input, "%ld", &minZ) <= 0);

        fprintf(stdout, "what's the maximum Z ?\n\r");

        do {
            fprintf(stdout, "> ");
            fgets(input, 1000, stdin);
            fflush(stdin);
        } while (sscanf(input, "%ld", &maxZ) <= 0);

        fprintf(stdout, "how many threads to start?\n\r");

        do {
            fprintf(stdout, "> ");
            fgets(input, 1000, stdin);
            fflush(stdin);
        } while (sscanf(input, "%d", &threadC) <= 0);

        fprintf(stdout, "save Folder Path? (default= current Directory)\n\r");

        strcpy(input, ".");

        fprintf(stdout, "> ");
        if(fgets(input, 1000, stdin)>0)
            strncpy(savepath, input,strlen(input)-1);
        fflush(stdin);

        struct stat st = {0};


        if (stat(savepath, &st) == -1) {
            if (mkdir(savepath, 0700) != 0) {
                perror("error: ");
                return -1;
            }
        }
    }

    total = (maxX-minX) * (maxZ - minZ);

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
                data->me=i;
                data->minZ=minZ;
                data->maxZ=maxZ;
                data->minX=minX + i*(maxX-minX)/threadC;
                data->maxX=minX + (i+1)*(maxX-minX)/threadC + 1;
                pthread_create(&threads[i],NULL,findChunk,data);
            }

            for(int i=0 ; i<threadC; i++)
                pthread_join(threads[i],NULL);

            char filename[1000] = {};
            strcpy(filename, savepath);
            strcat(filename, "/tmp.txt");

            FILE* file = fopen(filename,mode);
            if(file == NULL){
                perror("error:");
                return 1;
            }

            for(long i = 0; i<(maxX-minX); i++) {
                for (long j = 0; j < (maxZ - minZ); j++)
                    fprintf(file, "%c", (chunks[i][j]) ? 'X' : ' ');
                fprintf(file, "\n");
            }
            fclose(file);
        }
            break;
        case '2':
        {

            char filename[1000] = {};
            strcpy(filename, savepath);
            strcat(filename, "/min.txt");

            min = fopen(filename,mode);

            if(min==NULL){
                perror("error on min:");
                exit(-11);
            }

            strcpy(filename, savepath);
            strcat(filename, "/med.txt");
            med = fopen(filename,mode);

            if(med==NULL){
                perror("error on med:");
                exit(-11);
            }

            strcpy(filename, savepath);
            strcat(filename, "/large.txt");
            large = fopen(filename,mode);


            if(large==NULL){
                perror("error on large:");
                exit(-11);
            }

            strcpy(filename, savepath);
            strcat(filename, "/extra.txt");
            extra = fopen(filename,mode);


            if(extra==NULL){
                perror("error on extra:");
                exit(-11);
            }

            pthread_t* threads;

            threads = calloc(threadC,sizeof(pthread_t));

            myData* data;
            for(int i=0 ; i<threadC; i++){
                data = calloc(1, sizeof(myData));
                if(resume){
                    fgets(input,1000,resume);
                    sscanf(input, "x= [ %ld , %ld ] , x= %ld z= %ld\n", &(data->minX), &(data->maxX), &(data->x), &(data->z));
                    data->me = i;
                }else {
                    data->me = i;
                    data->minX = minX + i * (maxX - minX) / threadC;
                    data->maxX = minX + (i + 1) * (maxX - minX) / threadC;
                }
                pthread_create(&threads[i],NULL,countChunks,data);
            }

            if(resume){
                long old = 0;
                fgets(input,1000,resume);
                sscanf(input, "count= %ld\n", &old);
                __sync_fetch_and_add(&actual,old);
                fclose(resume);
                resume=NULL;
            }

            for(int i=0 ; i<threadC; i++)
                pthread_join(threads[i],NULL);

            if(resume){
                fprintf(resume,"count= %ld",actual);
            }

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
            chunks[i-minX][j-minZ]=isSlimeChunk(i,j);
}


void* countChunks(void* args){
    myData* data = args;
    printf("searching - x= [ %ld, %ld ]\n",data->minX,data->maxX);
    int count;
    for(long i=(data->x>0)?data->x:data->minX; i<data->maxX; i++) {
        data->x = 0;
        for (long j = (data->z > 0) ? data->z : minZ; j < maxZ; j++) {
            data->z = 0;
            count = 0;
            for (int dx = -8; dx <= 8; dx++) {
                int delta = (int) sqrt(8 * 8 - dx * dx);
                for (int dz = -delta; dz <= delta; dz++) {
                    if (isSlimeChunk(i + dx, j + dz))
                        ++count;
                }
            }
            long val = __sync_add_and_fetch(&actual, 1L);
            if (count > mincount) {
                printf("\rcount is %d , center chunk X= %ld Z= %ld\n", count, i, j);
                fprintf(min, "%ld %ld count: %d\n", i, j, count);
            }
            if (count > medcount) {
                fprintf(med, "%ld %ld count: %d\n", i, j, count);
            }
            if (count > largecount) {
                fprintf(large, "%ld %ld count: %d\n", i, j, count);
            }
            if (count > extracount) {
                fprintf(extra, "%ld %ld count: %d\n", i, j, count);
            }
            fprintf(stdout, "\rprogress = %.2f%% (%ld/%ld)", (val / (float) total) * 100, val, total);
            fflush(stdout);

            if (interrupt == data->me) {
                fprintf(resume, "x= [ %ld , %ld ] , x= %ld z= %ld\n", data->minX, data->maxX, i, j);
                interrupt--;
                return NULL;
            }
        }
    }
}

char isSlimeChunk(long Xcoord, long Zcoord){
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

void signalHandler(int signum){
    resume = fopen("resume.slime", "w+");
    if(min==NULL)
        exit(signum);
    if (resume == NULL)
        exit(-4);

    fprintf(resume, "seed= %ld\n", seed);
    fprintf(resume, "x= [ %ld , %ld ] z= [ %ld , %ld ]\n", minX, maxX, minZ, maxZ);
    fprintf(resume, "threads= %d\n", threadC);
    fprintf(resume, "path= %s\n", savepath);

    interrupt=threadC-1;
}