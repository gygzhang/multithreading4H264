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

//wxw

#ifdef __cplusplus       
#if __cplusplus
extern "C"{                
#endif
#endif /* __cplusplus */


#include <stdio.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>             // for system()
#include <sys/reboot.h>         // for reboot(RB_AUTOBOOT)
#include <errno.h>
#include <sys/stat.h> 
#include <unistd.h>

#include "vs_def_const.h"
#include "vs_thread.h"
#include "vs_main_rountin.h"
#include "vs_socket.h"
#include "vs_save.h"
#include "vs_video.h"
#include "vs_timer.h"
#include "video_comm.h"
#include "vs_video_manage.h"
#include "vs_history_video.h"
#include "vs_xml_parser_new.h"
#include "vs_config.h"
#include "vs_ftp_put.h"
#include "vs_call_exe.h"
#include "vs_sd.h"
#include "vs_factory_set.h"
#include "vs_audio_decode.h"
//#include "vs_crypt.h"

s_syn_ctrl syn_ctrl;

int thread_start()
{
    int ret;

    ret = vs_thread_create(THREAD_SOCKET);   // 网络通信线程
    if(ret!=0)
        return ret;
    ret = vs_thread_create(THREAD_PARSER);   // 消息解析线程
    if(ret!=0)
        return ret;
    ret = vs_thread_create(THREAD_SHM);      // Web服务线程
    if(ret!=0)
        return ret;
       
    return ret;
}

int ipc_init()
{
    
    config_read("config.txt");  // 读取配置文件
    vs_sd_init();               //SD卡初始化
    thread_syn_init(&syn_ctrl.mutex, &syn_ctrl.cond, &syn_ctrl.flag);
    thread_syn_init(&syn_ctrl_rtv.mutex, &syn_ctrl_rtv.cond, &syn_ctrl_rtv.flag);

    return 0;
}

int main(void)
{
    int ret;

    ret = ipc_init();
    if(ret !=0)
    {
        logger(TLOG_ERROR, "ipc init failed, ipc closed.\n");
        return -1;
    }
    
    ret = thread_start();    
    if(ret!=0)
    {
        logger(TLOG_ERROR, "thread create failed, ipc closed.\n");
        return -1;
    }

    thread_wait();
    logger(TLOG_ERROR, "all thread finished, ipc closed.\n");

    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

