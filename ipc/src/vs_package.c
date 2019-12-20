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

#include <string.h>
#include "vs_def_const.h"
#include "vs_config.h"
#include "vs_package.h"

extern AllConfig g_config;

/* 根据命令头编号，构造请求指令头 */
int32 cmd_package_head_construct(s_cmd_req_head *cmd_req_head, int cmd_req_head_ID, int length_xml)
{
	
	cmd_req_head->version 	= CMD_VERSION_NORMAL;
	cmd_req_head->length 		= length_xml;
	cmd_req_head->ID 			= cmd_req_head_ID;
	cmd_req_head->srandID 	= 0;

	return 0;
}

/* 根据命令头编号，构造请求指令头 */
int32 package_cmd_req_construct(s_cmd_req_head *cmd_req_head, int cmd_req_head_ID, int length_xml)
{
	
	cmd_req_head->version 	= CMD_VERSION_NORMAL;
	cmd_req_head->length 		= length_xml;
	cmd_req_head->ID 			= cmd_req_head_ID;
	cmd_req_head->srandID 	= 0;
	memset(cmd_req_head->srcID ,0 ,LENGTH_BYTE_32);
	memset(cmd_req_head->destID ,0 ,LENGTH_BYTE_32);	   

    // 加密模块，由于哪些信令需要加密还没有确定下来，目前为了测试，暂时只做注册信令
    // 注册信令加密需要修改: 1. 版本号 257->258; 2. srcID 空->ipc设备id 
    if((1 == g_config.para_encrypt.encrypt.var) && (1 == g_config.para_encrypt.encrypt_cmd_register_req.var))
    {
        if(cmd_req_head_ID == CMD_ID_REGISTER_REQ)
        {
            cmd_req_head->version 	= CMD_VERSION_ENCRYPT;
            memcpy(cmd_req_head->srcID, g_config.para_TermRegister.DeviceID.var, strlen(g_config.para_TermRegister.DeviceID.var));
        }
    }
    
    return 0;
	
}

/* 根据命令头编号，构造响应指令头 */
int32 package_cmd_res_construct(s_cmd_res_head *cmd_res_head,int cmd_res_head_ID, int length_xml)
{
	cmd_res_head->version 		= CMD_VERSION_NORMAL;
	cmd_res_head->length 		= length_xml;
	cmd_res_head->ID 			= cmd_res_head_ID;
	cmd_res_head->srandID 		= 0;
	cmd_res_head->flag			= 0;
	
	return 0;
}

/* 根据命令头编号，构造响应指令头  suc_flag: 0 成功, 其它 失败*/
int32 package_cmd_res_construct_1(s_cmd_res_head *cmd_res_head,int cmd_res_head_ID, int length_xml, int suc_flag)
{
	cmd_res_head->version 		= CMD_VERSION_NORMAL;
	cmd_res_head->length 		= length_xml;
	cmd_res_head->ID 			= cmd_res_head_ID;
	cmd_res_head->srandID 		= 0;
	cmd_res_head->flag			= suc_flag;
	
	return 0;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
