#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include"list.h"
#include"common.h"
#include"misc.h"
//#include"stdint.h"
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


#define size 20

pthread_mutex_t mutex;
pthread_t prod,cons[10],t_test;

sem_t s_full,s_empty;

int put,get;

int count;
int list[size];

FILE* fbin;
FILE* fh264;

struct list_head head; //定义全局变量用户链表表头

typedef struct fram{
    int len;
    char *data;
    
}fram;

typedef struct frame_list{
    fram fram;
    struct list_head list;
}frame_list;

fram frams[size];

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

uint32 fin ;
#define num_threads 1

#define LINUX_LIST
//#define CARRAY

void *producer(void *para);

void p(void *p, size_t len);

void ex_endian(void* p,size_t len);



frame_list frames_list;

frame_list *tmp_list;

struct list_head *pos,*n,frame_head;

uint32 ibp_type(uchar type);

uint32 is_fill(uchar type);

uint32 data_len(uint32 len);

uint32 list_is_full();

uint32 list_is_empty();

void *producer(void *para);

void *consumer(void *para);

int aa;

int sock = 0, valread; 
struct sockaddr_in serv_addr; 
#define PORT 2020


char dataSending[1025]; // Actually this is called packet in Network Communication, which contain data and send through.
int clintListn = 0, clintConnt = 0;
struct sockaddr_in ipOfServer;