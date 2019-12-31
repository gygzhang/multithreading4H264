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


#include "stdlib.h"
#include "vs_xml.h"
#include "sm_parser.h"



/* 根据命令头编号，构造相应xml  格式字符串 */
/************************************************************************
 * 函数名: buf_construct_xml
 * 作  者: pgh, sx
 * 描  述: 根据命令头编号, 根据命令头编号，构造相应xml  格式字符串
 * 输入参数: int cmd_head_ID, 命令头编号,比如 注册 TermRegister_Req的编号 2002, 回馈2003等 
 *           char *p_struct_data, 是本命令头对应的struct的数据, 根据里面的数据才能正确生成xml
 * 输出参数: int *length_xml, 返回的xml的长度.
 *           char *out_xml_buf, 返回的xml字符串, 如果==NULL,表示仅想得知xml的长度以便分配内存
 * 返回值: 0: 找到并返回, -1: 没有找到这个结构
************************************************************************/
int buf_construct_xml(int cmd_head_ID, char *p_struct_data, int *length_xml, char *out_xml_buf)
{
    int struct_index;

    if ( length_xml == NULL )
        return -1;

    *length_xml = 0;
    struct_index = -1;              // 命令头编号与struct的对应关系
    
    switch (cmd_head_ID) 
    {
        case XML_TYPE_ID_0:
            struct_index = s_member_node_index;
            break;
        default:
            break;
    }

    if (struct_index >= 0)
    {
        *length_xml = struct2xml(p_struct_data, out_xml_buf, struct_index, all_xml_find_desc );
        return 0;
    }
    return -1;
}

/************************************************************************
 * 函数名: all_xml_find_desc
 * 作  者: sx
 * 描  述: 根据xml对应的struct索引, 返回struct描述信息. 这些信息使用工具生成+人工调整.
 *         这里只需要这个函数的名字 all_xml_find_desc, 其参数根本不需要使用者调用
 * 输入参数: int index, 在使用方法上,通常为 该struct的name + "_index", 这是放在enum里枚举过了变量
 * 输出参数: INPUT_MESSAGE *out_inputmsg, 返回的描述信息(总信息)
 * 返回值: 返回的描述信息(分层信息)
************************************************************************/
char * all_xml_find_desc(int index, INPUT_MESSAGE *out_inputmsg)
{
    if (index == s_member_node_index)
    {
        INPUT_MESSAGE msg = { "s_member_node", 9, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"ID",              sizeof(int),             TYPE_INT,     0,            0 },
            {"name",            XML_MAXLEN_NAME,         TYPE_STRING,  0,            0 },
            {"srcaddr",         XML_MAXLEN_ADDR,         TYPE_STRING,  0,            0 },
            {"srcmac",          XML_MAXLEN_MAC,          TYPE_STRING,  0,            0 },
            {"srcport",         sizeof(int),             TYPE_INT,     0,            0 },
            {"proto",           XML_MAXLEN_PROTO,        TYPE_STRING,  0,            0 },
            {"destaddr",        XML_MAXLEN_ADDR,         TYPE_STRING,  0,            0 },
            {"destmac",         XML_MAXLEN_MAC,          TYPE_STRING,  0,            0 },
            {"destport",        sizeof(int),             TYPE_INT,     0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }
  
    if (index == s_xml_test_index)
    {
        INPUT_MESSAGE msg = { "s_xml_test", 3, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"DestAddr",        XML_MAXLEN_ADDR,         TYPE_STRING,  0,            0 },
            {"DestMac",         XML_MAXLEN_MAC,          TYPE_STRING,  0,            0 },
            {"DestPort",        sizeof(int),             TYPE_INT,     0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_opera_passwd_index)
    {
        INPUT_MESSAGE msg = { "s_opera_passwd", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"username",        _LEN_NORMAL,             TYPE_STRING,  0,            0 },
            {"password",        _LEN_NORMAL,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_passwd_set_req_index)
    {
        INPUT_MESSAGE msg = { "s_passwd_set_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"passwd_new",      sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_passwd_del_req_index)
    {
        INPUT_MESSAGE msg = { "s_passwd_del_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"username",        _LEN_NORMAL,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_time_syn_req_index)
    {
        INPUT_MESSAGE msg = { "s_time_syn_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"time_current",    _LEN_NORMAL,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_ip_get_res_index)
    {
        INPUT_MESSAGE msg = { "s_ip_get_res", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"ip_inner",        _LEN_IPADDR,             TYPE_STRING,  0,            0 },
            {"ip_outer",        _LEN_IPADDR,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_ip_set_req_index)
    {
        INPUT_MESSAGE msg = { "s_ip_set_req", 3, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"ip_inner",        _LEN_IPADDR,             TYPE_STRING,  0,            0 },
            {"ip_outer",        _LEN_IPADDR,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_hb_get_res_index)
    {
        INPUT_MESSAGE msg = { "s_hb_get_res", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"enabled",         _LEN_NORMAL,             TYPE_STRING,  0,            0 },
            {"pri_sec",         _LEN_NORMAL,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }
    
    if (index == s_hb_set_req_index)
    {
        INPUT_MESSAGE msg = { "s_hb_set_req", 3, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"enabled",         sizeof(int),             TYPE_INT,     0,            0 },
            {"pri_sec",         sizeof(int),             TYPE_INT,     0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    

    if (index == s_ip_rt_get_res_index)
    {
        INPUT_MESSAGE msg = { "s_ip_rt_get_res", 1, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"hb_rt_ip",        _LEN_IPADDR,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }
    
    if (index == s_ip_rt_set_req_index)
    {
        INPUT_MESSAGE msg = { "s_ip_rt_set_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"hb_rt_ip",        _LEN_IPADDR,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

/********************************************** IPC begin ***********************************************/
    if (index == s_rtv_str_req_index)
    {
        INPUT_MESSAGE msg = { "s_rtv_str_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"flag_start",      sizeof(int),             TYPE_INT,     0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    return NULL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
