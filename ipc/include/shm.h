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


#ifndef __VS_PROCESS_COM_H__
#define __VS_PROCESS_COM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <sys/sem.h> 
#include <sys/ipc.h> 

#include "vs_config.h"


#define ID_CGI_LOGIN_ACTION                 0
#define ID_CGI_ADVC                         1
#define ID_CGI_DETECT_ACTION  	            2
#define ID_CGI_ADD_AREA    			        3

#define ID_CGI_SD_CARD  	                4
#define ID_CGI_PARA_STORECONF               5
#define ID_CGI_PARA_SDCRTL  	            6
#define ID_CGI_PARA_SDCONF  	            7

#define ID_CGI_MISC  	                    8
#define ID_CGI_PARA_NETWORK  	            9             
#define ID_CGI_PARA_CONNECT                 10  
#define ID_CGI_PARA_OSD                     11                 
#define ID_CGI_PARA_VENC                    12                 
#define ID_CGI_PARA_RESTORE                 13                  
#define ID_CGI_PARA_REROOT                  14

#define ID_CGI_PHOTO_SEARCH                 15                  
#define ID_CGI_PHOTO_DELETE                 16  
#define ID_CGI_PHOTO                        17
#define ID_CGI_PARA_UPDATE                  18

#define SEGSIZE 1024        // 注意，此值要大于所有页面的
#define READTIME 1 

/*******************************各个CGI在这里定义需要从web server获得的数据的数据结构 **********************************/
typedef struct tag_cgi_login_action   // 登录页面数据结构
{
    char username[32];
    char password[32];
} s_cgi_login_action;

typedef struct tag_cgi_advc   // 向vs获得advc页面需要参数: 摄像头个数，IP, port
{
    int num_camera;
    int video_port;
    char device_IP[_LEN_NORMAL];
}s_cgi_advc;

typedef struct tag_cgi_detect_action_step1   // 第一步: 报警启动项配置
{
    int alarminA;                                      // 设备 A
    int alarminB;                                      // 设备 B
    int movedetect;                                    // 移动侦测
} s_cgi_detect_action_step1;


typedef struct tag_cgi_detect_action   // 移动侦测
{
    s_cgi_detect_action_step1 cgi_detect_action_step1;  // 第一步: 报警启动项配置 
    
    int Capture_enable;                                 // 抓拍照片使能
    int recording;                                      // 录像使能
    int alarmbell;                                      // 响警铃使能

    int Capture_style;                                  // 1. 抓拍到本地  2. 抓拍到ftp server
    int recordingtime_r;                                // 录像秒数
    int alarmbelltime_a;                                // 响警铃秒 延时
    int Cap_count;                                      // 报警时连续抓拍张数
    int Cap_Time;                                       // 间隔抓拍秒数

    char Detect_Ftp[_LEN_NORMAL];                                // ftp server IP
    char Detect_Ftp_user[_LEN_NORMAL];                           // ftp username
    char Detect_Ftp_pwd[_LEN_NORMAL];                            // ftp password
    char Detect_Ftp_Dir[_LEN_NORMAL];                            // ftp 存储目录路径

    char week_plan[_LEN_WEEK];                               // 计划任务表
} s_cgi_detect_action;


typedef struct tag_cgi_add_area                         // 移动侦测区域设置
{
    char dete_areas[_LEN_DETECK_AREAS];                 // 移动侦测区域           
    int  difference;                                    // 移动侦测灵敏度
} s_cgi_add_area;


/***********************************  sd卡 ***********************************/
typedef struct tag_cgi_para_storeconf  // SD卡存储配置  
{
    int sdStore_w;                                             // SD卡存储配置 0,不存储 1,存储
} s_cgi_para_storeconf;


typedef struct tag_cgi_para_sdinfo  // SD卡存储信息
{
    int sd_status;                                                  // SD卡状态
    int sd_capb;                                                    // SD卡总容量
    int sd_apb;                                                     // SD卡剩余容量
    int alarm_mes;                                                  // SD报警信息
} s_cgi_para_sdinfo;

typedef struct tag_cgi_para_sdconf  // SD卡配置
{
    int sdOverlay_w;                                                // 是否覆盖 0,不 1. 要
    int sdBackup_w;                                                 // 是否备份 0,不 1. 要;                                               
    int sdBackupclear_w;                                            // 备份后是否清空 0,不 1. 要
} s_cgi_para_sdconf;

typedef struct tag_cgi_para_sdcrtl  // SD卡信息查询、格式化
{  
    int sdhandle_w;                                                 // 1: 刷新 2:清除照片 3. SD卡格式化
} s_cgi_para_sdcrtl;

typedef struct tag_cgi_sd_card      // SD卡页面
{
    s_cgi_para_storeconf    cgi_para_storeconf;                     // SD卡存储配置,是否SD卡存储  0: 不选择SD卡存储  1: 选择SD卡存储
    s_cgi_para_sdinfo       cgi_para_sdinfo;                        // SD卡存储信息查询
    s_cgi_para_sdconf       cgi_para_sdconf;                        // SD卡配置                                          
} s_cgi_sd_card;


/***********************************  其他设置 ***********************************/
typedef struct tag_cgi_para_network   // 网络参数
{
    char ip_local[_LEN_NORMAL];                         // 本机IP地址
    char ip_mask[_LEN_NORMAL];                          // 子网掩码
    char ip_gateway[_LEN_NORMAL];                       // 网关
    char ip_DNS[_LEN_NORMAL];                           // DNS
    char ip_server_access[_LEN_NORMAL];                 // 接入服务器IP地址
    int port_server_access;                             // 接入服务器端口
} s_cgi_para_network;


typedef struct tag_cgi_para_connect   // 连接信息
{
    char ftp_username[_LEN_NORMAL];                     // FTP用户名
    char ftp_password[_LEN_NORMAL];                     // FTP密码
    char dvs_id[_LEN_NORMAL];                           // 设备id,用于解密
    int  dev_max_con;                                   // 设备最大连接数量，即最多允许的同时获取视频的用户数，包括直连模式和组网模式
} s_cgi_para_connect;

typedef struct tag_cgi_para_osd   // 显示控制及通道协议--字符叠加 
{
    int osd_enable;                                     // 填充字幕
    char text[_LEN_NORMAL];                                           // 文字内容
    int tpox;
    int tpoy;
     
    int osd_time_enable;                                // 显示时间
    char sTfmt[_LEN_NORMAL];                                         // 时间格式
    int cpox;                                       
    int cpoy;
} s_cgi_para_osd;


typedef struct tag_cgi_para_venc   // 图像品质--编码参数 
{
    int videoPara;                                      // 定码率
    
    int birate;                                         // 码率
    int maxquant;                                       // 最差质量
    int quant1;                                         // 最好质量
    int fps;                                            // 帧率控制
    int videoformat;                                    // 图像尺寸
    int bitype;                                         // 码流类型, 1:视频流 2:复合流
    int gjfra;                                          // 关键帧间隔
} s_cgi_para_venc;

/****************************************************************************************
对于文件http上传，有如下疑问:
1. 个人觉得，文件通过http上传上来，CGI直接存放到指定的位置就行了，不需要cgi将此数据再发送给vs主程序
2. 如果需要发送给vs主程序，会产生许多问题:
   (1) filesize大小是2M左右，那么共享内存就会浪费很多
   (2) 为了节约共享内存开销，只有对这个文件上传进行单独处理
   
typedef struct tag_cgi_para_update   // 版本升级  pgh 此数据结构成员需要进一步考证
{
    char filename[_LEN_NORMAL];                         
    char filepath[_LEN_NORMAL];
    char fileUID[_LEN_NORMAL];                          // 文件权限
    char file_other_info[_LEN_NORMAL];                  // 文件其他信息
    char file_content[???]
    int filesize;
} s_cgi_para_update;
***************************************************************************************/

typedef struct tag_cgi_para_restore   // 恢复出厂设置，直接操作，无需数据
{  
} s_cgi_para_restore;

typedef struct tag_cgi_para_reboot  // 系统重启，直接操作，无需数据
{  
} s_cgi_para_reboot;


typedef struct tag_cgi_misc      // 其他设置页面
{
    s_cgi_para_network       cgi_para_network;                     // 网络参数
    s_cgi_para_connect       cgi_para_connect;                     // 连接信息
    s_cgi_para_osd           cgi_para_osd;                         // 显示控制及通道协议--字符叠加
    s_cgi_para_venc          cgi_para_venc;                        // 图像品质--编码参数 
} s_cgi_misc;




/************************************* 照片浏览&下载 **************************************************/

typedef struct tag_cgi_photo_search   // 照片浏览&下载
{
    char begindate_w[_LEN_NORMAL];                                       // 开始日期
    int beginhour_w;                                            // 开始小时
    int beginminute_w;                                          // 开始分钟
    
    char enddate_w[_LEN_NORMAL];                                         // 结束日期
    int endhour_w;                                              // 结束小时
    int endminute_w;                                            // 结束分钟
    
    int pageno_w;                                               // 页号
    int searchtype_w;                                           // 1.搜索报警照片 0. 搜索全部照片

    int bphoto_search;                                          //是否点击搜索按钮(用于区分翻页还是搜索)
    int photoSize_r;                                            //照片总数
    int pageSize_r;                                             //总页数
    int pageNum_r;                                              //当前页数
    
    char photoUrl1_r[_LEN_NORMAL];                                    //第1张照片的具体url
    char photoUrl2_r[_LEN_NORMAL];                                     //第2张照片的具体url 
    char photoUrl3_r[_LEN_NORMAL];                                     //第3张照片的具体url
    char photoUrl4_r[_LEN_NORMAL];                                     //第4张照片的具体url
    char photoUrl5_r[_LEN_NORMAL];                                     //第5张照片的具体url
    char photoUrl6_r[_LEN_NORMAL];                                     //第6张照片的具体url
    char photoUrl7_r[_LEN_NORMAL];                                     //第7张照片的具体url
    char photoUrl8_r[_LEN_NORMAL];                                     //第8张照片的具体url
    char photoUrl9_r[_LEN_NORMAL];                                     //第9张照片的具体url
} s_cgi_photo_search;

typedef struct tag_cgi_photo_delete   // 照片删除
{
    char del_img1[_LEN_NORMAL];                                               // 删除照片1
    char del_img2[_LEN_NORMAL];                                               // 删除照片2
    char del_img3[_LEN_NORMAL];                                               // 删除照片3
    char del_img4[_LEN_NORMAL];                                               // 删除照片4
    char del_img5[_LEN_NORMAL];                                               // 删除照片5
    char del_img6[_LEN_NORMAL];                                               // 删除照片6
    char del_img7[_LEN_NORMAL];                                               // 删除照片7
    char del_img8[_LEN_NORMAL];                                               // 删除照片8
    char del_img9[_LEN_NORMAL];                                               // 删除照片9
    int del_img_all;                                                        // 删除所有照片   
} s_cgi_photo_delete;


typedef union tag_cgi_data_DS   
{
    s_cgi_login_action                      cgi_login_action;                // 序号: 0 , 登录页面(username, password)数据结构
    s_cgi_advc                              cgi_advc;                        // 序号: 1 , advc页面参数: 摄像头个数，IP, port
    s_cgi_detect_action                     cgi_detect_action;               // 序号: 2 , alert页面参数
    s_cgi_add_area                          cgi_add_area;                    // 序号: 3 , alert_move页面参数

    s_cgi_sd_card                           cgi_sd_card;                     // 序号: 4 , sd卡页面
    s_cgi_para_storeconf                    cgi_para_storeconf;              // 序号: 5 , alert_move页面参数
    s_cgi_para_sdcrtl                       cgi_para_sdcrtl;                 // 序号: 6 , alert_move页面参数
    s_cgi_para_sdconf                       cgi_para_sdconf;                 // 序号: 7 , alert_move页面参数

    s_cgi_para_network                      cgi_para_network;                // 序号: 8  , 网络参数
    s_cgi_para_connect                      cgi_para_connect;                // 序号: 9  , 连接信息
    s_cgi_para_osd                          cgi_para_osd;                    // 序号: 10 , 显示控制及通道协议--字符叠加
    s_cgi_para_venc                         cgi_para_venc;                   // 序号: 11 , 图像品质--编码参数 
    s_cgi_para_restore                      cgi_para_restore;                // 序号: 12 , 恢复出厂设置
    s_cgi_para_reboot                       cgi_para_reboot;                 // 序号: 13 , 系统重启

    s_cgi_photo_search                      cgi_photo_search;                // 序号: 14 , 照片浏览&下载
    s_cgi_photo_delete                      cgi_photo_delete;                // 序号: 15 , 照片删除                   
} s_cgi_data_DS;

typedef struct tag_cgi_data
{
    int result;					                // 成功标志0, 成功 >0 失败。共享内存的对方处理完毕后，填写此值
    int id_cgi_data_DS;                         // 用来对应后面的数据结构
    int send_rev;					            // 表示向共享内存的对方 发送还是接受数据0.发送 1. 接收  2.发送&接收
    s_cgi_data_DS cgi_data_DS;                  // 页面数据结构    
} s_cgi_data;

// 网页的每一个提交，对应一个CGI,对应一个vs的CGI处理程序，他们的连接关系:
// cgi_data_DS -- id_cgi_data_DS    id_cgi_handler -- cgi_handler
// 只要共享内存的双方的id_cgi_data_DS/id_cgi_handler使用相同的编号，就能实现提交和vs处理程序的对应
typedef int(*cgi_CallBack_operation)(char *p_shm);
typedef struct tag_map_CGI_flag2Handler
{

	int id_cgi_handler;
	cgi_CallBack_operation cgi_handler;               // 处理对应CGI的handler
} s_map_CGI_flag2Handler;

#define ID_CGI_LOGIN_ACTION                 0
#define ID_CGI_ADVC                         1
#define ID_CGI_DETECT_ACTION  	            2
#define ID_CGI_ADD_AREA    			        3

#define ID_CGI_SD_CARD  	                4
#define ID_CGI_PARA_STORECONF               5
#define ID_CGI_PARA_SDCRTL  	            6
#define ID_CGI_PARA_SDCONF  	            7

#define ID_CGI_MISC  	                    8
#define ID_CGI_PARA_NETWORK  	            9             
#define ID_CGI_PARA_CONNECT                 10  
#define ID_CGI_PARA_OSD                     11                 
#define ID_CGI_PARA_VENC                    12                 
#define ID_CGI_PARA_RESTORE                 13                  
#define ID_CGI_PARA_REROOT                  14

#define ID_CGI_PHOTO_SEARCH                 15                  
#define ID_CGI_PHOTO_DELETE                 16  
#define ID_CGI_PHOTO                        17
#define ID_CGI_PARA_UPDATE                  18



#define CGI_REQ_SUC                     0               // cgi对vs的请求成功
#define CGI_REQ_ERR                     1               // cgi对vs的请求失败



int sem_creat(key_t key) ;//生成信号量 
void del_sem(int semid) ;//删除信号量 
int p(int semid) ;//p 操作
int v(int semid) ;//v 操作
int handler_cgi_login_action(char *p_shm);
int handler_cgi_advc(char *p_shm);// 处理CGI的登录成功后advc页面请求
int handler_cgi_detect_action_r(s_cgi_detect_action *p_cgi_detect_action);// 读共享区的数据，专门由handler_cgi_detect_action()调用
int handler_cgi_detect_action_w(s_cgi_detect_action *p_cgi_detect_action);// 写共享区的数据，专门由handler_cgi_detect_action()调用
int handler_cgi_detect_action(char *p_shm);// 处理CGI的alert页面请求
int handler_cgi_add_area_r(s_cgi_add_area *p_cgi_add_area);// 处理alert_move页面请求--读共享区数据
int handler_cgi_add_area_w(s_cgi_add_area *p_cgi_add_area);// 处理alert_move页面请求--写共享区数据
int handler_cgi_add_area(char *p_shm);// 处理alert_move页面请求
int handler_cgi_sd_card_w(s_cgi_sd_card *p_cgi_sd_card);// 处理sd_card页面请求--写共享区数据
int handler_cgi_sd_card(char *p_shm);// 处理sd_card页面请求
int handler_cgi_para_storeconf_r(s_cgi_para_storeconf *p_cgi_para_storeconf);// 处理SD存储配置设置--读共享区数据
int handler_cgi_para_storeconf(char *p_shm);// 处理SD存储配置设置
int handler_cgi_para_sdcrtl_refresh(s_cgi_para_sdcrtl *p_cgi_para_sdcrtl);
int handler_cgi_para_sdcrtl_clear(s_cgi_para_sdcrtl *p_cgi_para_sdcrtl);
int handler_cgi_para_sdcrtl_format();
int handler_cgi_para_sdcrtl_umount();
int handler_cgi_para_sdcrtl(char *p_shm);// 处理SD卡操作: 1: 刷新 2:清除照片 3. SD卡格式化 4.卸载SD卡
int handler_cgi_para_sdconf_r(s_cgi_para_sdconf *p_cgi_para_sdconf);// 处理SD卡配置设置－－ 读
int handler_cgi_para_sdconf(char *p_shm);// 处理SD卡配置设置
int handler_cgi_misc_w(s_cgi_misc *p_cgi_misc);
int handler_cgi_para_update(char *p_shm);               // 处理其他设置页面 -- misc -- 版本升级
int handler_cgi_misc(char *p_shm);// 处理其他设置页面 -- misc
int handler_cgi_para_network_w(s_cgi_para_network *p_cgi_para_network);
int handler_cgi_para_network(char *p_shm);// 处理其他设置页面 -- misc -- 网络参数
int handler_cgi_para_connect_w(s_cgi_para_connect *p_cgi_para_connect);
int handler_cgi_para_connect(char *p_shm);// 处理其他设置页面 -- misc -- 连接信息
int handler_cgi_para_osd_w(s_cgi_para_osd *p_cgi_para_osd);
int handler_cgi_para_osd(char *p_shm);// 处理其他设置页面 -- misc -- 显示控制及通道协议--字符叠加
int handler_cgi_para_venc_w(s_cgi_para_venc *p_cgi_para_venc);
int handler_cgi_para_venc(char *p_shm);// 处理其他设置页面 -- misc -- 图像品质--编码参数 
int handler_cgi_para_restore(char *p_shm);// 处理其他设置页面 -- misc -- 恢复出厂设置
int handler_cgi_para_reboot(char *p_shm);// 处理其他设置页面 -- misc -- 重新启动
int handler_cgi_photo_search_w(s_cgi_photo_search *p_cgi_photo_search);
int handler_cgi_photo_search(char *p_shm);// 处理照片浏览&下载
int handler_cgi_photo_delete_w(s_cgi_photo_delete *p_cgi_photo_delete);
int handler_cgi_photo_delete(char *p_shm);// 处理照片删除
int cgi_getHandlerMatch(int id_cgi_handler); /* 查handler对应表，返回匹配数组单元号  */
int thread_process_com_main() ;                 // 进程间通信消息接收线程，目前采用共享内存方式



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __VS_SOCKET_H__ */
