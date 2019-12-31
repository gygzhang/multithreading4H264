/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_thread.h
  Version       : Initial Draft
  Author        : 
  Created       : 
  Last Modified :
  Description   : hi_thread.c header file
  Function List :
  History       :
  1.Date        : 
    Author      : Monster
    Modification: Created file

******************************************************************************/


#ifndef __VS_THREAD_H__
#define __VS_THREAD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*  线程序号定义       */
#define THREAD_SOCKET  	            0
#define THREAD_PARSER    			1
#define THREAD_SHM  	            2
#define THREAD_RTV  	            3
#define THREAD_PIC  	            4


/*  线程序号，handler对应表   */
typedef void  (*s_threadHandler)(); 
typedef struct tagMapThreadID2Handler
{
	int threadID;
	int thread_quit_flag;       // 线程主动退出标志,置1时退出该线程
	void *threadHandler;
} s_mapThreadID2Handler;

void thread_wait(void);
int thread_create(void *handler);
int vs_thread_create(int threadID);
    
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __VS_THREAD_H__ */

