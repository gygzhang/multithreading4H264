/***********************************************************************************
*              Copyright 2007 - 2010, Megaeyes. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: vs_process_com.c
* Description:  .
*
* History:
* Version   Date              Author        DefectNum    Description

***********************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "vs_process_com.h"
#include "vs_main_rountin.h"
#include "vs_sd.h"
#include "vs_factory_set.h"
#include "vs_photo.h"
//added by ww
#include "vs_video_manage.h"

union semun { 
    int val; 
    struct semid_ds *buf; 
    unsigned short *array; 
} arg; 


s_map_CGI_flag2Handler  map_CGI_flag2Handler[]=
{
		{	ID_CGI_LOGIN_ACTION      , 			handler_cgi_login_action	    },
		{	ID_CGI_ADVC              , 			handler_cgi_advc	            },
		{	ID_CGI_DETECT_ACTION     , 			handler_cgi_detect_action	    },
		{	ID_CGI_ADD_AREA          , 			handler_cgi_add_area	        },

		{	ID_CGI_SD_CARD           , 			handler_cgi_sd_card	            },
		{	ID_CGI_PARA_STORECONF    , 			handler_cgi_para_storeconf	    },
		{	ID_CGI_PARA_SDCRTL       , 			handler_cgi_para_sdcrtl	        },
		{	ID_CGI_PARA_SDCONF       , 			handler_cgi_para_sdconf	        },
 
        {	ID_CGI_MISC             , 			handler_cgi_misc	            },
		{	ID_CGI_PARA_NETWORK     , 			handler_cgi_para_network	    },
		{	ID_CGI_PARA_CONNECT     , 		    handler_cgi_para_connect	    },
		{	ID_CGI_PARA_OSD         , 			handler_cgi_para_osd	        },
		{	ID_CGI_PARA_VENC        , 			handler_cgi_para_venc	        },
		{	ID_CGI_PARA_RESTORE     , 			handler_cgi_para_restore	    },
		{	ID_CGI_PARA_REROOT      , 	        handler_cgi_para_reboot	        },

		{	ID_CGI_PHOTO_SEARCH     , 			handler_cgi_photo_search	    },
		{	ID_CGI_PHOTO_DELETE     , 	        handler_cgi_photo_delete	    },
		{	ID_CGI_PARA_UPDATE      , 	        handler_cgi_para_update	        }
		
};


//生成信号量 
int sem_creat(key_t key) 
{ 
    union semun sem; 
    int semid; 
    sem.val = 0; 
    semid = semget(key,1,IPC_CREAT |0777); 

    if (-1 == semid){ 
        printf("create semaphore error\n"); 
        return -1; 
    } 
    semctl(semid,0,SETVAL,sem); 
    return semid; 
} 

//删除信号量 
void del_sem(int semid) 
{ 
    union semun sem; 
    sem.val = 0; 
    semctl(semid,0,IPC_RMID,sem); 
} 

//p 操作
int p(int semid) 
{ 
    struct sembuf sops={0, -1,0}; 
    return (semop(semid,&sops,1)); 
} 

//v 操作
int v(int semid) 
{ 
    struct sembuf sops={0,1,0}; 
    return (semop(semid,&sops,1)); 
} 


/********************************** 这里处理各个CGI******************************/

/* 直连模式登录:
    1. 检查用户名和密码

    以下两点已经删除
    2. 产生ticket,浏览器返回页面中获得该ticket,并在今后所有的交互(网页浏览和控件视频)中，传递该ticket进行验证，
      ticket的失效时间为1800秒，每次浏览器的交互都重置该失效时间
    3. 返回advc.asp.htm页面
*/
int handler_cgi_login_action(char *p_shm)
{
    int ret;
    char name[_MAX_LEN_LOGIN_USERNAME];
    char pwd[_MAX_LEN_LOGIN_PASSWORD];
    s_cgi_login_action *cgi_login_action;

    // 得到用户名、密码
    cgi_login_action = (s_cgi_login_action *)(p_shm + sizeof(int)*3);
    memcpy(name, cgi_login_action->username, _MAX_LEN_LOGIN_USERNAME);
    memcpy(pwd, cgi_login_action->password, _MAX_LEN_LOGIN_PASSWORD);
    printf("name=%s\n", name);
    printf("password=%s\n", pwd);

    // 检查用户名，密码是否合法,并将结果填写到共享内存相应位置
    ret = login_check(name, pwd);
    *(int *)(p_shm) =  ret;

    // 登录非法，返回错误
    if(0 != ret)
    {
        printf("login failed.\n");
        return -1;
    }
    
    return 0;
    
}  

// 处理CGI的登录成功后advc页面请求
int handler_cgi_advc(char *p_shm)
{
    s_cgi_advc *p_cgi_advc;
    p_cgi_advc = (s_cgi_advc *)(p_shm + sizeof(int)*3);
    
    // pgh 目前对整个共享内存清空，理论上稍微影响速度，可否改进?
    memset(p_shm, 0, SEGSIZE);

    *(int *)p_shm = CGI_REQ_SUC;
    
    p_cgi_advc->num_camera = g_config.camara_id.camara_num.var;
    p_cgi_advc->video_port = g_config.para_TermRegister.VideoPort.var;
    strcpy(p_cgi_advc->device_IP, g_config.para_TermRegister.DeviceIP.var);

    return 0;
}

// 读共享区的数据，专门由handler_cgi_detect_action()调用
// 目前，在网页中，只写了一个摄像头的报警配置，今后应该对每个摄像头写一套报警配置
// 所以，目前系统实现方式是，对一个摄像头的报警配置，实现中对应所有的摄像头，即每个配置相同
int handler_cgi_detect_action_r(s_cgi_detect_action *p_cgi_detect_action)
{
   
    g_config.alarm_task.alarminA.var                        = p_cgi_detect_action->cgi_detect_action_step1.alarminA;         // 设备A:1 启动 0 停止
    g_config.alarm_task.alarminB.var                        = p_cgi_detect_action->cgi_detect_action_step1.alarminB;         // 设备B:1 启动 0 停止  
    g_config.alarm_task.movedetect.var                      = p_cgi_detect_action->cgi_detect_action_step1.movedetect;       // 移动侦测:1 启动 0 停止
    g_config.alarm_task.Capture_enable.var                  = p_cgi_detect_action->Capture_enable;                           // 抓拍：1 启动 0 停止
    g_config.alarm_task.Cap_count.var                       = p_cgi_detect_action->Cap_count;                                // 报警时连续抓拍张数  
    g_config.alarm_task.alarmbell.var                       = p_cgi_detect_action->alarmbell;                                // 响警铃 ：1启动 0 停止 
    g_config.alarm_task.alarmbelltime_a.var                 = p_cgi_detect_action->alarmbelltime_a;                          // 警铃延时时间  
    g_config.alarm_task.recording.var                       = p_cgi_detect_action->recording;                                // 录像 ：1 启动 0 停止
    g_config.alarm_task.recordingtime_r.var                 = p_cgi_detect_action->recordingtime_r;                          // 录像时间
    g_config.alarm_task.Capture_style.var                   = p_cgi_detect_action->Capture_style;                            // 抓拍图片存储地址 1 本地 2ftp服务器                                           
    g_config.alarm_task.Cap_Time.var                        = p_cgi_detect_action->Cap_Time;                                 // 间隔抓拍秒数 
    strcpy(g_config.alarm_task.Detect_Ftp.var               , p_cgi_detect_action->Detect_Ftp);                              // ftp服务器地址
    strcpy(g_config.alarm_task.Detect_Ftp_user.var          , p_cgi_detect_action->Detect_Ftp_user);                         // ftp用户名
    strcpy(g_config.alarm_task.Detect_Ftp_pwd.var           , p_cgi_detect_action->Detect_Ftp_pwd);                          // ftp密码 
    strcpy(g_config.alarm_task.Detect_Ftp_Dir.var           , p_cgi_detect_action->Detect_Ftp_Dir);                          // ftp上图片存储路径
    strcpy(g_config.alarm_task.week_plan.var                , p_cgi_detect_action->week_plan);                               // 报警计划(一位代表一天的一小时) 
    /*  将g_config对应配置文件的week_plan字段数据拷贝到schedule_0 ~ schedule_6中
        148位的week_plan不好写，故在配置文件中采用schedule_0 ~ schedule_6，初始化的时候，会把schedule_0 ~ schedule_6的数据拷贝到week_plan
        而视频参数设置的时候，为了方便，直接对week_plan进行了设置，所以需要将这些数据拷贝到schedule_0 ~ schedule_6  */
    vs_config_weekplan2schedule(0);
    
#if 0
    // 保存联动摄像头id, 目前配置文件只设置4个联动摄像头
    strcpy(g_config.para_alarm_camera_0.camera_id_0.var, alarmconfig1_req.cameraList.camera[0].id);
    strcpy(g_config.para_alarm_camera_0.camera_id_1.var, alarmconfig1_req.cameraList.camera[1].id);
    strcpy(g_config.para_alarm_camera_0.camera_id_2.var, alarmconfig1_req.cameraList.camera[2].id);
    strcpy(g_config.para_alarm_camera_0.camera_id_3.var, alarmconfig1_req.cameraList.camera[3].id);
#endif

    return 0;

}


// 写共享区的数据，专门由handler_cgi_detect_action()调用
int handler_cgi_detect_action_w(s_cgi_detect_action *p_cgi_detect_action)
{
    memset(p_cgi_detect_action, 0, SEGSIZE - sizeof(int)*3);
    
    p_cgi_detect_action->cgi_detect_action_step1.alarminA   = g_config.alarm_task.alarminA.var;          // 设备A:1 启动 0 停止
    p_cgi_detect_action->cgi_detect_action_step1.alarminB   = g_config.alarm_task.alarminB.var;          // 设备B:1 启动 0 停止  
    p_cgi_detect_action->cgi_detect_action_step1.movedetect = g_config.alarm_task.movedetect.var;        // 移动侦测:1 启动 0 停止
    p_cgi_detect_action->Capture_enable                     = g_config.alarm_task.Capture_enable.var;    // 抓拍：1 启动 0 停止
    p_cgi_detect_action->recording                          = g_config.alarm_task.recording.var;         // 录像 ：1 启动 0 停止
    p_cgi_detect_action->alarmbell                          = g_config.alarm_task.alarmbell.var;         // 响警铃 ：1启动 0 停止 
    p_cgi_detect_action->Capture_style                      = g_config.alarm_task.Capture_style.var;     // 抓拍图片存储地址 1 本地 2ftp服务器
    p_cgi_detect_action->recordingtime_r                    = g_config.alarm_task.recordingtime_r.var;   // 录像时间
    p_cgi_detect_action->alarmbelltime_a                    = g_config.alarm_task.alarmbelltime_a.var;   // 警铃延时时间  
    p_cgi_detect_action->Cap_count                          = g_config.alarm_task.Cap_count.var;         // 报警时连续抓拍张数
    p_cgi_detect_action->Cap_Time                           = g_config.alarm_task.Cap_Time.var;          // 间隔抓拍秒数 
    strcpy(p_cgi_detect_action->Detect_Ftp                  , g_config.alarm_task.Detect_Ftp.var);       // ftp服务器地址
    strcpy(p_cgi_detect_action->Detect_Ftp_user             , g_config.alarm_task.Detect_Ftp_user.var);  // ftp用户名
    strcpy(p_cgi_detect_action->Detect_Ftp_pwd              , g_config.alarm_task.Detect_Ftp_pwd.var);   // ftp密码 
    strcpy(p_cgi_detect_action->Detect_Ftp_Dir              , g_config.alarm_task.Detect_Ftp_Dir.var);   // ftp上图片存储路径
    strcpy(p_cgi_detect_action->week_plan                   , g_config.alarm_task.week_plan.var);        // 报警计划(一位代表一天的一小时) 


#ifdef debug_handler_cgi_detect_action
    printf("enter into handler_cgi_detect_action_w()");
    
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->cgi_detect_action_step1.alarminA   );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->cgi_detect_action_step1.alarminB   );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->cgi_detect_action_step1.movedetect );
    printf("p_cgi_detect_action.var=%d\n",          p_cgi_detect_action->Capture_enable                     );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->recording                          );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->alarmbell                          );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->Capture_style                      );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->recordingtime_r                    );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->alarmbelltime_a                    );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->Cap_count                          );                       
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->Detect_Ftp      );
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->Detect_Ftp_user );
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->Detect_Ftp_pwd  );
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->Detect_Ftp_Dir  );
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->week_plan       );
#endif

    return 0;
}

// 处理CGI的alert页面请求
int handler_cgi_detect_action(char *p_shm)
{
    int send_rev;                                               // cgi是发送数据还是请求数据

printf("enter into handler_cgi_detect_action()");
    
    s_cgi_detect_action *p_cgi_detect_action;
    p_cgi_detect_action = (s_cgi_detect_action *)(p_shm + sizeof(int)*3);
   
    //memset(p_cgi_detect_action, 0, SEGSIZE);                                  // pgh 目前对整个清空，理论上稍微影响速度，可否改进?
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记
    send_rev = *(int *)(p_shm + sizeof(int)*2);                 // 得到发送、接收标志

    switch(send_rev)
    {
       case 0:                                                  // CGI发送数据, 此时vs应该读共享区数据
         handler_cgi_detect_action_r(p_cgi_detect_action);
         vs_alarm_para_set(0);                                  // 报警初始化        
         vs_config_write();                                     // 重写配置文件
         break;
       case 1:                                                  // CGI接收数据, 此时vs应该写共享区数据
         handler_cgi_detect_action_w(p_cgi_detect_action);
         break;
       default:
         break;
    }
  
    return 0;
}

// 处理alert_move页面请求--读共享区数据
int handler_cgi_add_area_r(s_cgi_add_area *p_cgi_add_area)
{
    printf("enter into handler_cgi_add_area_r()");
    g_config.para_alarm_camera_0.DetectGrade.var = p_cgi_add_area->difference;
    strcpy(g_config.alarm_task.week_plan.var, p_cgi_add_area->dete_areas);
    vs_config_weekplan2schedule(0);
    
    // 回写到配置文件
    vs_file_opt_conf_write();
    
    return 0;
}

// 处理alert_move页面请求--写共享区数据
int handler_cgi_add_area_w(s_cgi_add_area *p_cgi_add_area)
{
    printf("enter into handler_cgi_add_area_w()");
    memset(p_cgi_add_area, 0, SEGSIZE - sizeof(int)*3);
    strcpy(p_cgi_add_area->dete_areas, g_config.alarm_task.week_plan.var);
    p_cgi_add_area->difference = g_config.para_alarm_camera_0.DetectGrade.var;

    return 0;
}

// 处理alert_move页面请求
int handler_cgi_add_area(char *p_shm)
{
    int send_rev;                                               // cgi是发送数据还是请求数据
    
    s_cgi_add_area *p_cgi_add_area;
    p_cgi_add_area = (s_cgi_add_area *)(p_shm + sizeof(int)*3);
   
    
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记
    send_rev = *(int *)(p_shm + sizeof(int)*2);                 // 得到发送、接收标志

    switch(send_rev)
    {
       case 0:                                                  // CGI发送数据, 此时vs应该读共享区数据
         handler_cgi_add_area_r(p_cgi_add_area);
         break;
       case 1:                                                  // CGI接收数据, 此时vs应该写共享区数据
         handler_cgi_add_area_w(p_cgi_add_area);
         break;
       default:
         break;
    }
  
    return 0;
}



/***********************************  sd卡操作 ***********************************/

// 处理sd_card页面请求--写共享区数据
int handler_cgi_sd_card_w(s_cgi_sd_card *p_cgi_sd_card)
{
    memset(p_cgi_sd_card, 0, SEGSIZE - sizeof(int)*3);
    
    p_cgi_sd_card->cgi_para_storeconf.sdStore_w      = g_config.para_store.SD_store.var;                // 是否SD卡存储  0: 不选择SD卡存储  1: 选择SD卡存储
    p_cgi_sd_card->cgi_para_sdinfo.sd_status         = g_config.para_store.SD_status.var;                // SD卡状态　0: 不存在，1:存在
    p_cgi_sd_card->cgi_para_sdinfo.sd_capb           = g_config.para_store.SD_size_amount.var;           // SD卡总容量　
    p_cgi_sd_card->cgi_para_sdinfo.sd_apb            = g_config.para_store.SD_size_useable.var;          // SD卡剩余容量　
    p_cgi_sd_card->cgi_para_sdinfo.alarm_mes         = g_config.para_store.SD_alarm.var;                 // 报警信息　:空间已满，空间足够
    p_cgi_sd_card->cgi_para_sdconf.sdOverlay_w       = g_config.para_store.SD_overWrite.var;             // SD卡存储照片，满了是否自动覆盖　0:不覆盖　　1:覆盖
    p_cgi_sd_card->cgi_para_sdconf.sdBackup_w        = g_config.para_store.SD_backup.var;                // 备份SD卡中照片：（将SD卡中的照片保存到本地计算机硬盘中） 0 :不备份　1:备份
    p_cgi_sd_card->cgi_para_sdconf.sdBackupclear_w   = g_config.para_store.SD_delete.var;                // 备份后选项　　0: 不删除　1:删除

    return 0;
}

// 处理sd_card页面请求
int handler_cgi_sd_card(char *p_shm)
{
    //int send_rev;                                               // cgi是发送数据还是请求数据
    
    s_cgi_sd_card *p_cgi_sd_card;
    p_cgi_sd_card = (s_cgi_sd_card *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记
    
    handler_cgi_sd_card_w(p_cgi_sd_card);                        // CGI接收数据, 此时vs应该写共享区数据
  
    return 0;
}

// 处理SD存储配置设置--读共享区数据
int handler_cgi_para_storeconf_r(s_cgi_para_storeconf *p_cgi_para_storeconf)
{
 //   p_cgi_para_storeconf->sdStore_w = g_config.para_store.SD_store.var;                // 是否SD卡存储  0: 不选择SD卡存储  1: 选择SD卡存储  
  // ww modified
    g_config.para_store.SD_store.var = p_cgi_para_storeconf->sdStore_w;
    printf("receive sdStore_w = %d\n",p_cgi_para_storeconf->sdStore_w);

    return 0;
}

// 处理SD存储配置设置
int handler_cgi_para_storeconf(char *p_shm)
{
    s_cgi_para_storeconf *p_cgi_para_storeconf;
    p_cgi_para_storeconf = (s_cgi_para_storeconf *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记
    
    handler_cgi_para_storeconf_r(p_cgi_para_storeconf);         // CGI接收数据, 此时vs应该读共享区数据
  
    return 0;
}

int handler_cgi_para_sdcrtl_refresh(s_cgi_para_sdcrtl *p_cgi_para_sdcrtl)
{

    return 0;
}

int handler_cgi_para_sdcrtl_clear(s_cgi_para_sdcrtl *p_cgi_para_sdcrtl)
{

    return 0;
}


int handler_cgi_para_sdcrtl_format()
{
    sd_format();    
    return 0;
}

int handler_cgi_para_sdcrtl_umount()
{
    sd_format();    
    return 0;
}

// 处理SD卡操作: 1: 刷新 2:清除照片 3. SD卡格式化 4.卸载SD卡
int handler_cgi_para_sdcrtl(char *p_shm)
{
    int id_oper;                                              
    
    s_cgi_para_sdcrtl *p_cgi_para_sdcrtl;
    p_cgi_para_sdcrtl = (s_cgi_para_sdcrtl *)(p_shm + sizeof(int)*3);
   
    //memset(p_shm, 0, SEGSIZE);                                  // pgh 目前对整个共享内存清空，理论上稍微影响速度，可否改进?
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    id_oper = p_cgi_para_sdcrtl->sdhandle_w;
    
     switch(id_oper) // 1: 刷新 2:清除照片 3. SD卡格式化 4.卸载SD卡
    {
       case 1:                                                  
         handler_cgi_para_sdcrtl_refresh(p_cgi_para_sdcrtl);
         break;
       case 2:                                                  
         handler_cgi_para_sdcrtl_clear(p_cgi_para_sdcrtl);
         break;
       case 3:                                                  
         handler_cgi_para_sdcrtl_format();
         break;
       case 4:                                                  
         handler_cgi_para_sdcrtl_umount();
         break;
       default:
         break;
    }
  
    return 0;
}

// 处理SD卡配置设置－－ 读
int handler_cgi_para_sdconf_r(s_cgi_para_sdconf *p_cgi_para_sdconf)
{
    g_config.para_store.SD_delete.var       = p_cgi_para_sdconf->sdBackupclear_w;       // 备份后选项　　0: 不删除　1:删除
    g_config.para_store.SD_backup.var       = p_cgi_para_sdconf->sdBackup_w;            // 备份SD卡中照片：（将SD卡中的照片保存到本地计算机硬盘中） 0 :不备份　1:备份
    g_config.para_store.SD_overWrite.var    = p_cgi_para_sdconf->sdOverlay_w;           // SD卡存储照片，满了是否自动覆盖　0:不覆盖　　1:覆盖
    
    return 0;
}

// 处理SD卡配置设置
int handler_cgi_para_sdconf(char *p_shm)
{                                        
    s_cgi_para_sdconf *p_cgi_para_sdconf;
    p_cgi_para_sdconf = (s_cgi_para_sdconf *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    handler_cgi_para_sdconf_r(p_cgi_para_sdconf);           
    return 0;
}



/***********************************  其他设置 ***********************************/

int handler_cgi_misc_w(s_cgi_misc *p_cgi_misc)
{
    memset(p_cgi_misc, 0, SEGSIZE - sizeof(int)*3);

    //连接信息
    p_cgi_misc->cgi_para_connect.dev_max_con                = g_config.para_connect.dev_max_con.var;
    strcpy(p_cgi_misc->cgi_para_connect.dvs_id              , g_config.para_connect.dvs_id.var);
    strcpy(p_cgi_misc->cgi_para_connect.ftp_password        , g_config.para_connect.ftp_password.var);
    strcpy(p_cgi_misc->cgi_para_connect.ftp_username        , g_config.para_connect.ftp_username.var);

    // 网络参数
    strcpy(p_cgi_misc->cgi_para_network.ip_local            , g_config.para_network.ip_local.var);
    strcpy(p_cgi_misc->cgi_para_network.ip_mask             , g_config.para_network.ip_mask.var);
    strcpy(p_cgi_misc->cgi_para_network.ip_gateway          , g_config.para_network.ip_gateway.var);
    strcpy(p_cgi_misc->cgi_para_network.ip_DNS              , g_config.para_network.ip_DNS.var);
    strcpy(p_cgi_misc->cgi_para_network.ip_server_access    , g_config.para_network.ip_server_access_1.var);
    p_cgi_misc->cgi_para_network.port_server_access         = g_config.para_network.port_server_access.var;

    // 显示控制及通道协议 字符叠加 
    p_cgi_misc->cgi_para_osd.osd_enable                     = g_config.para_alarm_camera_0.osd_enable.var;
    p_cgi_misc->cgi_para_osd.tpox                           = g_config.para_alarm_camera_0.tpox.var;
    p_cgi_misc->cgi_para_osd.tpoy                           = g_config.para_alarm_camera_0.tpoy.var;
    p_cgi_misc->cgi_para_osd.osd_time_enable                = g_config.para_alarm_camera_0.osd_enable.var;
    p_cgi_misc->cgi_para_osd.cpox                           = g_config.para_alarm_camera_0.cpox.var;
    p_cgi_misc->cgi_para_osd.cpoy                           = g_config.para_alarm_camera_0.cpoy.var;

    strcpy(p_cgi_misc->cgi_para_osd.text                    , g_config.para_alarm_camera_0.discontent.var);
    strcpy(p_cgi_misc->cgi_para_osd.sTfmt                   , g_config.para_alarm_camera_0.sTfmt.var);

    // 图像品质--编码参数 
    p_cgi_misc->cgi_para_venc.videoPara                     = g_config.para_VENC.radio_rdvfmt.var; 
    p_cgi_misc->cgi_para_venc.birate                        = g_config.para_VENC.rate_bit.var;
    p_cgi_misc->cgi_para_venc.maxquant                      = g_config.para_VENC.badest_quality.var;
    p_cgi_misc->cgi_para_venc.quant1                        = g_config.para_VENC.best_quality.var;
    p_cgi_misc->cgi_para_venc.fps                           = g_config.para_VENC.FrameRate.var;
    p_cgi_misc->cgi_para_venc.bitype                        = g_config.para_VENC.VENC_type.var;
    p_cgi_misc->cgi_para_venc.gjfra                         = g_config.para_VENC.KeyFrameIntervals.var;
    p_cgi_misc->cgi_para_venc.videoformat                   = g_config.para_VENC.resolution.var;
    return 0;
}

// 处理其他设置页面 -- misc
int handler_cgi_misc(char *p_shm)
{
    s_cgi_misc *p_cgi_misc;
    p_cgi_misc = (s_cgi_misc *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    handler_cgi_misc_w(p_cgi_misc);           
    return 0;   
}

int handler_cgi_para_network_r(s_cgi_para_network *p_cgi_para_network)
{
    strcpy(g_config.para_network.ip_local.var               , p_cgi_para_network->ip_local         );
    strcpy(g_config.para_network.ip_mask.var                , p_cgi_para_network->ip_mask          );        
    strcpy(g_config.para_network.ip_gateway.var             , p_cgi_para_network->ip_gateway       );
    strcpy(g_config.para_network.ip_DNS.var                 , p_cgi_para_network->ip_DNS           );
    strcpy(g_config.para_network.ip_server_access_1.var     , p_cgi_para_network->ip_server_access );
    g_config.para_network.port_server_access.var            = p_cgi_para_network->port_server_access ;
    
    return 0;         
}

// 处理其他设置页面 -- misc -- 网络参数
int handler_cgi_para_network(char *p_shm)
{
    s_cgi_para_network *p_cgi_para_network;
    p_cgi_para_network = (s_cgi_para_network *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    handler_cgi_para_network_r(p_cgi_para_network);           
    return 0;   
}

int handler_cgi_para_connect_r(s_cgi_para_connect *p_cgi_para_connect)
{
    g_config.para_connect.dev_max_con.var                   = p_cgi_para_connect->dev_max_con        ;
    strcpy(g_config.para_connect.dvs_id.var                 , p_cgi_para_connect->dvs_id)      ;       
    strcpy(g_config.para_connect.ftp_password.var           , p_cgi_para_connect->ftp_password);
    strcpy(g_config.para_connect.ftp_username.var           , p_cgi_para_connect->ftp_username);
         
    return 0;         
}

// 处理其他设置页面 -- misc -- 连接信息
int handler_cgi_para_connect(char *p_shm)
{
    s_cgi_para_connect *p_cgi_para_connect;
    p_cgi_para_connect = (s_cgi_para_connect *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    handler_cgi_para_connect_r(p_cgi_para_connect);           
    return 0;   
}


int handler_cgi_para_osd_r(s_cgi_para_osd *p_cgi_para_osd)
{
    g_config.para_alarm_camera_0.osd_enable.var             = p_cgi_para_osd->osd_enable       ;  
    g_config.para_alarm_camera_0.tpox.var                   = p_cgi_para_osd->tpox             ;
    g_config.para_alarm_camera_0.tpoy.var                   = p_cgi_para_osd->tpoy             ;
    g_config.para_alarm_camera_0.osd_enable.var             = p_cgi_para_osd->osd_time_enable  ;
    g_config.para_alarm_camera_0.cpox.var                   = p_cgi_para_osd->cpox             ;
    g_config.para_alarm_camera_0.cpoy.var                   = p_cgi_para_osd->cpoy             ;
    strcpy(g_config.para_alarm_camera_0.discontent.var      , p_cgi_para_osd->text)            ;
    strcpy(g_config.para_alarm_camera_0.sTfmt.var           , p_cgi_para_osd->sTfmt);
   
    return 0;         
}

// 处理其他设置页面 -- misc -- 显示控制及通道协议--字符叠加
int handler_cgi_para_osd(char *p_shm)
{
    s_cgi_para_osd *p_cgi_para_osd;
    p_cgi_para_osd = (s_cgi_para_osd *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    handler_cgi_para_osd_r(p_cgi_para_osd);           
    return 0;   
}


int handler_cgi_para_venc_r(s_cgi_para_venc *p_cgi_para_venc)
{
    g_config.para_VENC.radio_rdvfmt.var             = p_cgi_para_venc->videoPara    ; 
    g_config.para_VENC.rate_bit.var                 = p_cgi_para_venc->birate       ;
    g_config.para_VENC.badest_quality.var           = p_cgi_para_venc->maxquant     ;
    g_config.para_VENC.best_quality.var             = p_cgi_para_venc->quant1       ;
    g_config.para_VENC.FrameRate.var                = p_cgi_para_venc->fps          ;
    g_config.para_VENC.VENC_type.var                = p_cgi_para_venc->bitype       ;
    g_config.para_VENC.KeyFrameIntervals.var        = p_cgi_para_venc->gjfra        ;
    g_config.para_VENC.resolution.var               = p_cgi_para_venc->videoformat  ;

    return 0;         
}

// 处理其他设置页面 -- misc -- 图像品质--编码参数 
int handler_cgi_para_venc(char *p_shm)
{
    s_cgi_para_venc *p_cgi_para_venc;
    p_cgi_para_venc = (s_cgi_para_venc *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    handler_cgi_para_venc_r(p_cgi_para_venc);    
    //vs_vm_set_D1_CIF(g_config.para_vedio.SupportFormat.var, 0); 
    vs_vm_set_D1_CIF(g_config.para_VENC.resolution.var, 0); 
    return 0;   
}

// 处理其他设置页面 -- misc -- 恢复出厂设置
int handler_cgi_para_restore(char *p_shm)
{
printf("进行恢复出厂设置\n");
    // 调用函数，进行恢复出厂设置
    vs_restore();  
    *(int *)p_shm = CGI_REQ_SUC;                                // pgh 设置成功标记，这里CGI接到成功标志后，应显示系统将要重启消息
    vs_stop_thread();                                           // 停止所有线程，从而系统重启
              
    return 0;   
}

// 处理其他设置页面 -- misc -- 重新启动
int handler_cgi_para_reboot(char *p_shm)
{
printf("重新启动8888888888888888888888888888\n");
    *(int *)p_shm = CGI_REQ_SUC;                                // pgh 设置成功标记，这里CGI接到成功标志后，应显示系统将要重启消息
    vs_reboot();
         
    return 0;   
}


// 处理其他设置页面 -- misc -- 版本升级
int handler_cgi_para_update(char *p_shm)
{
    *(int *)p_shm = CGI_REQ_SUC;                                // pgh 设置成功标记，这里CGI接到成功标志后，应显示系统将要重启消息
         
    return 0;   
}




/******************************************* 照片浏览&下载 删除 *****************************************************/

int printf_s_cgi_photo_search(s_cgi_photo_search *p_cgi_photo_search)
{
    printf("p_cgi_photo_search->begindate_w=%s\n",p_cgi_photo_search->begindate_w);
    printf("p_cgi_photo_search->beginhour_w=%d\n",p_cgi_photo_search->beginhour_w);
    printf("p_cgi_photo_search->beginminute_w=%d\n",p_cgi_photo_search->beginminute_w);
 
    printf("p_cgi_photo_search->enddate_w=%s\n",p_cgi_photo_search->enddate_w);
    printf("p_cgi_photo_search->endhour_w=%d\n",p_cgi_photo_search->endhour_w);
    printf("p_cgi_photo_search->endminute_w=%d\n",p_cgi_photo_search->endminute_w);
 
    
    printf("p_cgi_photo_search->pageno_w=%d\n",p_cgi_photo_search->pageno_w);
    printf("p_cgi_photo_search->searchtype_w=%d\n",p_cgi_photo_search->searchtype_w);
 
    printf("p_cgi_photo_search->bphoto_search=%d\n",p_cgi_photo_search->bphoto_search);
    printf("p_cgi_photo_search->photoSize_r=%d\n",p_cgi_photo_search->photoSize_r);
    printf("p_cgi_photo_search->pageSize_r=%d\n",p_cgi_photo_search->pageSize_r);
    printf("p_cgi_photo_search->pageNum_r=%d\n",p_cgi_photo_search->pageNum_r);

    printf("p_cgi_photo_search->photoUrl1_r=%s\n",p_cgi_photo_search->photoUrl1_r);
    printf("p_cgi_photo_search->photoUrl2_r=%s\n",p_cgi_photo_search->photoUrl2_r);
    printf("p_cgi_photo_search->photoUrl3_r=%s\n",p_cgi_photo_search->photoUrl3_r);
    printf("p_cgi_photo_search->photoUrl4_r=%s\n",p_cgi_photo_search->photoUrl4_r);
    printf("p_cgi_photo_search->photoUrl5_r=%s\n",p_cgi_photo_search->photoUrl5_r);
    printf("p_cgi_photo_search->photoUrl6_r=%s\n",p_cgi_photo_search->photoUrl6_r);
    printf("p_cgi_photo_search->photoUrl7_r=%s\n",p_cgi_photo_search->photoUrl7_r);
    printf("p_cgi_photo_search->photoUrl8_r=%s\n",p_cgi_photo_search->photoUrl8_r);
    printf("p_cgi_photo_search->photoUrl9_r=%s\n",p_cgi_photo_search->photoUrl9_r);
    
    return 0;
}

int printf_g_cgi_photo_search(s_photo_manage photo_manage)
{
    printf("photo_manage.begindate_w=%s\n", photo_manage.begindate_w.var);
    printf("photo_manage.beginhour_w=%d\n", photo_manage.beginhour_w.var);
    printf("photo_manage.beginminute_w=%d\n", photo_manage.beginminute_w.var);

    printf("photo_manage.enddate_w=%s\n", photo_manage.enddate_w.var);
    printf("photo_manage.endhour_w=%d\n", photo_manage.endhour_w.var);
    printf("photo_manage.endminute_w=%d\n", photo_manage.endminute_w.var);

    printf("photo_manage.pageno_w=%d\n", photo_manage.pageno_w.var);
    printf("photo_manage.searchtype_w=%d\n", photo_manage.searchtype_w.var);

    printf("photo_manage.bphoto_search=%d\n", photo_manage.bphoto_search.var);
    printf("photo_manage.photoSize_r=%d\n", photo_manage.photoSize_r.var);
    printf("photo_manage.pageSize_r=%d\n", photo_manage.pageSize_r.var);
    printf("photo_manage.pageNum_r=%d\n", photo_manage.pageNum_r.var);

    printf("photo_manage.photoUrl1_r=%s\n",photo_manage.photoUrl1_r.var);
    printf("photo_manage.photoUrl2_r=%s\n",photo_manage.photoUrl2_r.var);
    printf("photo_manage.photoUrl3_r=%s\n",photo_manage.photoUrl3_r.var);
    printf("photo_manage.photoUrl4_r=%s\n",photo_manage.photoUrl4_r.var);
    printf("photo_manage.photoUrl5_r=%s\n",photo_manage.photoUrl5_r.var);
    printf("photo_manage.photoUrl6_r=%s\n",photo_manage.photoUrl6_r.var);
    printf("photo_manage.photoUrl7_r=%s\n",photo_manage.photoUrl7_r.var);
    printf("photo_manage.photoUrl8_r=%s\n",photo_manage.photoUrl8_r.var);
    printf("photo_manage.photoUrl9_r=%s\n",photo_manage.photoUrl9_r.var);
    
    
    return 0;
}


int handler_cgi_photo_search_r(s_cgi_photo_search *p_cgi_photo_search)
{ 
   // printf_s_cgi_photo_search(p_cgi_photo_search);
    
    strcpy(g_config.photo_manage.begindate_w.var            ,  p_cgi_photo_search->begindate_w);
    strcpy(g_config.photo_manage.enddate_w.var              ,  p_cgi_photo_search->enddate_w);
    g_config.photo_manage.beginhour_w.var                   =  p_cgi_photo_search->beginhour_w;
    g_config.photo_manage.beginminute_w.var                 =  p_cgi_photo_search->beginminute_w;
    g_config.photo_manage.endhour_w.var                     =  p_cgi_photo_search->endhour_w;
    g_config.photo_manage.endminute_w.var                   =  p_cgi_photo_search->endminute_w;
    g_config.photo_manage.pageno_w.var                      =  p_cgi_photo_search->pageno_w;
    g_config.photo_manage.searchtype_w.var                  =  p_cgi_photo_search->searchtype_w;

    g_config.photo_manage.bphoto_search.var                 =  p_cgi_photo_search->bphoto_search;
  //g_config.photo_manage.pageno_w.var                      =  p_cgi_photo_search->pageNum_r;
  printf("p_cgi_photo_search->pageno_w=%d\n",p_cgi_photo_search->pageno_w);
  printf("p_cgi_photo_search->pageNum_r=%d\n",p_cgi_photo_search->pageNum_r);
    
  //  printf_g_cgi_photo_search(g_config.photo_manage);
    return 0;         
}



int handler_cgi_photo_search_w(s_cgi_photo_search *p_cgi_photo_search)
{

    memset(p_cgi_photo_search, 0, SEGSIZE - sizeof(int)*3);
    
    strcpy(p_cgi_photo_search->begindate_w         ,  g_config.photo_manage.begindate_w.var  );
    strcpy(p_cgi_photo_search->enddate_w           ,  g_config.photo_manage.enddate_w.var    );
    p_cgi_photo_search->beginhour_w         =  g_config.photo_manage.beginhour_w.var    ;
    p_cgi_photo_search->beginminute_w       =  g_config.photo_manage.beginminute_w.var  ;
    p_cgi_photo_search->endhour_w           =  g_config.photo_manage.endhour_w.var      ;
    p_cgi_photo_search->endminute_w         =  g_config.photo_manage.endminute_w.var    ;
    p_cgi_photo_search->pageno_w            =  g_config.photo_manage.pageno_w.var       ;
    p_cgi_photo_search->searchtype_w        =  g_config.photo_manage.searchtype_w.var   ;
  //  p_cgi_photo_search->pageNum_r           =  g_config.photo_manage.pageno_w.var       ;

    p_cgi_photo_search->bphoto_search               =  g_config.photo_manage.bphoto_search.var ;
    p_cgi_photo_search->photoSize_r                 =  g_config.photo_manage.photoSize_r.var   ;
    p_cgi_photo_search->pageSize_r                  =  g_config.photo_manage.pageSize_r.var    ;
    p_cgi_photo_search->pageNum_r                   =  g_config.photo_manage.pageNum_r.var     ;

    strcpy(p_cgi_photo_search->photoUrl1_r         ,  g_config.photo_manage.photoUrl1_r.var  );
    strcpy(p_cgi_photo_search->photoUrl2_r         ,  g_config.photo_manage.photoUrl2_r.var    );
    strcpy(p_cgi_photo_search->photoUrl3_r         ,  g_config.photo_manage.photoUrl3_r.var  );
    strcpy(p_cgi_photo_search->photoUrl4_r         ,  g_config.photo_manage.photoUrl4_r.var    );
    strcpy(p_cgi_photo_search->photoUrl5_r         ,  g_config.photo_manage.photoUrl5_r.var  );
    strcpy(p_cgi_photo_search->photoUrl6_r         ,  g_config.photo_manage.photoUrl6_r.var    );
    strcpy(p_cgi_photo_search->photoUrl7_r         ,  g_config.photo_manage.photoUrl7_r.var  );
    strcpy(p_cgi_photo_search->photoUrl8_r         ,  g_config.photo_manage.photoUrl8_r.var    );
    strcpy(p_cgi_photo_search->photoUrl9_r         ,  g_config.photo_manage.photoUrl9_r.var  );

    return 0;         
}

// 处理照片浏览&下载
int handler_cgi_photo_search(char *p_shm)
{
    int send_rev;
    s_cgi_photo_search *p_cgi_photo_search;
    p_cgi_photo_search = (s_cgi_photo_search *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    send_rev = *(int *)(p_shm + sizeof(int)*2);
    switch(send_rev) 
    {
       case 0:   
printf("处理照片浏览\n");
         handler_cgi_photo_search_r(p_cgi_photo_search);
         vs_photo_browse(0);
         break;
       case 1: 
 printf("处理照片下载\n");
         handler_cgi_photo_search_w(p_cgi_photo_search);      
         break;
       case 3:                                                  
       default:
         break;
    }

    return 0;   
}


int handler_cgi_photo_delete_r(s_cgi_photo_delete *p_cgi_photo_delete)
{
    strcpy(g_config.photo_manage.del_img1_w.var                    ,  p_cgi_photo_delete->del_img1);
    strcpy(g_config.photo_manage.del_img2_w.var                    ,  p_cgi_photo_delete->del_img2);
    strcpy(g_config.photo_manage.del_img3_w.var                    ,  p_cgi_photo_delete->del_img3);
    strcpy(g_config.photo_manage.del_img4_w.var                    ,  p_cgi_photo_delete->del_img4);
    strcpy(g_config.photo_manage.del_img5_w.var                    ,  p_cgi_photo_delete->del_img5);
    strcpy(g_config.photo_manage.del_img6_w.var                    ,  p_cgi_photo_delete->del_img6);
    strcpy(g_config.photo_manage.del_img7_w.var                    ,  p_cgi_photo_delete->del_img7);
    strcpy(g_config.photo_manage.del_img8_w.var                    ,  p_cgi_photo_delete->del_img8);
    strcpy(g_config.photo_manage.del_img9_w.var                    ,  p_cgi_photo_delete->del_img9);

    g_config.photo_manage.del_imgall_w.var = p_cgi_photo_delete->del_img_all;

    return 0;         
}

// 处理照片删除
int handler_cgi_photo_delete(char *p_shm)
{
    s_cgi_photo_delete *p_cgi_photo_delete;
    p_cgi_photo_delete = (s_cgi_photo_delete *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // 设置成功标记

    handler_cgi_photo_delete_r(p_cgi_photo_delete);    
    vs_delete_photo();
    return 0;   
}

// 处理照片显示主页面, 现在这个函数废弃
int handler_cgi_photo_puts(char *p_shm)
{

    
    return 0;   
}



/* 查handler对应表，返回匹配数组单元号  */
int cgi_getHandlerMatch(int id_cgi_handler)
{
	int i;
printf("rrrrrrrrrrrrrrrrrrrrrrrr id_cgi_handler=%d\n", id_cgi_handler);
	for(i=0; map_CGI_flag2Handler[i].cgi_handler!= '\0'; i++)
		if(id_cgi_handler == map_CGI_flag2Handler[i].id_cgi_handler)
		{	
			return i;
		}
	return -1;
}

// 进程间通信消息接收线程，目前采用共享内存方式
int thread_shm_main() 
{ 
    key_t key;                  
    key_t key_cgi;                // 为外部CGI程序对共享内存操作信号量
    int shm_id,sem_id; 
    int sem_id_cgi;                 // 外部CGI程序对共享内存操作信号量
    struct semid_ds buf; 

    int id_cgi_handler; 
    int id_cgi_data_DS;

    char *p_shm;
    // 创建并关联共享内存
    key = ftok("./vs_shm",0); 
    key_cgi = ftok("./vs_shm_cgi",0); 
    //shm_id = shmget(key,SEGSIZE,IPC_CREAT |0604); 
    shm_id = shmget(key,SEGSIZE,IPC_CREAT |0777);
    if (-1 == shm_id){ 
        logger(TLOG_ERROR, "shared memory create error, system will stop");
        thread_stop_all();
        return -1; 
    }  
    p_shm = (char *)shmat(shm_id,0,0);   
    if (-1 == (int)p_shm){ 
        logger(TLOG_ERROR, "shared memory attach error, system will stop");
        thread_stop_all();
        return -1; 
    } 

    // 创建互斥锁
    sem_id = sem_creat(key);                    // vs主程序对共享内存操作信号量
    sem_id_cgi = sem_creat(key_cgi);            // 外部CGI程序对共享内存操作信号量
    
    while( !mapThreadID2Handler[THREAD_SHM].thread_quit_flag )
    {
        p(sem_id);      // vs等待共享内存
        
        id_cgi_data_DS = *(int*)(p_shm + sizeof(int));
        id_cgi_handler = cgi_getHandlerMatch(id_cgi_data_DS);
        if(-1 == id_cgi_handler)
        {             
            logger(TLOG_WARNING, "no cgi handle.");
            continue;
        }
        
        map_CGI_flag2Handler[id_cgi_handler].cgi_handler(p_shm );    

        v(sem_id_cgi);    // 外部程序操作共享内存使能
    }

    shmdt(p_shm); 
    shmctl(shm_id,IPC_RMID,&buf); 
    del_sem(sem_id); 
    del_sem(sem_id_cgi);
	
    return 0; 
//gcc -o shm shm.c -g 
} 



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

