#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#define high_code_stram  0x000000FA
#define low_code_stram   0x000000FB
#define audio_code_stram 0x000000FC


#define LOG(rank,M, ...)  { \
    fprintf(stdout, " [RANK%d  %d]: " M "", rank, __LINE__, ##__VA_ARGS__); }

#define CLOG(rank, M, ...) {\
    if(PROC_RANK==rank)fprintf(stdout, "(RANK%d  %d): " M "\n", rank, __LINE__, ##__VA_ARGS__); }


#define printx(val) printf("%08x\n",val)
#define print(val) printf("%d\n",val)

typedef unsigned char  uchar;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long  uint64;

typedef struct package_head{
    uint32 stream;
    uint32 len;
    uint32 fram;
    uchar type;
}package_head;



void print_bytes(void *p, size_t len)
{
    size_t i;
    printf("(");
    for (i = 0; i < len; ++i)
        printf("%02X", ((unsigned char*)p)[i]);
    printf(")");
}

void p(void *p, size_t len)
{
    size_t i;
    printf("(");
    for (i = 0; i < len; ++i)
        printf("%02X", ((unsigned char*)p)[i]);
    printf(")");
}

void ex_endian(void* p,size_t len){
    int i;
    uint32 tmep;
    uchar temp1;
    if(len%2!=0) return;
    for(i=0;i<len/2;i++){
        temp1 = ((uchar*)p)[i];
        ((uchar*)p)[i] = ((uchar*)p)[len-i-1];
        ((uchar*)p)[len-i-1] = temp1;
    }
}

uint32 ibp_type(uchar type){
    return type&~0xfc;
}

uint32 is_fill(uchar type){
    return (type&~0x7f)==128?1:0;
}

uint32 data_len(uint32 len){
    return len;
}

#define size 20

pthread_mutex_t mutex;
pthread_t prod,cons[10],t_test;

sem_t s_full,s_empty;

int put,get;

int count;
int list[size];

FILE* fbin;
FILE* fh264;

typedef struct fram{
    char *data;
    int len;
}fram;

fram frams[size];

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
uint32 list_is_full(){
    pthread_mutex_lock(&m);
    int result = (get == (put + 1)%size);
    pthread_mutex_unlock(&m);
    return result;
}

uint32 list_is_empty(){
    pthread_mutex_lock(&m);
    int result = (get == put);
    pthread_mutex_unlock(&m);
    return result;

}


uint32 fin ;
void *producer(void *para){

    //FILE* fh264 = fopen("./BB.h264","wb");
    fin = 0;
    package_head phead;
    uint32 ihead,readbytes,len;
    int ii=0;
    unsigned short temp1,temp2;
    readbytes = 0;
    uchar *data;
    int temp4;
    //fread(&ihead,sizeof(int),1,f);
    //printx(ihead);
    //ex_endian((void*)&ihead,4);
    //printx(ihead);
    //return 0;
    //short temp1,temp2;
    //fseek(f,0X00342a60,SEEK_CUR);
    while(fread(&temp1,sizeof(short),1,fbin)==1){
        //fseek(fp,SEEK_CUR)
        if(temp1==0x0000){
            if(fread(&temp2,sizeof(short),1,fbin)!=1) return;
            //            print_bytes((void*)&temp2,2);
            //printf("%02X\n",temp2);
            ii++;
            //if(ii==4) return 0; 

            //  return 0; 
            ex_endian((void*)&temp2,2);
            if(temp2==0x00fa){
                fseek(fbin,-4,SEEK_CUR);
                fread(&phead,sizeof(package_head),1,fbin);
                fseek(fbin,-2,SEEK_CUR);
                ex_endian((void*)&(phead.stream),4);
                len = data_len(phead.len);
                data = (uchar*)malloc(sizeof(uchar)*len);
                fread(data,sizeof(uchar)*len,1,fbin);
                while(list_is_full());

                //LOG(888,"\n\nempty: %d\n\n",temp4);
                //pthread_mutex_lock(&m);
                //while(count<20)
                //                frams[count].data = (uchar*)malloc(sizeof(uchar)*len);
                //sem_wait(&s_empty);
                ///pthread_mutex_lock(&m);
                frams[put].len = len;
                frams[put].data = data;

                put = (put + 1) % size;
                count++;
                //sem_post(&s_full);

                //pthread_mutex_unlock(&m);
                //sleep(1);
                //LOG(0,"get: %d  put: %d count: %d\n",get,put,count);
                //pthread_mutex_unlock(&m);
                //sem_getvalue(&s_empty,&temp4);
                //printf("\nproducer=(empty: %d",temp4);
                //sem_getvalue(&s_full,&temp4);
                //printf("full: %d)\n",temp4);
                //sleep(2);

                //put it to queue

                //fwrite((void*)data,len,1,fh264);
                // ex_endian((void*)&(phead.len),4);
                //ex_endian((void*)&(phead.fram),4);
                //fread(&ihead,sizeof(int),1,f);
                //ex_endian((void*)&ihead,4);
                /* printx(phead.stream);
                   print(phead.len);
                   print(phead.fram);
                   printf("%02x\n",phead.type);
                   print_bytes((void*)data,12);
                   printf("\n%d  %d\n\n\n",ibp_type(phead.type),is_fill(phead.type));*/
                //printf("%d\n",sizeof(package_head));
                //                print_bytes((void*)&temp2,2);
                //return 0;
            }
            //return 0;
        }

    }
    //print_bytes((void*)&phead,13);
    //    printf("%x\n",phead.stream);
    //int aa = high_code_stram;
    //printf("%x\n",aa);
    LOG(222,"out there!\n\n");
    fin = 1;
    return ;

}

void *consumer(void *para){
    pthread_t id = pthread_self(); 
    int temp4;
    while(!fin){

        while(list_is_empty());
        // sem_getvalue(&s_full,&temp3);
        //LOG(888,"\n\nfull: %d\n\n",temp3);
        //sem_wait(&s_full);
        //print(987654321);
        //if(count<=0) continue;
        //pthread_mutex_lock(&m);
        //while(get==put);
        //while(empty);
        //print(123456789);
        //print(id);
        LOG(id,"get:%d put:%d count: %d\n",get,put,count);
        get = (get + 1) % size;
        count--;


        fwrite((void*)frams[get].data,frams[get].len,1,fh264);
        //sem_post(&s_empty);

        //sleep(0.005);
       
        //sleep(1);


        //pthread_mutex_unlock(&m);
        //sem_getvalue(&s_empty,&temp4);
        //printf("\nconsumer=(empty: %d",temp4);
        //sem_getvalue(&s_full,&temp4);
        //printf("full: %d)\n",temp4);

    }
    LOG(333,"\n\nconsumer finish!\n\n");

    return ;


}
#define num_threads 1
int main(){
    //sem_init(&s_full,0,0);
   // sem_init(&s_empty,0,20);
    int i,j;
    count = 0;
    get = put = 0;
    fbin = fopen("./BB.bin","rb");
    fh264 = fopen("./BB.h264.mp4","wb");
    pthread_create(&prod,NULL,producer,NULL);
    //sleep(3);

    for(i=0;i<num_threads;i++){
        pthread_create(&cons[i],NULL,consumer,NULL);
    }

    pthread_join(prod,NULL);
    //print(777777777);
    for(j=0;j<num_threads;j++){
        pthread_join(cons[j],NULL);
        //printf("aaa" "bbb");
        LOG(444,"\n\nIN JOIN!\n\n");
    }

    return 0;

}
