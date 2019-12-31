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

int vs_get_headtype(int head)
{
    switch(head) {
        case CMD_ID_LOGIN_REQ:              
        case CMD_ID_LOGOUT_REQ:              
        case CMD_ID_PW_SET_REQ:
        case CMD_ID_RULE_GET_REQ:                
        case CMD_ID_RULE_SET_REQ:               
        case CMD_ID_LOG_GET_REQ:            
        case CMD_ID_DEBUG_REQ: 
        case CMD_ID_TIME_SYN_REQ: 
        case CMD_ID_IP_GET_REQ:
        case CMD_ID_IP_SET_REQ:
        case CMD_ID_HB_GET_REQ: 
        case CMD_ID_HB_SET_REQ:
            return HEAD_REQ_4;
            
        case CMD_ID_LOGIN_RES:             
        case CMD_ID_LOGOUT_RES:             
        case CMD_ID_PW_SET_RES:              
        case CMD_ID_RULE_GET_RES:
        case CMD_ID_RULE_SET_RES:
        case CMD_ID_LOG_GET_RES:
        case CMD_ID_DEBUG_RES:
        case CMD_ID_TIME_SYN_RES:
        case CMD_ID_IP_GET_RES:
        case CMD_ID_IP_SET_RES:
        case CMD_ID_ERR_FORMAT_RES:
        case CMD_ID_HB_GET_RES:  
        case CMD_ID_HB_SET_RES:
            return HEAD_RES_5;
     }

     return 0;
}

/* 命令ID编号,handler对应表   */
s_cmd2Handler  cmd2Handler[]=
{
		{	CMD_ID_LOGIN_REQ        , 			routine_login_req	    },
		{	CMD_ID_PW_SET_REQ       , 			routine_pw_set_req	 	},
		{	CMD_ID_RULE_GET_REQ     , 			routine_rule_get_req	},
		{	CMD_ID_RULE_SET_REQ     , 			routine_rule_set_req	},
		{	CMD_ID_LOG_GET_REQ      , 			routine_log_get_req	 	},	
		{	CMD_ID_TIME_SYN_REQ     , 			routine_time_syn_req	},
		{	CMD_ID_IP_GET_REQ       , 			routine_ip_get_req	 	},
		{	CMD_ID_IP_SET_REQ       , 			routine_ip_set_req	 	},
		{   CMD_ID_HB_GET_REQ       ,           routine_hb_get_req      },
		{   CMD_ID_HB_SET_REQ       ,           routine_hb_set_req      },
		{	CMD_ID_IP_RT_GET_REQ       , 		routine_ip_rt_get_req	},
		{	CMD_ID_IP_RT_SET_REQ       , 		routine_ip_rt_set_req	},


		{	CMD_ID_RTV_STR_REQ       , 		    routine_rtv_str_req	    },
		{	CMD_ID_RTV_END_REQ       , 		    routine_rtv_end_req	    },
		{	-1                      , 			NULL	 	            }
};

/* 查命令ID编号handler对应表，返回匹配数组单元号  */
static int handler_get(int cmd_id)
{
	int i;

	for(i=0; cmd2Handler[i].handler!= NULL; i++)
		if(cmd_id == cmd2Handler[i].cmd_id)
			return i;
	return -1;
}

/* 根据不同命令ID编号，查编号handler对应表，得到函数入口，调用该函数*/
int sm_handler(int cmd_id, void *handler_para)
{
	fun_handler handler;
	int ret;
	int num_match;
	
	handler = 0;

	num_match = handler_get(cmd_id);
	if(num_match < 0)   
	{
	    logger(TLOG_ERROR, "sm_handler cmd_id do not match, cmd_id=%d", cmd_id);
	    rountin_common_flag(CMD_ID_ERR_FORMAT_RES, MSG_RES_FLAG_ERR_FORMAT_CMD, 0);
		return -1;
    }
	
	handler = cmd2Handler[num_match].handler;
	ret = handler(handler_para);
	
	return 0;
}

/* 整形字节序反转: 100  x86为: 64 00 00 00  ppc为00 00 00 64, 
   p_int指向要转换的整形首地址    */
int sequence_convert_int(char *p_int)
{   
    int i;
    char *p_loc;
    char tmp_int[4];

    p_loc = p_int;
    memcpy(tmp_int, p_loc, 4);

    for(i=0; i<4; i++)
    {
        *p_loc = tmp_int[3 - i];
        p_loc++;    
    }

    return 0;  
}

// 协议头字节序转换, 转换连续几个整形的字节序
int sm_header_sequence_convert(char *cmd_header, int num)
{
    int i = 0;
    char *p_int;

    p_int = cmd_header;
    while(i < num)
    {
        sequence_convert_int(p_int);
        p_int += 4;
        i++;
    }

    return 0;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

