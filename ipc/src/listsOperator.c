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

static TGAP_LIST_HEAD_STATIC(members, member);

// 链尾增加节点
int list_node_add(char *buf, int buf_len)
{
    struct member *list_node;

    list_node = (struct member *)calloc(1, sizeof(struct member));
    if(NULL == list_node)
    {
        logger(TLOG_WARNING, "no enough memory to malloc");
        return -1;
    }
    memcpy(list_node->data, buf, buf_len);

    TGAP_LIST_LOCK(&members);
    TGAP_LIST_INSERT_TAIL(&members, list_node, list);
    TGAP_LIST_UNLOCK(&members);
    return 0;
}

// 取头节点
int list_node_get(struct member **list_node)
{
    struct member *list_node;

    TGAP_LIST_LOCK(&members);
    *list_node = TGAP_LIST_REMOVE_HEAD(&members, list);
    TGAP_LIST_UNLOCK(&members);

    return 0;
}


// 删除链中所有节点
int list_node_free(void)
{
    struct member *mem;
    
    TGAP_LIST_LOCK(&members);
    while ((mem = TGAP_LIST_REMOVE_HEAD(&members, list))) {
        free(mem);
    TGAP_LIST_UNLOCK(&members);
    }
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

