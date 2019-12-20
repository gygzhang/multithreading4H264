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


#ifndef __VS_SOCKET_H__
#define __VS_SOCKET_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "vs_def_videoProtocol_3.h"
#include "vs_bufManager.h"



#define   MAXBUF  1024

#define VIDEOPORT                           4600    /* 视频服务器的VideoPort    */
#define AUDIOPORT                           4601    /* 视频服务器的AudioPort    */
#define MSGPORT                             4602    /* 视频服务器的MsgPort      */

#define MAX_SOCKET_BUF_SIZE    512          // 注意一条控制消息的总长度小于该值，并且小于网络MTU阀值


// 接入服务器
#define ACCESS_SERVER       0

// 分发服务器
#define BRODCAST_SERVER     1

// 备份服务器
#define BACKUP_SERVER       2

// 被动连接
#define PASSIVENESS_CONNECT 3

// 最大主动连接数
#define MAX_CONNECT_TO_SERVER   4


int listen_sock(int socket_id, int port);
int vs_socket_vital_error();
int thread_socket_msg_main();
int thread_socket_msg_usr_main();
int thread_recv_from_con(void *param);
int thread_recv_from_con();

int vs_send( int sockid, char *buf, int ilen );
int vs_send_real( int sockid, char *buf, int ilen );

int vs_connect( char *ip, int port, int type );
int package_send( char *buf, int ilen, int type, int send_type, int socketid );
int thread_access_reconnnet_main();


typedef struct tag_thread_socket_maindata
{
	fd_set	read_fds;
    struct  sockaddr_in remoteaddr;
    struct  timeval tv;
    int     listener_msg, listener_video;
    int     newfd;
    int     nbytes;
    int     addrlen;
    int     i;
    int     ret;
    char    *msg_buf;
    
} s_thread_socket_maindata;

typedef struct tag_socket_maindata
{
	fd_set  g_sock_fd;
    int     g_fd_max;
    int     g_quit_sock;    
    char    buffer[BUF_REC_BLK_SIZE];
    char    *buf_msg;
} s_socket_maindata;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __VS_SOCKET_H__ */

