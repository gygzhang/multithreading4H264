/***********************************************************************************
*              Copyright 2007 - 2010, Megaeyes. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_server.c
* Description: main model .
*
* History:
* Version   Date              Author        DefectNum    Description
* 1.1       2007-08-22   q60002125     NULL            Create this file.
***********************************************************************************/




#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdlib.h>      // for exit()
#include <unistd.h>     // for sleep()
#include <stdio.h>
#include <pthread.h>

#include "vs_def_const.h"
#include "vs_def_struct.h"
#include "vs_thread.h"
#include "vs_sender.h"
#include "vs_receiver.h"
#include "vs_parser.h"
#include "vs_video.h"
// #include "../thttpd/thttpd.h"
#include "thttpd.h"
#include "vs_realtimeVideo.h"
#include "vs_socket.h"
#include "vs_timer.h"
#include "thttpd.h"
#include "vs_call_exe.h"
#include "vs_factory_set.h"
#include "vs_sd.h"
#include "vs_led.h"
#include "vs_process_com.h"
#include "vs_audio_decode.h"


/* 线程编号,handler对应表   */
s_mapThreadID2Handler  mapThreadID2Handler[]=
{
		{	THREAD_SOCKET     ,     0,	    thread_socket_msg_main， thread_reset	},
		{	THREAD_PARSER     , 	0,		thread_parser_main	 	},
		{	THREAD_SHM        , 	0,		thread_shm_main 	    },
		{	THREAD_RTV        , 	0,		thread_rtv_main	        },
		{	THREAD_PIC        , 	0,		thread_pic_main 	    }

};

/*   数组threadID[] 中存放所有线程ID */
pthread_t threadID[THREAD_NUM_MAX];
int  num_threadID = 0;


/****************************************************************************************************/

/* 查线程编号，handler对应表，返回匹配数组单元号  */
static int getHandlerMatch(int threadID)
{
	int i;

	for(i=0; mapThreadID2Handler[i].threadHandler!= NULL; i++)
		if(threadID == mapThreadID2Handler[i].threadID)
			return i;
	return -1;

}

 /*等待所有线程结束*/
void thread_wait(void)
{
    if(threadID[0] != 0)
    {
        printf("等待线程%d结束...\n",threadID[0]);
        pthread_join(threadID[0],NULL);
        printf("线程%d已经结束\n",threadID[0]);
    }

    // 解析线程调用buf模块中的函数，其中存在阻塞。目前，为了简化，不等待parser线程结束
    /*
    if(threadID[1] !=0)
    {
        printf("等待线程%d结束...\n",threadID[1]);
        pthread_join(threadID[1],NULL);
        printf("线程%d已经结束\n",threadID[1]);
    }
    */

    if(threadID[2] !=0)
    {
        printf("等待线程%d结束...\n",threadID[2]);
        pthread_join(threadID[2],NULL);
        printf("线程%d已经结束\n",threadID[2]);
    }

    return;
}


/* 创建线程 */
int thread_create(void *handler)
{
	int ret;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
//	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	ret=pthread_create(&threadID[num_threadID],&attr,handler,NULL);
	if(ret!=0){

#ifdef debug_printf_thread_create
		printf ("Create pthread error!\n");
#endif

	    return -1;
	}

#ifdef debug_printf_thread_create
    printf("线程创建成功，线程号=%d\n",threadID[num_threadID]);
#endif

	num_threadID++;
	return 0;

}

/* 根据不同的线程编号，查编号，handler对应表，得到线程入口，创建相应线程
     线程ID号与线程入口函数定义见vs_def.h  */
int vs_thread_create(int threadID)
{
	int *handler;
	int ret;
	int num_mapThreadID2Handler;

	handler = 0;

	num_mapThreadID2Handler = getHandlerMatch(threadID);
	if(num_mapThreadID2Handler < 0)
		return -1;

	handler = mapThreadID2Handler[num_mapThreadID2Handler].threadHandler;

	ret = thread_create(handler);

	return 0;
}

// 根据id号得到线程退出控制变量，并将其置1,使线程主动退出
int vs_thread_stop(int threadID)
{
	int num_mapThreadID2Handler;

	num_mapThreadID2Handler = getHandlerMatch(threadID);
	if(num_mapThreadID2Handler < 0)
		return -1;
    mapThreadID2Handler[num_mapThreadID2Handler].thread_quit_flag = 1;

    return 0;
}

int thread_stop_all()
{
    int i;

    for(i=0; i++; i<4)
    {
        vs_thread_stop(i);
    }

    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
