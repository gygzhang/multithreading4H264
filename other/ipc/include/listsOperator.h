


#ifndef __LISTOPERATOR_H__
#define __LISTOPERATOR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


struct member {
    char *data;
    TGAP_LIST_ENTRY(member) list;
 };

int list_node_add(char *buf, int buf_len);
int list_node_get(struct member **list_node);
int list_node_free(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __VS_SOCKET_H__ */
