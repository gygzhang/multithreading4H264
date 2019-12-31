#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include <errno.h>
#include <unistd.h>
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

typedef struct fram{
    char *data;
    int len;
}fram;

fram frams[size];

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

volatile uint32 fin ;
#define num_threads 2

 #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


void p(void *p, size_t len);

void ex_endian(void* p,size_t len);

uint32 ibp_type(uchar type);

uint32 is_fill(uchar type);

uint32 data_len(uint32 len);

uint32 list_is_full();

uint32 list_is_empty();

void *producer(void *para);

void *consumer(void *para);

void display_pthread_attr(pthread_attr_t *attr, char *prefix);

int aa;
int bb;
int cc;