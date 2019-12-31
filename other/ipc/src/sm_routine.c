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

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "sm_routine.h"
#include "vs_package.h"
#include "vs_config_auto.h"
#include "sm_serial.h"
#include "sm_config.h"
#include "vs_xml.h"
#include "sm_fileOption.h"
#include "logger.h"
#include "utils.h"
#include "netcrosser.h"


// int g_login_suc = 0;            // login成功置1 
// time_t g_login_time;

extern AllConfig g_config;
extern  int log_file_select;      // 0: NETCROSSER_LOGFILE_1文件 1: NETCROSSER_LOGFILE_2文件

/************************************************************************
* 函数名: rountin_common_flag
* 作  者: pgh, sx
* 描  述: 1.上面的rountin_common()函数,是发送一个带有后缀的消息. 如果需要发送一个错误的
*         返回结果,或者无后缀的消息,则不方便.所以这个函数就是不带后缀的消息
*         2.(仅向接入服务发送)
* 输入参数: int cmd_head_ID, 消息指令头编号
*           int flag, 对于res型的消息,可能带有一个flag标记, 0 成功 >0, 错误编号
* 输出参数: 无
* 返回值: 目前==0
************************************************************************/
int rountin_common_flag(int cmd_head_ID, int flag, int srand_id)
{
   //  int i=0;
   // char *p;
    
    int headtype;
    char package[32];
    int len_package;
    s_cmd_req_head cmd_req_head;
    s_cmd_res_head cmd_res_head;

    memset(package, 0, 32);
    len_package = 0;
    
     // 构造起始字节标记
    package[0] = 0x03;
    len_package += 1;

    headtype = vs_get_headtype(cmd_head_ID);
    if ( headtype == 0 )
    {
        logger(TLOG_ERROR, "vs_get_headtype parse cmd_head_ID=%d failed!", cmd_head_ID);
        return -1;
    }
    
    if ( headtype == HEAD_REQ_4 )
    {            
        len_package += sizeof(s_cmd_req_head);
        package_cmd_req_construct(&cmd_req_head, cmd_head_ID, 0, srand_id);
/*        
logger(TLOG_NOTICE, "before convert cmd_res_head---");
p = (char *)(&cmd_req_head);
while(i<16)
{
    logger(TLOG_NOTICE, "%2X", *(p+i));
    i++;
}    
*/
        sm_header_sequence_convert((char *)&cmd_req_head, HEAD_REQ_4);
/*     
logger(TLOG_NOTICE, "after convert cmd_res_head---");
p = (char *)(&cmd_req_head);
i=0;
while(i<20)
{
    logger(TLOG_NOTICE, "%2X", *(p+i));
    i++;
}
*/

        memcpy(package+1, (char *)&cmd_req_head, HEAD_REQ_4 * 4);  
    }
    else
    {  
        len_package += sizeof(s_cmd_res_head);
        package_cmd_res_construct(&cmd_res_head, cmd_head_ID, 0, flag, srand_id);
/*        
logger(TLOG_NOTICE, "before convert cmd_res_head---");
p = (char *)(&cmd_res_head);
while(i<20)
{
    logger(TLOG_NOTICE, "%2X", *(p+i));
    i++;
}
*/
        sm_header_sequence_convert((char *)&cmd_res_head, HEAD_RES_5);  
/*        
logger(TLOG_NOTICE, "after convert cmd_res_head---");
p = (char *)(&cmd_res_head);
i = 0;
while(i<20)
{
    logger(TLOG_NOTICE, "%2X", *(p+i));
    i++;
}
*/
        memcpy(package+1, (char *)&cmd_res_head, HEAD_RES_5 * 4);
    }

    // 构造结束字节标记
    package[len_package] = 0x1a;
    len_package += 1;
    
    sm_serial_send(package, len_package);
    
    return 0;
}

/************************************************************************
* 函数名: rountin_common_res_data
* 作  者: pgh, sx
* 描  述: 1. 这个仅用于回馈，有后缀，后缀可能是自编XML文件，可能是非XML字符串
*         2. (仅向接入服务发送)
* 输入参数: int cmd_head_ID, 消息指令头编号
*           data, 后缀数据
*           data_len, 后缀长度
*           int flag, res的标志位
* 输出参数: 无
* 返回值: 目前==0
************************************************************************/
int rountin_common_res_data(int cmd_head_ID, char *data, int data_len, int flag, int srand_id)
{
    char *package;
    int len_package;

    
    if (data == NULL)
    {
        len_package = sizeof(s_cmd_res_head) + 2;
    }
    else
    {
        len_package = sizeof(s_cmd_res_head) + data_len + 2;
    }

    package = malloc(len_package);
    if (package == NULL)
    {
        logger(TLOG_ERROR, "rountin_common_res_data, malloc package space failed");
        return -1;
    }

    // 构造起始字节标记
    *package = 0x03;
    
    // 构造消息头
    package_cmd_res_construct( (s_cmd_res_head *)(package + 1), cmd_head_ID, data_len, flag, srand_id);
    sm_header_sequence_convert((char *)(package + 1), HEAD_RES_5);
     
    // 构造消息体
    if (data_len > 0 && data != NULL)
    {
        memcpy(package + 1 + sizeof(s_cmd_res_head), data, data_len);
    }

    // 构造结束字节标记
    *(package + len_package - 1) = 0x1a;     
    
    // send  
    sm_serial_send(package, len_package);

    free(package);
    return 0;
}


int xml_get(char *buf, s_xml *xml)
{
    xml->context = buf + sizeof(int) * 4;
    xml->len = *(int *)(buf + sizeof(int));

    return 0;
}

int srand_id_get(char *buf, int *srand_id)
{   
    *srand_id = *(int *)(buf + sizeof(int)*3);
    return 0;
}


// 操作密码验证, 成功返0 
int xml_pw_validate(char *xml, int xml_len)
{
    char password[_LEN_NORMAL];
 
    // 1. 解析xml的Password
    findxml_string(xml, xml_len, "Password", password);

    // 2. 验证
    return memcmp(g_config.para_login.password.var, password, _LEN_NORMAL);
}

// 验证用户名和密码，成功返0, 1, 2,3 ,分别对应4组用户名和密码
int passwd_validate(char *username, char *password)
{
    if((0 == memcmp(g_config.para_login.username.var,  username, _LEN_NORMAL))
       &&(0 == memcmp(g_config.para_login.password.var,  password, _LEN_NORMAL)))
        return 0;
        
    if((0 == memcmp(g_config.para_login_1.username.var,  username, _LEN_NORMAL))
       &&(0 == memcmp(g_config.para_login_1.password.var,  password, _LEN_NORMAL)))
        return 1;

    if((0 == memcmp(g_config.para_login_2.username.var,  username, _LEN_NORMAL))
       &&(0 == memcmp(g_config.para_login_2.password.var,  password, _LEN_NORMAL)))
        return 2;
        
    if((0 == memcmp(g_config.para_login_3.username.var,  username, _LEN_NORMAL))
       &&(0 == memcmp(g_config.para_login_3.password.var,  password, _LEN_NORMAL)))
        return 3;

    return -1;
}

// 仅验证用户名，成功返0, 1, 2,3 ,分别对应4组用户名
int user_match(char *username)
{
    if(0 == memcmp(g_config.para_login.username.var,  username, _LEN_NORMAL))
        return 0;
        
    if(0 == memcmp(g_config.para_login_1.username.var,  username, _LEN_NORMAL))
        return 1;

    if(0 == memcmp(g_config.para_login_2.username.var,  username, _LEN_NORMAL))
        return 2;
        
    if(0 == memcmp(g_config.para_login_3.username.var,  username, _LEN_NORMAL))
        return 3;

    return -1;
}

int user_del(int n)
{
    switch(n)
    {
        case 0:
            memset(g_config.para_login.username.var, 0, _LEN_NORMAL);
            memset(g_config.para_login.password.var, 0, _LEN_NORMAL);
            break;
        case 1:
            memset(g_config.para_login_1.username.var, 0, _LEN_NORMAL);
            memset(g_config.para_login_1.password.var, 0, _LEN_NORMAL);
            break;
        case 2:
            memset(g_config.para_login_2.username.var, 0, _LEN_NORMAL);
            memset(g_config.para_login_2.password.var, 0, _LEN_NORMAL);
            break;
        case 3:
            memset(g_config.para_login_3.username.var, 0, _LEN_NORMAL);
            memset(g_config.para_login_3.password.var, 0, _LEN_NORMAL);
            break;
        default:
            break;
    }

    return 0;
}


// 对第n组进行设置
int passwd_set(int n, char *username, char *password)
{
    switch(n)
    {
        case 0:
            memset(g_config.para_login.username.var, 0, _LEN_NORMAL);
            memset(g_config.para_login.password.var, 0, _LEN_NORMAL);
            memcpy(g_config.para_login.username.var, username, _LEN_NORMAL);
            memcpy(g_config.para_login.password.var, password, _LEN_NORMAL);
            break;
        case 1:
            memset(g_config.para_login_1.username.var, 0, _LEN_NORMAL);
            memset(g_config.para_login_1.password.var, 0, _LEN_NORMAL);
            memcpy(g_config.para_login_1.username.var, username, _LEN_NORMAL);
            memcpy(g_config.para_login_1.password.var, password, _LEN_NORMAL);
            break;
        case 2:
            memset(g_config.para_login_2.username.var, 0, _LEN_NORMAL);
            memset(g_config.para_login_2.password.var, 0, _LEN_NORMAL);
            memcpy(g_config.para_login_2.username.var, username, _LEN_NORMAL);
            memcpy(g_config.para_login_2.password.var, password, _LEN_NORMAL);
            break;
        case 3:
            memset(g_config.para_login_3.username.var, 0, _LEN_NORMAL);
            memset(g_config.para_login_3.password.var, 0, _LEN_NORMAL);
            memcpy(g_config.para_login_3.username.var, username, _LEN_NORMAL);
            memcpy(g_config.para_login_3.password.var, password, _LEN_NORMAL);
            break;
        default:
            break;
    }

    return 0;
}

// 增加新用户
int passwd_add(char *username, char *password)
{
    int ret;
    
    if(NULL == g_config.para_login_1.username.var){
        ret = passwd_set(1, username, password);
        return 0;
    }
    if(NULL == g_config.para_login_2.username.var){
        ret = passwd_set(2, username, password);
        return 0;
    }
    if(NULL == g_config.para_login_3.username.var){
        ret = passwd_set(3, username, password);
        return 0;
    }
    return -1;
}

// 删除用户
int passwd_del(char *username)
{
    int ret;
    
    ret = user_match(username);
    if((0 == ret) ||(-1 == ret))
        return -1;
    
    user_del(ret);
    return -1;
}

/* 密码强度检查
   只能是: A ~ Z a ~ b 0 ~ 9*/
int passwd_check(char *passwd)
{
    char *loc;
    int is_upper = 0, is_lower = 0, is_digital = 0;

    loc = passwd;
    while(0 != *loc)
    {
        if((*loc > 'A')&&(*loc < 'Z')){
            is_upper = 1;
            continue;
        }
        if((*loc > 'a')&&(*loc < 'z')){
            is_lower = 1;
            continue;
        }
        if((*loc > '0')&&(*loc < '9')){
            is_digital = 1;
            continue;
        }
        
        return -1;
    }
    if((1 == is_upper)&&(1 == is_lower)&&(1 == is_digital))
        return 0;   
    
    return -1;
}


// 登录验证
void routine_login_req(char *buf)
{
    int ret;
    int srand_id;                // 指令序号
    s_xml xml;
    s_opera_passwd opera_passwd;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);

    // 2. xml转换成struct
    memset((char *)&opera_passwd, 0, sizeof(s_opera_passwd));
    ret = xml2struct(xml.context, xml.len, (char *)&opera_passwd, s_opera_passwd_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager  --- routine_login_req, xml2struct failed");
        rountin_common_flag(CMD_ID_LOGIN_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
    //ret = memcmp(g_config.para_login.password.var,  opera_passwd.password, _LEN_NORMAL);
    ret =passwd_validate(opera_passwd.username, opera_passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager  --- routine_login_req, password wrong");
        rountin_common_flag(CMD_ID_LOGIN_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }
    
    // 4. 登录成功
//    g_login_suc = 1;
//    LOGIN_TIME_GET();
    logger(TLOG_NOTICE, "serial manager --- login success");
    rountin_common_flag(CMD_ID_LOGIN_RES, MSG_RES_FLAG_SUC, srand_id);
        
    return;      
}

void routine_logout_req(char *buf)
{
    return ;
}

// 用户增加
void routine_pw_add_req(char *buf)
{
    int ret;
    s_xml xml;
    int srand_id;                // 指令序号
    s_passwd_set_req passwd_set_req;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&passwd_set_req, 0, sizeof(s_passwd_set_req));
    ret = xml2struct(xml.context, xml.len, (char *)&passwd_set_req, s_passwd_set_req_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager  --- routine_usr_add_req, xml2struct failed");
        rountin_common_flag(CMD_ID_PW_ADD_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 2.5 新密码强度检查
    ret = passwd_check(passwd_set_req.passwd_new.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_set_req, password frail");
        rountin_common_flag(CMD_ID_PW_ADD_RES, MSG_RES_FLAG_ERR_PASSWD_FRAIL, srand_id);
        return ;
    }

    // 3. 密码验证，非0组用户不能进行修改操作
//    ret = memcmp(g_config.para_login.password.var,  passwd_set_req.password, _LEN_NORMAL);
    ret = passwd_validate(passwd_set_req.passwd.username, passwd_set_req.passwd.password);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_set_req, password wrong");
        rountin_common_flag(CMD_ID_PW_ADD_RES, MSG_RES_FLAG_ERR_PASSWORD, srand_id);
        return ;
    }
    
    // 4. 增加新用户  
    ret = passwd_add(passwd_set_req.passwd_new.username, passwd_set_req.passwd_new.password);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_set_req, password full");
        rountin_common_flag(CMD_ID_PW_ADD_RES, MSG_RES_FLAG_ERR_PASSWD_FULL, srand_id);
        return ;
    }
    
    vs_config_write(); 
    logger(TLOG_NOTICE, "serial manager --- routine_pw_add_req success");
    rountin_common_flag(CMD_ID_PW_ADD_RES, MSG_RES_FLAG_SUC, srand_id); 
    return ;
}

void routine_pw_del_req(char *buf)
{
    int ret;
    s_xml xml;
    int srand_id;                // 指令序号
    s_passwd_del_req passwd_del_req;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&passwd_del_req, 0, sizeof(s_passwd_set_req));
    ret = xml2struct(xml.context, xml.len, (char *)&passwd_del_req, s_passwd_set_req_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_del_req, xml2struct failed");
        rountin_common_flag(CMD_ID_PW_ADD_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证，非0组用户不能进行修改操作
//    ret = memcmp(g_config.para_login.password.var,  passwd_set_req.password, _LEN_NORMAL);
    ret = passwd_validate(passwd_del_req.passwd.username, passwd_del_req.passwd.password);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_del_req, password wrong");
        rountin_common_flag(CMD_ID_PW_DEL_RES, MSG_RES_FLAG_ERR_PASSWORD, srand_id);
        return ;
    }
    
    // 4. 删除用户  
    ret = passwd_del(passwd_del_req.username);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_del_req, password full");
        rountin_common_flag(CMD_ID_PW_DEL_RES, MSG_RES_FLAG_ERR_PASSWD_EMPTY, srand_id);
        return ;
    }
    
    vs_config_write(); 
    logger(TLOG_NOTICE, "serial manager --- routine_pw_add_req success");
    rountin_common_flag(CMD_ID_PW_DEL_RES, MSG_RES_FLAG_SUC, srand_id); 
    return ;
}

// 密码设置
void routine_pw_set_req(char *buf)
{
    int ret;
    s_xml xml;
    int srand_id;                // 指令序号
    s_passwd_set_req passwd_set_req;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&passwd_set_req, 0, sizeof(s_passwd_set_req));
    ret = xml2struct(xml.context, xml.len, (char *)&passwd_set_req, s_passwd_set_req_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_set_req, xml2struct failed");
        rountin_common_flag(CMD_ID_PW_SET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 2.5 新密码强度检查
    ret = passwd_check(passwd_set_req.passwd_new.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_set_req, password frail");
        rountin_common_flag(CMD_ID_PW_SET_RES, MSG_RES_FLAG_ERR_PASSWD_FRAIL, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  passwd_set_req.password, _LEN_NORMAL);
    ret = passwd_validate(passwd_set_req.passwd.username, passwd_set_req.passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_set_req, password wrong");
        rountin_common_flag(CMD_ID_PW_SET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

    // 4. 设置新密码
    passwd_set(ret, passwd_set_req.passwd_new.username, passwd_set_req.passwd_new.password);    
    vs_config_write();
    
    logger(TLOG_NOTICE, "serial manager --- password set success, new password=%s", g_config.para_login.password.var);
    rountin_common_flag(CMD_ID_PW_SET_RES, MSG_RES_FLAG_SUC, srand_id); 
    return ;
}

void routine_rule_get_req(char *buf)
{
    int ret;
    int n_read;                 // 从文件中读出的字符数，一般说来是整个文件的大小
    s_xml xml;
    s_opera_passwd opera_passwd;
    int srand_id;
    int xml_version_info_len;
    char buf_file[MAX_LEN_SERIAL_TRANS];

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&opera_passwd, 0, sizeof(s_opera_passwd));
    ret = xml2struct(xml.context, xml.len, (char *)&opera_passwd, s_opera_passwd_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_pw_get_req, xml2struct failed");
        rountin_common_flag(CMD_ID_RULE_GET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  opera_passwd.password, _LEN_NORMAL);
    ret = passwd_validate(opera_passwd.username, opera_passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager  --- routine_pw_get_req, password wrong");
        rountin_common_flag(CMD_ID_RULE_GET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

    // 读文件到buf
    n_read = file_read(RULE_FILENAME, buf_file, MAX_LEN_SERIAL_TRANS, "rb");
    if(0 > n_read){
        logger(TLOG_ERROR, "serial manager --- routine_rule_get_req, file oper failed");
        rountin_common_flag(CMD_ID_RULE_GET_RES, MSG_RES_FLAG_ERR_FILE, srand_id);
        return ;
    }

//    logger_printf(buf_file, n_read);
    
    // 组包发送, 前面字符串<? xml version = "1.0" ?>  不发送
    xml_version_info_len = sizeof("<? xml version = \"1.0\" ?>");
//    xml_version_info_len += 1;

    rountin_common_res_data(CMD_ID_RULE_GET_RES, buf_file + xml_version_info_len, n_read - xml_version_info_len, MSG_RES_FLAG_SUC, srand_id);
    return ;
}

void routine_rule_set_req(char *buf)
{
    int ret;
    s_xml xml;
    s_opera_passwd opera_passwd;
    int srand_id;
    
    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&opera_passwd, 0, sizeof(s_opera_passwd));
    ret = xml2struct(xml.context, xml.len, (char *)&opera_passwd, s_opera_passwd_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_rule_set_req, xml2struct failed");
        rountin_common_flag(CMD_ID_RULE_SET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
    //ret = memcmp(g_config.para_login.password.var,  opera_passwd.password, _LEN_NORMAL);
    ret= passwd_validate(opera_passwd.username, opera_passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- password wrong");
        rountin_common_flag(CMD_ID_RULE_SET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

    // 4. xml覆盖到两边rules.xml中
    ret = tgap_rewrite_rule_xml(xml.context, xml.len);
    if(0 != ret)
    {
        rountin_common_flag(CMD_ID_RULE_SET_RES, MSG_RES_FLAG_ERR_FILE, srand_id);
        return;
    }    
    ret = tgap_write_right_ruleinfo(xml.context, xml.len);
     if(0 != ret)
    {
        rountin_common_flag(CMD_ID_RULE_SET_RES, MSG_RES_FLAG_ERR_FILE, srand_id);
        return;
    }

    // 5 保存成功，发成功响应消息
    logger(TLOG_NOTICE, "serial manager --- set rule success");
    rountin_common_flag(CMD_ID_RULE_SET_RES, MSG_RES_FLAG_SUC, srand_id);
    
    return ;
}

void routine_log_get_req(char *buf)
{
    int ret;
    s_xml xml;
    char file_context[MAX_LEN_SERIAL_TRANS];
    int n_read = 0;
    int counter = 0;
    s_opera_passwd opera_passwd;
    int srand_id;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&opera_passwd, 0, sizeof(s_opera_passwd));
    ret = xml2struct(xml.context, xml.len, (char *)&opera_passwd, s_opera_passwd_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_log_get_req, xml2struct failed");
        rountin_common_flag(CMD_ID_LOG_GET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  opera_passwd.password, _LEN_NORMAL);
    ret = passwd_validate(opera_passwd.username, opera_passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- routine_log_get_req, password wrong");
        rountin_common_flag(CMD_ID_LOG_GET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

/*    
    // 分配发送消息缓冲，分配失败则发出错响应，并返错
    buf = malloc(MAX_LEN_SERIAL_TRANS);
    if(NULL == buf){
        logger(TLOG_ERROR, "serial manager routine_log_get_req, malloc failed");
        rountin_common_flag(CMD_ID_LOG_GET_RES, MSG_RES_FLAG_ERR_MALLOC, srand_id);
        return ;
    }
*/

    // 填写<Gap>
    memcpy(file_context, "<Gap>", 5);
    counter += 5;
    
    
    if(0 == log_file_select)
    {
        rountin_common_flag(CMD_ID_LOG_GET_RES, MSG_RES_FLAG_ERR_FILE, srand_id);
        return ;
    }

    // 目前日志文件是NETCROSSER_LOGFILE_1，先读NETCROSSER_LOGFILE_2，读不到说明当前没有NETCROSSER_LOGFILE_2；再读1
    if(1 == log_file_select)
    {
        n_read = file_read(NETCROSSER_LOGFILE_2, file_context+counter, MAX_LEN_XML/2, "rb");
        if(-1 == n_read)
        {         
            n_read = 0;
        }
        counter += n_read;
        
        n_read = file_read(NETCROSSER_LOGFILE_1, file_context + counter, MAX_LEN_XML/2, "rb");
        if(0 > n_read)
        {
            logger(TLOG_ERROR, "serial manager --- routine_log_get_req, file oper failed");
            rountin_common_flag(CMD_ID_LOG_GET_RES, MSG_RES_FLAG_ERR_FILE, srand_id);
            return ;
        }
        counter += n_read;

        // 填写</Gap>
        memcpy(file_context+counter, "</Gap>", 6);
        counter += 6;
        // 组包发送
        rountin_common_res_data(CMD_ID_LOG_GET_RES, file_context, counter, MSG_RES_FLAG_SUC, srand_id);
        return ;
    }
    
    // 目前日志文件是NETCROSSER_LOGFILE_2, 应该先读NETCROSSER_LOGFILE_1，再读NETCROSSER_LOGFILE_2，任何一个文件读出错，就全错
    n_read = file_read(NETCROSSER_LOGFILE_1, file_context + counter, MAX_LEN_XML/2, "rb");
    if(-1 == n_read)
    {
        logger(TLOG_ERROR, "serial manager --- routine_log_get_req, file oper failed");
        rountin_common_flag(CMD_ID_LOG_GET_RES, MSG_RES_FLAG_ERR_FILE, srand_id);
        return ;
    }
    counter += n_read;
    
    n_read = file_read(NETCROSSER_LOGFILE_2, file_context + counter, MAX_LEN_XML/2, "rb");
    if(0 > n_read)
    {
        logger(TLOG_ERROR, "serial manager --- routine_log_get_req, file oper failed");
        rountin_common_flag(CMD_ID_LOG_GET_RES, MSG_RES_FLAG_ERR_FILE, srand_id);
        return ;
    }
    counter += n_read;

    // 填写</Gap>
    memcpy(file_context+counter, "</Gap>", 6);
    counter += 6;    
    // 组包发送
    logger(TLOG_NOTICE, "serial manager --- get log success");
    rountin_common_res_data(CMD_ID_LOG_GET_RES, file_context, counter, MSG_RES_FLAG_SUC, srand_id);
    return ;
}

// 时间同步请求
void routine_time_syn_req(char *buf)
{
    int ret;
    s_xml xml;
    s_time_syn_req time_syn_req;
    int srand_id;
    
    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&time_syn_req, 0, sizeof(s_opera_passwd));
    ret = xml2struct(xml.context, xml.len, (char *)&time_syn_req, s_time_syn_req_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_time_syn_req, xml2struct failed");
        rountin_common_flag(CMD_ID_TIME_SYN_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  time_syn_req.password, _LEN_NORMAL);
    ret = passwd_validate(time_syn_req.passwd.username, time_syn_req.passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- routine_time_syn_req, password wrong");
        rountin_common_flag(CMD_ID_TIME_SYN_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

    // pgh 设置系统当前时间
    tgap_rewrite_system_datetime(time_syn_req.time_current);
    tgap_rewrite_right_systemdate(time_syn_req.time_current);

    logger(TLOG_NOTICE, "serial manager --- syn time success");
    rountin_common_flag(CMD_ID_TIME_SYN_RES, MSG_RES_FLAG_SUC, srand_id);
    
    return ;
}


// 目前该函数无操作，直接返成功
void routine_debug_req(char *buf)
{

    return ;
}


// 获取ip请求
void routine_ip_get_req(char *buf)
{
    int ret;
    s_xml xml;
    char outbuf[128];
    s_opera_passwd opera_passwd;
    s_ip_get_res ip_get_res;
    memset(outbuf, 0, 128);
    int srand_id;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&opera_passwd, 0, sizeof(s_opera_passwd));
    ret = xml2struct(xml.context, xml.len, (char *)&opera_passwd, s_opera_passwd_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_log_get_req, xml2struct failed");
        rountin_common_flag(CMD_ID_IP_GET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  opera_passwd.password, _LEN_NORMAL);
    ret = passwd_validate(opera_passwd.username, opera_passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- routine_log_get_req, password wrong");
        rountin_common_flag(CMD_ID_IP_GET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

    // 4. 读取IP地址并发送
    memcpy(ip_get_res.ip_inner, g_config.Network_setting.ip_inner.var, _LEN_IPADDR);
    memcpy(ip_get_res.ip_outer, g_config.Network_setting.ip_outer.var, _LEN_IPADDR);
    struct2xml((char *)(&ip_get_res), outbuf, s_ip_get_res_index, all_xml_find_desc);
    rountin_common_res_data(CMD_ID_IP_GET_RES, outbuf, strlen(outbuf), MSG_RES_FLAG_SUC, srand_id);
    
    return;
}

// 设置ip请求
void routine_ip_set_req(char *buf)
{
    int ret;
    s_xml xml;
    char outbuf[128];
    //s_opera_passwd opera_passwd;
    s_ip_set_req ip_set_req;
    memset(outbuf, 0, 128);
    int srand_id;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&ip_set_req, 0, sizeof(s_ip_set_req));
    ret = xml2struct(xml.context, xml.len, (char *)&ip_set_req, s_ip_set_req_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_ip_set_req, xml2struct failed");
        rountin_common_flag(CMD_ID_IP_SET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  ip_set_req.password, _LEN_NORMAL);
    ret = passwd_validate(ip_set_req.passwd.username, ip_set_req.passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- routine_ip_set_req, password wrong");
        rountin_common_flag(CMD_ID_IP_SET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

    if (( NULL == ip_set_req.ip_inner ) || (NULL == ip_set_req.ip_outer))
    {
        logger(TLOG_ERROR, "serial manager --- routine_ip_set_req, ip addr is NULL, can not to save");
        rountin_common_flag(CMD_ID_IP_SET_RES, MSG_RES_FLAG_ERR_IP, srand_id);
        return;
    }

    // 4. 发送成功响应，并重写两边IP配置文件，重启
    logger(TLOG_NOTICE, "serial manager --- set ip success, system will restart ...");
    rountin_common_flag(CMD_ID_IP_SET_RES, MSG_RES_FLAG_SUC, srand_id);
    tgap_write_right_ipaddress(ip_set_req.ip_outer); 
    tgap_rewritte_ipaddr(ip_set_req.ip_inner, ip_set_req.ip_outer);
  
    return;
}

// 获取HB请求
void routine_hb_get_req(char *buf)
{
    int ret;
    s_xml xml;
    char outbuf[128];
    s_opera_passwd opera_passwd;
//    s_hb_get_res hb_get_res;
    memset(outbuf, 0, 128);
    int srand_id;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
        
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&opera_passwd, 0, sizeof(s_opera_passwd));
    ret = xml2struct(xml.context, xml.len, (char *)&opera_passwd, s_opera_passwd_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_hb_get_req, xml2struct failed");
        rountin_common_flag(CMD_ID_HB_GET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  opera_passwd.password, _LEN_NORMAL);
    ret = passwd_validate(opera_passwd.username, opera_passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- routine_hb_get_req, password wrong %s | %s", 
                           g_config.para_login.password.var, opera_passwd.password );
        rountin_common_flag(CMD_ID_HB_GET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

//    sprintf(hb_get_res.enabled, "%d", g_config.HeartBeat_Setting.hb_enable.var);
//    sprintf(hb_get_res.pri_sec, "%d", g_config.HeartBeat_Setting.hb_primary_secondary.var);
//    hb_get_res.enabled = g_config.HeartBeat_Setting.hb_enable.var;
//    hb_get_res.pri_sec = g_config.HeartBeat_Setting.hb_primary_secondary.var;
//    struct2xml((char *)(&hb_get_res), outbuf, s_hb_get_res_index, all_xml_find_desc);
    sprintf(outbuf, "<GAP><enabled>%d</enabled><pri_sec>%d</pri_sec></GAP>",
            g_config.HeartBeat_Setting.hb_enable.var, g_config.HeartBeat_Setting.hb_primary_secondary.var);

    rountin_common_res_data(CMD_ID_HB_GET_RES, outbuf, strlen(outbuf), MSG_RES_FLAG_SUC, srand_id);
    logger(TLOG_NOTICE, "serial manager -- get heartbeat setting successful!");

    return;
}

// 设置hb请求
void routine_hb_set_req(char *buf)
{
    int ret;
    s_xml xml;
    char outbuf[128];
    s_hb_set_req hb_set_req;
    memset(outbuf, 0, 128);
    int srand_id;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
       
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&hb_set_req, 0, sizeof(s_hb_set_req));
    ret = xml2struct(xml.context, xml.len, (char *)&hb_set_req, s_hb_set_req_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_ip_set_req, xml2struct failed");
        rountin_common_flag(CMD_ID_HB_SET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  hb_set_req.password, _LEN_NORMAL);
    ret = passwd_validate(hb_set_req.passwd.username, hb_set_req.passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- routine_ip_set_req, password wrong");
        rountin_common_flag(CMD_ID_HB_SET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

    // 4. 发送成功响应，并重写单边配置文件，重启
    logger(TLOG_NOTICE, "serial manager -- enabled=%d, pri_sec=%d", hb_set_req.enabled , hb_set_req.pri_sec); 
    ret = tgap_rewritte_hb(hb_set_req.enabled, hb_set_req.pri_sec);
    if(0 != ret)
    {
        logger(TLOG_ERROR, "serial manager -- set heartbeat setting para error!"); 
        rountin_common_flag(CMD_ID_HB_SET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
    }else {
        rountin_common_flag(CMD_ID_HB_SET_RES, MSG_RES_FLAG_SUC, srand_id);
        logger(TLOG_NOTICE, "serial manager --- set hb success, system will restart ...");
        tgap_reboot_right("reboot");
        tgap_system_reset();
    }

    return;
}

// 获取主机上配置的备份机的ip请求
void routine_ip_rt_get_req(char *buf)
{
    int ret;
    s_xml xml;
    char outbuf[128];
    s_opera_passwd opera_passwd;
    s_ip_rt_get_res ip_rt_get_res;
    memset(outbuf, 0, 128);
    int srand_id;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&opera_passwd, 0, sizeof(s_opera_passwd));
    ret = xml2struct(xml.context, xml.len, (char *)&opera_passwd, s_opera_passwd_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_ip_rt_get_req, xml2struct failed");
        rountin_common_flag(CMD_ID_IP_RT_GET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  opera_passwd.password, _LEN_NORMAL);
    ret = passwd_validate(opera_passwd.username, opera_passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- routine_ip_rt_get_req, password wrong");
        rountin_common_flag(CMD_ID_IP_RT_GET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }

    // 4. 读取IP地址并发送
    memcpy(ip_rt_get_res.hb_rt_ip, g_config.HeartBeat_Setting.hb_rt_ip.var, _LEN_IPADDR);
    struct2xml((char *)(&ip_rt_get_res), outbuf, s_ip_rt_get_res_index, all_xml_find_desc);
    rountin_common_res_data(CMD_ID_IP_RT_GET_RES, outbuf, strlen(outbuf), MSG_RES_FLAG_SUC, srand_id);
    
    return;
}

// 设置主机上配置的备份机的ip请求
void routine_ip_rt_set_req(char *buf)
{
    int ret;
    s_xml xml;
    char outbuf[128];
    //s_opera_passwd opera_passwd;
    s_ip_rt_set_req ip_rt_set_req;
    memset(outbuf, 0, 128);
    int srand_id;

    // 0. 获取指令序号
    srand_id_get(buf, &srand_id);
    
    // 1. 获取xml
    xml_get(buf, &xml);
 
    // 2. xml转换成struct
    memset((char *)&ip_rt_set_req, 0, sizeof(s_ip_set_req));
    ret = xml2struct(xml.context, xml.len, (char *)&ip_rt_set_req, s_ip_set_req_index, all_xml_find_desc);
    if(0 != ret){
        logger(TLOG_ERROR, "serial manager --- routine_ip_rt_set_req, xml2struct failed");
        rountin_common_flag(CMD_ID_IP_RT_SET_RES, MSG_RES_FLAG_ERR_XML, srand_id);
        return ;
    }

    // 3. 密码验证
//    ret = memcmp(g_config.para_login.password.var,  ip_rt_set_req.password, _LEN_NORMAL);
    ret = passwd_validate(ip_rt_set_req.passwd.username, ip_rt_set_req.passwd.password);
    if(-1 == ret){
        logger(TLOG_ERROR, "serial manager --- routine_ip_rt_set_req, password wrong");
        rountin_common_flag(CMD_ID_IP_RT_SET_RES, MSG_RES_FLAG_ERR_LOGIN_PWD, srand_id);
        return ;
    }
  
    if ( NULL == ip_rt_set_req.hb_rt_ip ) 
    {
        logger(TLOG_ERROR, "serial manager --- routine_ip_rt_set_req, ip addr is NULL, can not to save");
        rountin_common_flag(CMD_ID_IP_RT_SET_RES, MSG_RES_FLAG_ERR_IP, srand_id);
        return;
    }

    // 4. 发送成功响应，并重写主机配置文件的rt IP(注意备份机不做配置)
    memset(g_config.HeartBeat_Setting.hb_rt_ip.var, 0, _LEN_NORMAL);
    memcpy(g_config.HeartBeat_Setting.hb_rt_ip.var, ip_rt_set_req.hb_rt_ip, _LEN_IPADDR);
    vs_config_write(); 
    rountin_common_flag(CMD_ID_IP_RT_SET_RES, MSG_RES_FLAG_SUC, srand_id);
    logger(TLOG_NOTICE, "serial manager --- set rt ip=%s success", g_config.HeartBeat_Setting.hb_rt_ip.var);
  
    return;
}

/********************************** IPC begin *****************************************************/
int routine_rtv_str_req(char *buf)
{
    int ret;
    s_xml xml;
    char outbuf[128];
    int srand_id;
    s_rtv_str_req rtv_str_req;

    ROUNTINE_COMM_1(rtv_str_req, s_rtv_str_req, s_rtv_str_req_index, CMD_ID_RTV_STR_RES);

    if(g_config.rtv.start == rtv_str_req.flag_start)
    {
        rountin_common_flag(CMD_ID_RTV_STR_RES, MSG_RES_FLAG_SUC, srand_id);
        logger(TLOG_NOTICE, "routine -- %s success", __FUNCTION__);
        return ;
    }

    g_config.rtv.start = rtv_str_req.flag_start;
    vs_config_write();
    if(0 == g_config.rtv.start)
        thread_syn_flag_clr(&syn_ctrl_rtv.mutex, &syn_ctrl_rtv.cond, &syn_ctrl_rtv.flag);
    else
        thread_syn_flag_set(&syn_ctrl_rtv.mutex, &syn_ctrl_rtv.cond, &syn_ctrl_rtv.flag);

    // 发送成功响应
    ROUNTINE_COMM_RES_SUCCESS(CMD_ID_RTV_STR_RES);
  
    return;
}

#ifdef __cplusplus
#if __cplusplus
	
#endif
#endif /* __cplusplus */
