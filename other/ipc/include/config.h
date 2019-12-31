#ifndef __VS_CONFIG_H_
#define __VS_CONFIG_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define _LEN_NORMAL 48
#define _LEN_EXTEND 64
#define _LEN_WEEK 172               // 布防时间专用
#define _LEN_DETECK_AREAS 255       // 移动侦测区域
#define _TICKET_LENGHT 6
#define _MAX_OSD_TIME_LENGTH_ 28
#define DETECT_USLEEP_MS 1000
#define _MAX_LEN_SESSION_ID                     64                  // 网页session_id长度
#define _MAX_LEN_LOGIN_USERNAME                 32                  // 网页登录用户名长度
#define _MAX_LEN_LOGIN_PASSWORD                 32                  // 网页登录密码长度
#define _MAX_NUM_SESSION_ID                 8                       // 最大的session_id组数，即同时允许的最大web访问用户数


// 直连视频连接 SessionID = "DIRECT", 来表明
#define SESSION_ID_DIRECT                 "DIRECT"

// CIF 、QCIF 、2CIF、4CIF/D1。分别用0,1,2,3代替
#define RESOLUTION_D1                3
#define RESOLUTION_HD1               2
#define RESOLUTION_QCIF              1
#define RESOLUTION_CIF               0

// 基本类型
typedef struct tagD_string
{
    char name[_LEN_NORMAL];
    char var[_LEN_NORMAL];                                          // pgh 这个地方如果是移动侦测，需要设置为_LEN_DETECK_AREAS
    char note[_LEN_EXTEND];
}D_string;

typedef struct tagD_string_detect
{
    char name[_LEN_NORMAL];
    char var[_LEN_DETECK_AREAS];                                          // pgh 这个地方如果是移动侦测，需要设置为_LEN_DETECK_AREAS
    char note[_LEN_EXTEND];
}D_string_detect;

typedef struct tagD_week_string
{
    char name[_LEN_NORMAL];
    char var[_LEN_WEEK];        // 专用于布防时间的空间. 每周168个小时
    char note[_LEN_EXTEND];
}D_week_string;

typedef struct tagD_int
{
    char name[_LEN_NORMAL];
    int var;
    char note[_LEN_EXTEND];
}D_int;

typedef struct tagD_long
{
    char name[_LEN_NORMAL];
    long var;
    char note[_LEN_EXTEND];
}D_long;
// .... 其它

// sx added auto
#include "vs_config_auto.h"
// sx added auto end

// 调试用宏定义
//#define DEBUG_SEND_MESSAGE                                    //  tian 2080122 add for debug the reveived and sending message package
//#define debug_printf_buf                                      // buf管理模块打印语句
//#define debug_printf_socket                                   // socket模块打印语句
//#define   debug_cap_print                                     //图片抓拍模块调试打印
//#define   DEBUG_VIDEO_PRINT					 //video print for saving 
//#define  debug_printf_signal                                    // 打印信号时间
//#define debug_printf_msg_rev                                    // 打印接收的消息
//#define debug_printf_mainrountine                               // mainrountine模块打印语句
#define debug_printf_mainrountine_1                             // mainrountine模块打印语句
//#define debug_printf_mainrountine_2                             // mainrountine模块打印语句
//#define debug_printf_mainrountine_RTV                           // mainrountine模块打印语句
//#define debug_printf_timer                                    // timer模块打印语句
//#define debug_printf_timer_queue                              // timer模块打印timer队列
#define debug_printf_parser                                   // parser模块打印语句
//#define debug_printf_detect                                   // detect模块打印语句
//#define debug_printf_main                                     // main模块打印语句
//#define debug_printf_save                                     // save模块打印语句
//#define debug_printf_venc                                     // 专用的视频编码等打印语句
//#define debug_printf_crypt                                    // 加密模块打印语句
//#define debug_printf_crypt_package                            // 加密模块，zdq修改整体信令加密
//#define debug_printf_crypt_MD5                                // MD5模块打印语句
//#define debug_printf_mainrountine_register_res                // 注册反馈打印语句
//#define debug_printf_mainrountine_register_req
//#define debug_printf_mainrountine_store_register_res
//#define debug_printf_mainrountine_keepAlive_req               //　心跳
//#define debug_printf_thread_create                            // 创建线程打印语句
//#define debug_osd_print                                       //osd调试语句
//#define debug_printf_http_context                             // hc->http_context文件内容解析
//#define debug_printf_restart                                  // 直连方式重启
//#define detect_cap
////#define debug_printf_ftp_struct2string                        // ftp打印
//#define debug_photo_searh_print                               //照片浏览调试打印
 // #define debug_sd_state_print                                //SD卡状态信息
 // #define debug_sd_detect_print                               //SD卡状态检测
// #define   debug_ftp_print
//#define debug_handler_cgi_detect_action                      
// 调试用宏定义 end 


#define RTV_AV_FLAG             0                   // 实时音视频标记，0:数据中需要00dc(4字节) + 视频流长度(4字节) + 时间戳(8字节)
                                                    // 1: 数据中不需要00dc(4字节) + 视频流长度(4字节) + 时间戳(8字节)

#define SEND_AV_SLICE           1              // 1:视频流按片发送  0:视频流按帧发送

#define   FAILTIME_DIRECT         10                        // 直连模式实时视频失效秒数
#define   FAILTIME_ACCESS         10                        // 组网模式实时视频失效秒数

int vs_config_put_var(char *name, char *var, char *note);

int vs_config_weekplan2schedule();
int vs_config_schedule2weekplan();

// 读取配置文件
int config_read();

// 重写配置文件
int config_write();

// 根据全局参数，创建一定的定时器任务
int vs_config_init_timer();
AllConfig g_config;         // 全局变量,配置的所有参数

    
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __VS_CONFIG_H_ */





