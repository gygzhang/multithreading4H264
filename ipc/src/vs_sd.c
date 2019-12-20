/***********************************************************************************
*              Copyright 2007 - 2010, Megaeyes. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: vs_sd.c
* Description: SD卡的相关操作:获取总容量、剩余容量、SD卡状态，并对全局变量g_config.para_store中的相关项进行赋值
*
* History:
* Version   Date              Author  WXW      DefectNum    Description
* 1.1       2008-01-12   q60002125     NULL            Create this file.
***********************************************************************************/



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdio.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>                  
#include <sys/vfs.h>        /* for statfs() */
#include <sys/stat.h>        /* for stat() */
#include <unistd.h>
#include <fcntl.h>

#include "vs_def_videoProtocol_3.h"
#include "vs_config.h"
#include "vs_sd.h"
#include "vs_save.h"

#include <asm/io.h>
#include <linux/ioctl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>

pthread_mutex_t sd_format_lock;

unsigned int*  gpio_map=0;
unsigned int gpio_mapaddr;
int fd;

//#define GPIO_1_BASE     0x101E5000
//#define GPIO_1_DIR (gpio_mapaddr + 0x400)
//#define GPIO_1_2_DATA (gpio_mapaddr + 0x010) /* GPIO1_2 is sd card insert detect */
//#define GPIO_1_3_DATA (gpio_mapaddr + 0x020) /* GPIO1_3 is reset usb to sd chip 817 */

#define GPIO_3_BASE     0x101E7000
#define GPIO_3_DIR (gpio_mapaddr + 0x400)
#define GPIO_3_3_DATA (gpio_mapaddr + 0x020)

#define MMAP_LENGTH        0x1000

#define HW_REG(reg) *((volatile unsigned int *)(reg))


static int  sd_detect_init(void)
{
    fd=open("/dev/mem",O_RDWR|O_SYNC,00777);
    
    if(fd<0)
    {
        return -1;
    }
    gpio_map = (unsigned int *) mmap( NULL,MMAP_LENGTH,PROT_READ|PROT_WRITE,MAP_SHARED,fd,GPIO_3_BASE);

    if(gpio_map==0)
    {
        return -1;
    }
    gpio_mapaddr = (unsigned int)gpio_map;
    
    return 0;
}


static int sd_detect_exit(void)
{
    printf("sd_detect_exit\n");
    if(-1== munmap( gpio_map, MMAP_LENGTH))
    {
        printf("mumap failed");
        return -1;
    }
    if(-1==close( fd ))
    {
        printf("close the file operator failed");
        return -1;
    }
    return 0;
}

static int sd_detect_operate(void)
{
    unsigned char regvalue;
    
    regvalue = HW_REG(GPIO_3_DIR);
    regvalue &= 0xf7;                        // 配置GPIO3的寄存器GPIO_DIR，表示GPIO3[3]为输入                                    
    HW_REG(GPIO_3_DIR) = regvalue;
    regvalue = HW_REG(GPIO_3_3_DATA);
    if((regvalue &0x08) == 0x08)
        return 1;
    else 
        return 0;   
}

/*函数名:sd_detect_wait
  功能:检测 SD卡的状态、给全局变量赋值，并判断是否有插入或拔出SD卡的动作，如果有就做相应的处理
*/
void sd_detect_wait(void)
{
    unsigned char tempvalue;
    int m;
    m=g_config.para_store.SD_status.var;
    while(1)
    {
        /* when gpio_3_3 is low, sd card is insert. */
        if(sd_detect_operate() == 0)                    //SD卡存在
        {
            usleep(100);
             g_config.para_store.SD_status.var=1;
            if(m!=g_config.para_store.SD_status.var)  //启动过程中有SD卡存在或程序在运行过程中有SD卡插入
            {  
               m=g_config.para_store.SD_status.var;
               system("insmod /dvs/extdrv/sd.ko");
               sleep(3);
               system("mount -t vfat /dev/sd/0/part1 ./SD");
               vs_sd_init();
               vs_set_save_base_folder();
               printf("detect sd card insert!\n");
            }
            
            #ifdef debug_sd_detect_print 
            printf("sd card is power on.\n");
            #endif
        }
        else                                          //SD卡不存在
        {
             g_config.para_store.SD_status.var=0;
            if(m!=g_config.para_store.SD_status.var)  //程序在运行过程中SD卡被拔出
            { 
                m=g_config.para_store.SD_status.var;
                vs_sd_init();
                vs_set_save_base_folder();
                printf("SD卡被拔出\n");
            }
            #ifdef debug_sd_detect_print 
            printf("sd card is power off.\n");
            #endif
        }
        sleep(1);
            
    }    
}


/*函数名:get_sd_capacity
  功能:获取SD卡的容量大小和剩余空间大小，并给全局变量g_config.para_store中的相关项赋值
 */
void get_sd_capacity()
{

    long size ;
    long allsize ;
    int percent;
    
    struct statfs stat;
    statfs("/remote/vs/SD",&stat);
    size = stat.f_bsize * stat.f_bavail;
    allsize = stat.f_bsize * stat.f_blocks;
    if(g_config.para_store.SD_status.var==0)
    { 
      g_config.para_store.SD_size_amount.var = 0;
      g_config.para_store.SD_size_useable.var =0;
      g_config.para_store.SD_alarm.var=0; 
    }
    else
    {
       if ( stat.f_blocks > 0 )
       {
           percent = stat.f_bavail * 100 / stat.f_blocks;
       }
    
       g_config.para_store.SD_size_amount.var = allsize/(1024*1024);  //SD卡总容量
       g_config.para_store.SD_size_useable.var = size/(1024*1024);    //SD卡剩余容量

       if (percent < 10)
       {
           g_config.para_store.SD_alarm.var=0;     //空间不足
       }
       else
       {
          g_config.para_store.SD_alarm.var=1;     //空间够大
       }
    }
    
//#ifdef debug_sd_state_print 
      printf("SD卡总容量 = %d M\n", g_config.para_store.SD_size_amount.var);
      printf("SD卡剩余容量 = %d M\n", g_config.para_store.SD_size_useable.var);
//#endif

    return;
}


int sd_format()
{
    pthread_mutex_lock (&sd_format_lock);
    system("mkfs.vfat /dev/sd/0/part1");
    pthread_mutex_unlock (&sd_format_lock);

    sleep(10);                  // pgh 这里使用sleep可能会有问题，以后改进
    return 0;

}

int sd_umount()
{
    pthread_mutex_lock (&sd_format_lock);
    system("umount /remote/vs/SD");
    sleep(5);
    system("rmmod sd.ko");
    pthread_mutex_unlock (&sd_format_lock);

    sleep(1);                   // pgh 这里使用sleep可能会有问题，以后改进
    return 0;
}




/*  函数名:vs_sd_init
    功能:初始化时调用此函数来对SD卡的相关全局变量(g_config.para_store)赋值
*/
void vs_sd_init()
{
    get_sd_capacity();  

}


/*  此线程是用来检测SD卡的状态*/

void thread_detectsd_main()
{
   if(sd_detect_init() != 0)
        printf("sd card detect init error!\n");
    sd_detect_wait();
    sd_detect_exit();
}


/*函数名:sd_test
  功能:测试SD卡的读写
*/
 
int sd_test()
{
    char buf1[100],buf2[100];
    FILE * fp;
    //int bytesread,byteswrite;
    memset(buf1,'a',sizeof(buf1));
    memset(buf2,'b',sizeof(buf2));
    fp=fopen("/mnt/1.txt","r+");
    fwrite(buf1,sizeof(buf1),1,fp);
    fclose(fp);
return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


