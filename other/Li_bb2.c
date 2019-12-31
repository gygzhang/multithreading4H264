//还不能正确运行的

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define NP 2 //生产者的数目
#define NC 1 //消费者的数目
#define M  20//缓冲数目

typedef unsigned char  uchar;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long  uint64;

typedef struct package_head{
    uint32 len;//4个字节的LEN
    uint32 fram;//4个字节的FRAM
    uchar type;//1个字节的TYPE
}package_head;//数据包结构体

typedef struct fram{
	char *data;
	int  len;
}fram;//帧数据
fram buffer[M];//buffer

int in=0;//生产者放置产品的位置
int out=0;//消费者取产品的位置
sem_t empty;//同步信号量，buffer满阻止生产者放入
sem_t full;//同步信号量，buffer空时阻止消费者消费
pthread_mutex_t mutex;//互斥信号量，一次只能有一个线程访问buffer
int producer_id=0;//生产者id
int comsumer_id=0;//消费者id

FILE *fin;//源文件指针
FILE *fout;//目标文件指针

void *producer(void *arg);//生产者线程函数声明
void *comsumer(void *arg);//消费者线程函数说明
/*主函数*/
int main()
{
	pthread_t id1[NP];
	pthread_t id2[NC];
	int i;
	int ret1[NP];
	int ret2[NC];
	fin=fopen("BB.bin","rb");
	fout=fopen("out","wb");
	 
	//初始化同步信号量
	int ini1 = sem_init(&empty, 0, M);
	int ini2 = sem_init(&full, 0, 0);
	if(ini1 && ini2 != 0)
	{
	   printf("sem init failed \n");
	   exit(1);
	} 
	//初始化互斥信号量 
	int ini3 = pthread_mutex_init(&mutex, NULL);
	if(ini3 != 0)
	{
	   printf("mutex init failed \n");
	   exit(1);
	} 
	//创建NP个生产者线程
	for(i = 0; i < NP; i++)
	{
	   ret1[i] = pthread_create(&id1[i], NULL, producer, NULL);
	   /*
		*第一个参数为指向线程标识符的指针。
		*第二个参数用来设置线程属性。
		*第三个参数是线程运行函数的起始地址。
		*最后一个参数是运行函数的参数。
		*/
	   if(ret1[i] != 0)
	   {
	    printf("producer%d creation failed \n", i);
	    exit(1);
	   }
	}
	//创建NC个消费者线程
	for(i = 0; i < NC; i++)
	{
	   ret2[i] = pthread_create(&id2[i], NULL, comsumer, NULL);
	   if(ret2[i] != 0)
	   {
	    printf("comsumer%d creation failed \n", i);
	    exit(1);
	   }
	}
	//销毁线程
	for(i = 0; i < NP; i++)
	{
	   pthread_join(id1[i],NULL);
	}
	for(i = 0; i < NC; i++)
	{
	   pthread_join(id2[i],NULL);
	}
	
	fclose(fin);
	fclose(fout);
	exit(0); 
}

/*生产者*/
void *producer(void *arg)
{
	uint16 temp1,temp2;//包头:高码流0xfa000000
	package_head phead;
	uint32 len;
	uchar *data;

	while(1)
	{	
		pthread_mutex_lock(&mutex);
	int temp3 = fread(&temp1,sizeof(short),1,fin);//一次读2个字节	

					pthread_mutex_unlock(&mutex);
		while(temp3)//一次读2个字节
		{
			if(temp1==0x0000)
			{

		pthread_mutex_lock(&mutex);
				if(fread(&temp2,sizeof(short),1,fin)!=1) return;
					pthread_mutex_unlock(&mutex);
				if(temp2==0xfa00)
				{
		pthread_mutex_lock(&mutex);
					fread(&phead,sizeof(package_head),1,fin);
					pthread_mutex_unlock(&mutex);
					fseek(fin,-2,SEEK_CUR);
					/*移动文件指针fin从当前位置负向偏移2个字节
					*根据word文档里的说明,package_head总共13个字节，但是编译器编译代码时
					*会4字节对齐，也就是说sizeof(package_head)等于16,所以为了保证读取到
					*完整的数据帧，需要后退，读BB.bin发现RESERVED字段为1个字节，所以后退
					*2个字节
					*/

		sem_wait(&empty);
					len = phead.len;
					data = (uchar*)malloc(sizeof(uchar)*len);
					//data指针指向一片大小为len的内存区域
					fread(data,sizeof(uchar)*len,1,fin);
					
					printf("LEN=%d,FRAM=%d,TYPE=%d\n",len,phead.fram,phead.type);
					//打印出LEN、FRAM、TYPE字段长度信息
					buffer[in].len=len;//将读取到的帧压缩数据的长度放到buffer里
					buffer[in].data=data;//将读取到的帧压缩数据放到buffer里
					in = (in + 1) % M;
					
					sem_post(&full);
				}
			}
			
		}
	}
}
/* 消费者*/
void *comsumer(void *arg)
{
	while(1)
	{
		sem_wait(&full);
		//pthread_mutex_lock(&mutex);	

		fwrite((void*)buffer[out].data,buffer[out].len,1,fout);
		//读取buffer里面的数据输出到文件out.h264里
		out = (out + 1) % M;
		
		//pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	}
}
