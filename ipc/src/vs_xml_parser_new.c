/*****************************************************************
*     2007.9.5 针对 "视频服务器接口协议（VSP）3.0"的读取分析
*****************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdio.h>
#include <string.h>
#include <expat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "vs_xml_parser_new.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define BUFFSIZE        8192                // 仅仅用于测试时


// 所有会被子结构调用的,都在这里留一索引,并在find_sub_struct_desc关联起来使用
enum sub_struct_index
{
    DEMO_INDEX = 0,                      // 0
    Record_INDEX,                          // 1
    StoreRecord_INDEX,                 // 2
    StoreRecord2_INDEX,               // 3
    test_xml_INDEX
};

/********************************************************************
*                 记录结构体嵌套关系的结点结构
********************************************************************/
typedef struct tagSAVE_XML_PARSE_DATA
{
    FIELD_TYPE * fdarray;                 // 当前处理什么字段描述中,可能位置某个子struct中呢
    int fdcount;                          // 当前fdarray数组数
    int in_fdarray_index;               // 当前结点,在fdarray中的索引位置
    int getted_data;                       // 0,1, 当前结点的值已赋过,用来避免重复赋值错误
    int in_parent_fdarray_index;       // 当前结点,在父结构中fdarray中的索引位置
    int offset;                           // 当前结点,在当前结构体中的数据偏移位置
    char start_sub_structing;           // 0,还没碰到子结点,1,开始循环创建子结点,2结束了

    int loop_index;                      // 数组对象时,其总维数.
    int loop_current_count;             // 数组对象时,其当前总维数. (只第一个兄弟记录)
    char start_name[50];                 // 判断每个结点的名字

    struct tagSAVE_XML_PARSE_DATA * next;             // 兄弟之下一结点
    struct tagSAVE_XML_PARSE_DATA * first_prev;      // 兄弟第一个(每个兄弟都记录)
    struct tagSAVE_XML_PARSE_DATA * end_next;        // 兄弟最末一个(只第一个兄弟记录)

    struct tagSAVE_XML_PARSE_DATA * subfirst;        // 下属子struct之第一个
    struct tagSAVE_XML_PARSE_DATA * parent;          // 父对象

    struct tagSAVE_XML_PARSE_DATA * allsave_next;   // 统一管理时,单链表
}SAVE_XML_PARSE_DATA,*PSAVE_XML_PARSE_DATA;

/********************************************************************
*                 对各种结构体总体信息的备份
********************************************************************/
typedef struct tagFDARRAY_PTR
{
    INPUT_MESSAGE inputmsg;          // 放在第一位
    int index;
    FIELD_TYPE * fdarray;
    struct tagFDARRAY_PTR *next;
}FDARRAY_PTR,*PFDARRAY_PTR;

/********************************************************************
*                   解析环境参数总表
********************************************************************/
typedef struct PARSER_FUN_PARAM
{
    char *outbuf;                           // 返回结构体buf
    int write_buf_len;                      // 当写outbuf为xml时，记录其长度
    int ret_err;                              // 返回错误信息
    SAVE_XML_PARSE_DATA * head;
    SAVE_XML_PARSE_DATA * current;

    SAVE_XML_PARSE_DATA * allsave;
    SAVE_XML_PARSE_DATA * allsave_end;

    FDARRAY_PTR * fdarrayptr;     // fdarray索引表

    // 简易模式专用
    int easymode;                         // 是否简易模式 0,1
    char easymode_findname[50];             // 在easy模式下,需要查找的名称
    int easymode_ret_type;         // 简易模式下,需要返回的类型. int or string
    int easymode_finded;             // 找到没有 0: 尚未找到. 1.找到了但还未赋值. 2.已完成.

    void * fun;                               // 记住本次调用的函数名
}PARSER_FUN_PARAM,*PPARSER_FUN_PARAM;

/********************************************************************
*                         中间过渡函数
********************************************************************/
static int find_xml_value(void *data, int index, const char *el);
static void XMLCALL start_xml(void *data, const char *el, const char **attr);
static int XMLCALL XMLEncodingHandle(void *data, const char *el, XML_Encoding *info);
//int GBK_convert(void *data, const char *s);
static void XMLCALL end_xml(void *data, const char *el);
static void XMLCALL chardatahandle(void *data, const char *el, int len);

// --------------------------------------------------------------------------------------
/********************************************************************
*                          测试 结构体 定义
********************************************************************/
typedef struct tagRecord
{
    char BeginTime[100];
    char EndTime[100];
}Record;
typedef struct tagStoreRecord
{
    int DevType;
    int ID;
    Record record[8];
}StoreRecord;
typedef struct tagStoreRecord2
{
    int DevType;
    int ID;
    Record record[8];
}StoreRecord2;
typedef struct tagtest_xml
{
    int ID[3];
    StoreRecord storerecord[3];
    StoreRecord2 storerecord2[3];
    int ID2;
}test_xml;







/************************************************************************
 * 函数名: malloc_struct
 * 作  者: sx
 * 描  述: 根据输入的两个结构体描述信息,备份一个INPUT_MESSAGE,并malloc
 *          出另一个FIELD_TYPE*的数据
 * 输入参数: INPUT_MESSAGE *in_inputmsg,描述的总体信息
 *          FIELD_TYPE * in_fdarray,描述的各字段的详细信息
 * 输出参数: char *out_msg_buf,复制的 INPUT_MESSAGE *in_inputmsg信息
 * 返回值: 复制的 FIELD_TYPE * in_fdarray 详细信息. (malloc出来的)
************************************************************************/
char * malloc_struct( INPUT_MESSAGE *in_inputmsg,
                            char *out_msg_buf,
                            FIELD_TYPE * in_fdarray)
{
    FIELD_TYPE * buf_fdarray;
    int i;
    if ( in_inputmsg == NULL )
    {
        return NULL;
    }
    if ( in_inputmsg->input_fdarray_count > 10000 || in_inputmsg->input_fdarray_count <= 0)
    {
        return NULL;
    }

    buf_fdarray = malloc( in_inputmsg->input_fdarray_count * sizeof(FIELD_TYPE) );
    if (buf_fdarray)
    {
        memcpy(buf_fdarray, in_fdarray, in_inputmsg->input_fdarray_count * sizeof(FIELD_TYPE) );
        // 校验数组一个变量值:非结构体的数组 如 int a[3]
        for(i=0; i <in_inputmsg->input_fdarray_count; i++)
        {
            if ( buf_fdarray[i].type != TYPE_STRUCT && buf_fdarray[i].repeat > 1 )
                buf_fdarray[i].only_struct_index = 0;       // 这是为了以后统计数组当前总数使用.
        }
    }
    memcpy( out_msg_buf, in_inputmsg, sizeof(INPUT_MESSAGE) );
    return (char *)buf_fdarray;
}


/************************************************************************
 * 函数名: find_sub_strcut_desc
 * 作  者: sx
 * 描  述: 根据索引信息,返回该结构体的两种描述信息
 *           1.这个函数,可以根据以下方式,构造自己的函数,并将函数名做为
 *             xml2struct,struct2xml的第4个参数传进来.
 *           2.下面函数里,都是做示范的.真正的struct描述信息,要使用者来完成
 * 输入参数: int index,索引
 *            FIELD_TYPE * in_fdarray,描述的各字段的详细信息
 * 输出参数: char *out_msg_buf,复制的 INPUT_MESSAGE *in_inputmsg信息
 * 返回值: 复制的 FIELD_TYPE * in_fdarray 详细信息. (malloc出来的)
************************************************************************/
char * find_sub_strcut_desc(int index, INPUT_MESSAGE *out_inputmsg)
{
    if (index == Record_INDEX)
    {
        INPUT_MESSAGE msg = { "Record",2,TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"BeginTime",      100,               TYPE_STRING,   0, 0 },
            {"EndTime",        100,               TYPE_STRING,   0, 0 }
        };

        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == StoreRecord_INDEX)
    {
        INPUT_MESSAGE msg = { "StoreRecord",3,TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"DevType",        sizeof(int),       TYPE_INT,      0, 0 },
            {"ID",             sizeof(int),       TYPE_INT,      0, 0 },
            {"Record",         sizeof(Record),TYPE_STRUCT, 8, Record_INDEX }
        };
            
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == test_xml_INDEX)
    {
        INPUT_MESSAGE msg = { "test_xml",3,TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"ID",             sizeof(int),       TYPE_INT,          3, 0 },
            {"StoreRecord",    sizeof(StoreRecord),TYPE_STRUCT,3, StoreRecord_INDEX },
            {"StoreRecord2",       sizeof(StoreRecord2),TYPE_STRUCT,3, StoreRecord_INDEX },
        };

        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    };
    return NULL;
}

/************************************************************************
* 函数名: find_fdarray
* 作  者: sx
* 描  述: 1.根据索引信息,首先从以前的信息中查找,如果没有找到,就调用指定fun函数里,
*           查找出新的结构体描述信息,用单链表保存起来
*         2.内部函数.
* 输入参数: PARSER_FUN_PARAM *pf,数据集合
*           int index,索引
*            __find_sub_strcut_desc fun,指定的fun函数
* 输出参数: 无
* 返回值: 返回一个结构体描述信息(一个结点).
************************************************************************/
FDARRAY_PTR * find_fdarray( PARSER_FUN_PARAM *pf, int index, __find_sub_strcut_desc fun )
{
    FDARRAY_PTR *next;
    FDARRAY_PTR *end_next;
    
    next = pf->fdarrayptr;
    end_next = next;
    while( next != NULL )
    {
        end_next = next;
        if ( next->index == index )
        {
            return next;
        }
        next = end_next->next;
    }
    
    // 用个简单的单链表保存
    next = malloc( sizeof(FDARRAY_PTR) );
    if ( next == NULL )
        return NULL;
    memset( next,0,sizeof(FDARRAY_PTR) );
    // 没有找到
    next->fdarray = (FIELD_TYPE *)fun(index,&next->inputmsg);
    next->index = index;
    if ( pf->fdarrayptr == NULL )
    {
        pf->fdarrayptr = next;
    }
    else
    {
        end_next->next = next;
    }
    return next;
}

/************************************************************************
* 函数名: find_xml_value
* 作  者: sx
* 描  述: 1.当找到一个关键词及关键词的值后,根据结构体描述信息,确定对应的
*           值在整个结构体中的偏移地址.赋值
*         2.内部函数.
* 输入参数: void *data,数据集合,内部既有输入数据,也有输出数据
*           int index,在结构体描述信息中的索引(数组下标)
*           const char *el,关键词的值
* 输出参数: 无
* 返回值: 0表示成功. >0表示错误编号
************************************************************************/
int find_xml_value(void *data, int index, const char *el)
{
    PARSER_FUN_PARAM *pf;
    int n;
    int i;
    int sum;

    pf = (PARSER_FUN_PARAM *)data;

    sum = pf->current->offset;
    for ( i = 0; i < index; i ++ )
    {
        n = pf->current->fdarray[i].repeat > 1 ? pf->current->fdarray[i].repeat : 1;
        sum += n * pf->current->fdarray[i].size;
    }

    if ( pf->current->fdarray[index].type != TYPE_STRUCT && pf->current->fdarray[index].repeat > 1 )
    {
        // 对于非结构体的数组来说,因为没有结点信息,所以在此取巧,
        // 将数组的当前维数,保存到 pf->current->fdarray[index].only_struct_index中,
        // 因为类似int[10]这种非结构体,是不使用only_struct_index变量的.
        sum += pf->current->fdarray[index].only_struct_index * pf->current->fdarray[index].size;    // 数组位置偏移
        pf->current->fdarray[index].only_struct_index ++;

        // 同时,进行数组维数限制
        n = pf->current->fdarray[index].repeat > 1 ? pf->current->fdarray[index].repeat : 1;
        if (pf->current->fdarray[index].only_struct_index > n )
        {
            // 失败,维数不足
            pf->ret_err |= XML_ERR_ARRAY_LESS;
            return pf->ret_err;
        }
    }

    if( TYPE_INT == pf->current->fdarray[index].type )
    {
        // 因为此处有可能是bool型,所以先排除
        n = strlen( el );
        if ( n == 0 || el[0] == 'f' || el[0] == 'F' )
        {
            n = 0;
        }
        else if ( el[0] == 't' || el[0] == 'T' )
        {
            n = 1;
        }
        else
        {
            n = atoi( el );
        }
        memcpy( pf->outbuf + sum, &n, sizeof(int) );
    }
    else if ( TYPE_STRING == pf->current->fdarray[index].type )
    {
        n = strlen( el );
        memcpy( pf->outbuf + sum, el, n );
        pf->outbuf[sum +n] = '\0';
    }
    else if ( TYPE_BOOL == pf->current->fdarray[index].type )
    {
        /* bool 应该都是用 int 表示的 */
        n = strlen( el );
        if ( n > 0 && (el[0] == 't' || el[0] == 'T') ) {
            n = 1;
        }
        else {
            n = 0;
        }
        memcpy( pf->outbuf + sum, &n, sizeof(int) );
    }
    else if ( TYPE_CHAR == pf->current->fdarray[index].type )
    {
        n = atoi( el );
        memcpy( pf->outbuf + sum, &n, sizeof(int) );
    }
    else if ( TYPE_IP_ADDR == pf->current->fdarray[index].type )
    {
        struct in_addr addr;
        addr.s_addr = 0;
        n = inet_aton(el, &addr);
        memcpy( pf->outbuf + sum, &addr.s_addr, sizeof(unsigned long int) );
    }
    else if ( TYPE_TIME_LDT == pf->current->fdarray[index].type )
    {
         // 日期型,还没弄清楚linux下怎么应用及分类的.linux c有date型么?
    }
    
    return 0;
}

/************************************************************************
* 函数名: start_xml
* 作  者: sx
* 描  述: 1.解析XML必须的函数,用于处理找到标签头及属性值
*         2.内部函数.
*         3.实现支持全部结构体嵌套,及忽略嵌套只查找某一值的简易方式
* 输入参数: void *data,数据集合,内部既有输入数据,也有输出数据
*           const char *el,标签头名称
*           const char **attr,标签的属性,分别是(关键词,关键词的值)(,)
* 输出参数: 无
* 返回值: 无
************************************************************************/
static void XMLCALL start_xml(void *data, const char *el, const char **attr)
{
    int i;
    int j;
    int k;
    int sum;

    PARSER_FUN_PARAM *pf;
    pf = (PARSER_FUN_PARAM *)data;

    // 简易模式
    if (pf->easymode)
    {
        if (pf->easymode_finded == 2)
        {
            return;
        }
        if ( strcmp(el, pf->easymode_findname)==0 )
        {
            pf->easymode_finded = 1;
            return;
        }
        /* 处理属性 attr[] */
        for (i = 0; attr[i]; i += 2)
        {
            if ( strcmp(attr[i], pf->easymode_findname) == 0 )
            {
                strcpy( pf->outbuf, attr[i+1] );                    // 赋值,然后结束了
                pf->easymode_finded = 2;
                return;
            }
        }
        return;
    }

    /* 处理 *el */
    pf->current->in_fdarray_index = -1;
    pf->current->getted_data = 0;
    for (i = 0; i < pf->current->fdcount; i ++)
    {
        if ( strcmp(el, pf->current->fdarray[i].name) == 0 )
        {
            pf->current->in_fdarray_index = i;      // 当前找到的位于
            break;
        }
    }
    if ( pf->current->in_fdarray_index < 0)         // 可能是本身结构体的属性
    {
        /* 处理属性 attr[] */
        for (i = 0; attr[i]; i += 2)
        {
            for (j = 0; j < pf->current->fdcount; j ++)
            {
                if ( strcmp(attr[i], pf->current->fdarray[j].name) == 0 )
                {
                    find_xml_value( data, j, attr[i+1] );
                    break;
                }
            }
        }
        return;
    }
    // 规则一: start逢struct,进一,end逢struct,退一
    // 规则二: start逢struct,搜索是否已存在,无则建,有则追加
    if ( pf->current->fdarray[ pf->current->in_fdarray_index ].type == TYPE_STRUCT )        // 当前位置是一个结点
    {
        // 增加一个新的子结点,并将当前结点设到此新结点上
        SAVE_XML_PARSE_DATA *sub;
        SAVE_XML_PARSE_DATA *next;
        SAVE_XML_PARSE_DATA *search_end;
        FDARRAY_PTR *fdarrayptr;
        int sub_fdcount;
        
        sub_fdcount = 0;
        // 查找该结点的描述信息
        fdarrayptr = find_fdarray( pf, pf->current->fdarray[ pf->current->in_fdarray_index ].only_struct_index, pf->fun );
        if (fdarrayptr != NULL)
        {
            sub_fdcount = fdarrayptr->inputmsg.input_fdarray_count;
        }

        if (fdarrayptr == NULL || fdarrayptr->fdarray == NULL )
        {
            // 如果没有找到.则end也不处理
            pf->current->in_fdarray_index = -1;
            pf->ret_err |= XML_ERR_NO_FIND_DESC;
            return;
        }

        // 搜索一下,前面是否已有此结构体
        sub = NULL;
        next = pf->current->subfirst;
        search_end = next;
        while (next != NULL)
        {
            search_end = next;
            if ( strcmp(next->start_name, el) == 0 )
            {
                sub = next;
            }
            next = next->next;
        }

        if (sub)
        {
            SAVE_XML_PARSE_DATA *prev;

            if ( sub->first_prev->loop_current_count >= pf->current->fdarray[ pf->current->in_fdarray_index ].repeat )
            {
                pf->current->in_fdarray_index = -1;
                pf->ret_err |= XML_ERR_ARRAY_LESS;
                return;
            }

            // 已找到同类的最后一个
            prev = sub;
            sub = malloc( sizeof(SAVE_XML_PARSE_DATA) );
            if (!sub)
            {
                pf->current->in_fdarray_index = -1;
                pf->ret_err |= XML_ERR_MEMORY;
                return;
            }
            memset(sub,0,sizeof(SAVE_XML_PARSE_DATA));
            strcpy(sub->start_name, el);

            sub->parent = prev->parent;          // 父子关系
            prev->next = sub;                          // 兄弟关系对应
            sub->first_prev = prev->first_prev; // 第一个
            sub->first_prev->end_next = sub;   // 记录最末一个

            sub->in_parent_fdarray_index = pf->current->in_fdarray_index;
            sub->fdarray = fdarrayptr->fdarray;
            sub->fdcount = sub_fdcount;

            // 根据上一个,计算一些数据保存: 偏移,索引等
            sub->loop_index = prev->loop_index +1;  // 俺是下一个
            sub->first_prev->loop_current_count = sub->loop_index;  // 记录当前总数
            sub->offset = prev->offset + pf->current->fdarray[ pf->current->in_fdarray_index ].size;    // 在前一个基础上,再前进一个struct长度

            // 进一
            pf->current = sub;

            pf->allsave_end->allsave_next = sub;
            pf->allsave_end = sub;
        }
        else
        {
            // 之前没有遇到过这个结构体
            sub = malloc( sizeof(SAVE_XML_PARSE_DATA) );
            if (!sub)
            {
                pf->current->in_fdarray_index = -1;
                pf->ret_err |= XML_ERR_MEMORY;
                return;
            }
            // 设置链表关系
            memset(sub,0,sizeof(SAVE_XML_PARSE_DATA));
            strcpy(sub->start_name, el);

            if (search_end != NULL)
            {
                search_end->next = sub;         // 根据兄弟关系确定
                sub->first_prev = search_end->first_prev;
            }
            else
            {
                pf->current->subfirst = sub;    // 父子关系对应
                sub->first_prev = sub;          // 第一个
            }
            sub->parent = pf->current;
            sub->first_prev->end_next = sub;            // 记录最末一个

            sub->in_parent_fdarray_index = pf->current->in_fdarray_index;
            sub->fdarray = fdarrayptr->fdarray;
            sub->fdcount = sub_fdcount;

            // 结点关系构成. 再计算当前结点在outbuf中的偏移地址
            sub->loop_index = 1;            // 俺是第一个
            sub->first_prev->loop_current_count = sub->loop_index;  // 记录当前总数
            
            // 计算出此子struct的内存位置,因为是数组中的第一个,所以 offset = parent.end.offset + pf->current->in_fdarray_index之间的
            sum = 0;
            for ( j = 0; j < pf->current->in_fdarray_index; j++ )
            {
                k = pf->current->fdarray[j].repeat > 1 ? pf->current->fdarray[j].repeat : 1;
                sum += pf->current->fdarray[j].size * k;
            }
            sub->offset = pf->current->offset + sum;

            // 进一
            pf->current = sub;

            if ( pf->allsave == NULL )
                pf->allsave = sub;
            else
                pf->allsave_end->allsave_next = sub;
            pf->allsave_end = sub;
        }
    }
    /* 处理属性 attr[] */
    for (i = 0; attr[i]; i += 2)
    {
        for (j = 0; j < pf->current->fdcount; j ++)
        {
            if ( strcmp(attr[i], pf->current->fdarray[j].name) == 0 )
            {
                find_xml_value( data, j, attr[i+1] );
                break;
            }
        }
    }
}

/************************************************************************
* 函数名: end_xml
* 作  者: sx
* 描  述: 1.解析XML必须的函数,用于处理找到标签尾
*         2.内部函数.
*         3.主要处理嵌套关系
* 输入参数: void *data,数据集合,内部既有输入数据,也有输出数据
*           const char *el,标签尾名称
* 输出参数: 无
* 返回值: 无
************************************************************************/
static void XMLCALL end_xml(void *data, const char *el)
{
    PARSER_FUN_PARAM *pf;
    pf = (PARSER_FUN_PARAM *)data;

    if ( pf->easymode > 0 || pf->current == NULL)       // 简易模式下,根本不用这个函数
    {
        return;
    }
    // 上次start根本没有找到这个标签.此时也就不理它
    if ( pf->current->in_fdarray_index < 0 )
    {
        return;
    }

    if ( pf->current->parent != NULL && pf->current->parent->fdarray[ pf->current->in_parent_fdarray_index].type == TYPE_STRUCT )       // 当前位置是一个结点
    {
        // 规则一: start逢struct,进一,end逢struct,退一
        pf->current = pf->current->parent;
    }
}

/************************************************************************
* 函数名: chardatahandle
* 作  者: sx
* 描  述: 1.解析XML必须的函数,用于处理找到的标签中间的内容.即关键词的值
*         2.内部函数.
* 输入参数: void *data,数据集合,内部既有输入数据,也有输出数据
*           const char *el,值,没有\0结尾
*           int len,值的长度,用来判断el的有效值长度
* 输出参数: 无
* 返回值: 无
************************************************************************/
static void XMLCALL chardatahandle(void *data, const char *el, int len)
{
    char *buf;
    PARSER_FUN_PARAM *pf;
    pf = (PARSER_FUN_PARAM *)data;

    // 前面的start标签,并没有在当前结点的描述信息中找到
    // 对于<A struct>空格</A> 型,里面的空格容易将结构体的第一个数据清空.所以此判断条件忽略.
	if ( (pf->easymode && pf->easymode_finded != 1) 
		|| (pf->easymode == 0 && pf->current->in_fdarray_index < 0)
		|| (pf->easymode == 0 && pf->current->parent != NULL && pf->current->parent->fdarray[ pf->current->in_parent_fdarray_index].type == TYPE_STRUCT ) )
	{
		return;
	}

    
    // 现在一个问题，当<A>1</A>    <B>2</B>中的</A>执行完后，
    // 又可能出现一群空格的值,即</A>   <B>之间的空格, 怎么办呢？
    if ( !pf->easymode && pf->current->getted_data == 1 )        // 如果已经赋过值
    {
        return;
    }
    buf = malloc(len +2);
    if (!buf)
    {
        return;
    }

    memcpy( buf, el, len );
    buf[ len ] = '\0';
    if ( pf->easymode )
    {
        // 简易模式专用
        strcpy( pf->outbuf, buf );
        pf->easymode_finded = 2;            // 只找第一个
    }
    else
    {
        find_xml_value(data, pf->current->in_fdarray_index, buf);
        pf->current->getted_data = 1;          // 赋完值后，就避免下次重复赋个错误的空格值
    }
    free(buf);
}

/************************************************************************
* 函数名: XMLEncodingHandle
* 作  者: sx
* 描  述: 1.解析XML必须的函数,用于处理不可识别的编码，比如GBK，GB2312，BIG5等等
*         2.内部函数.
* 输入参数: void *data,数据集合,内部既有输入数据,也有输出数据,在此处用不上
*           const char *name, 编码名字,如"GBK"
*           XML_Encoding *info, 一个指定编码解析转换的结构
* 输出参数: 无
* 返回值: 无
************************************************************************/
static int XMLCALL XMLEncodingHandle(void *data, const char *name, XML_Encoding *info)
{
   //  map[i]  值意义,
   //      >=0 表示对应的值是单字符
   //      -1  表示非法字符
   //      -2,-3,-4    表示是多字节字符的第一个,分别是2字节、3字节、4字节的字符
   //  使用:
   //      expat会根据map[(unsigned char)s[i]] 的值判断某个字符后面跟多少个字节，然后跳过对应的数目
   //  
   //  info->data    附加数据，这个只会回传给convert和release,expat内部不进行处理,
   //  expat解析过程中会调用convert,解析完后调用release(如果这两个值不为NULL的话)
   //  如果是单字节字符集，那convert可以为NULL,
   //  如果是多字节安符集,convert应该有值,负责把对应的语言转换成ucs2编码
    int   i;
    if( name != NULL && ( strcasecmp(name,"GBK") == 0 || strcasecmp(name,"GB2312") == 0 ) )
    {
        for(i=0;i<128;i++)
            info->map[i]   =   i;
        for(;i<256;i++)
                info->map[i]   =   -2;
        
        // 既然认为协议v3.0发过来的信息没有汉字，那么我也可以偷懒不处理了。
        info->convert       =   NULL;       // GBK_convert
        info->release       =   NULL;
        return   1;//XML_STATUS_OK;
    }

    return   XML_STATUS_ERROR;

}

//int GBK_convert(void *data, const char *s)
//{
//    return 0;
//}

/************************************************************************
* 函数名: free_pf
* 作  者: sx
* 描  述: 1.将数据集合中所malloc出来的嵌套结点数据free
*         2.内部函数.
* 输入参数: PARSER_FUN_PARAM *pf,数据集合
* 输出参数: 无
* 返回值: 无
************************************************************************/
void free_pf(PARSER_FUN_PARAM *pf)
{
    SAVE_XML_PARSE_DATA * allsave_next;
    SAVE_XML_PARSE_DATA * current;

    if ( pf == NULL || pf->allsave == NULL)
    {
        return;
    }

    allsave_next = pf->allsave;
    current = allsave_next;
    while( current != NULL )
    {
        allsave_next = current->allsave_next;
        free( current );
        current = allsave_next;
    }
}

/************************************************************************
* 函数名: free_fdarray
* 作  者: sx
* 描  述: 1.将数据集合中所malloc出来的结构体描述信息free
*         2.内部函数.
* 输入参数: PARSER_FUN_PARAM *pf,数据集合
* 输出参数: 无
* 返回值: 无
************************************************************************/
void free_fdarray(PARSER_FUN_PARAM *pf)
{
    FDARRAY_PTR * fdarrayptr;
    FDARRAY_PTR * current;

    if( pf == NULL || pf->fdarrayptr == NULL )
    {
        return;
    }

    fdarrayptr = pf->fdarrayptr;
    current = fdarrayptr;
    while ( current != NULL )
    {
        fdarrayptr = current->next;
        if ( current->fdarray != NULL )
        {
            free(current->fdarray);
        }
        free(current);
        current = fdarrayptr;
    }
}

/************************************************************************
* 函数名: vs_xml_parser
* 作  者: sx
* 描  述: 测试函数
* 输入参数: 无
* 输出参数: 无
* 返回值: 无
************************************************************************/
//#ifdef AMIGA_SHARED_LIB
//#include <proto/expat.h>
//int
//amiga_main(int argc, char *argv[])
//#else
int vs_xml_parser()
//main()
//#endif
{
    char *Buff2;
    test_xml ptest;
    int ret_err;
    int n;
    char ret_string[300];
    memset( &ptest, 0, sizeof( test_xml ) );      // 尽量初始化0

    ret_err = sizeof(char) + sizeof(int);

    Buff2 = malloc( BUFFSIZE );
    memset( Buff2, 0, BUFFSIZE );


    //printf("start------------------\n");

    // 正确型式
    strcpy( Buff2 ,"<Message>"
        "<ID>10001</ID>"
        "<ID>10002</ID>"
        "<ID>10003</ID>"

        "<Record BeginTime = \"EXIT-MM-DD HH:MM:SS\" EndTime = \"EXIT-MM-DD HH:MM:SS\" />"

        "<StoreRecord DevType=\"100000\" ID=\"1111\" >"
        "<Record BeginTime = \"1-MM-DD HH:MM:SS\" EndTime = \"11-MM-DD HH:MM:SS\" />"
        "<Record BeginTime = \"2-MM-DD HH:MM:SS\" EndTime = \"22-MM-DD HH:MM:SS\" />"
        "</StoreRecord>"

        "<StoreRecord2 DevType=\"999999\" ID=\"999\" >" 
        "<Record BeginTime = \"2___0-MM-DD HH:MM:SS\" EndTime = \"2___11-MM-DD HH:MM:SS\" />"
        "<Record BeginTime = \"2___0-MM-DD HH:MM:SS\" EndTime = \"2___22-MM-DD HH:MM:SS\" />"
        "</StoreRecord2>"

        "<StoreRecord DevType=\"200000\" ID=\"2222\" >"
        "<Record BeginTime = \"3-MM-DD HH:MM:SS\" EndTime = \"33-MM-DD HH:MM:SS\" />"
        "<Record BeginTime = \"4-MM-DD HH:MM:SS\" EndTime = \"44-MM-DD HH:MM:SS\" />"
        "<Record BeginTime = \"5-MM-DD HH:MM:SS\" EndTime = \"55-MM-DD HH:MM:SS\" />"
        "</StoreRecord>"

        "</Message>");

    //printf( "\r\n原始的xml字符串为:\r\n%s\r\n", Buff2 );

    //printf("\r\ndo 1------------------xml2struct\r\n");
    ret_err = xml2struct( Buff2, strlen(Buff2),
        (char *)&ptest, test_xml_INDEX, find_sub_strcut_desc );

    //printf("第一个storerecord的record[0]的BeginTime 应该是1-MM-DD HH:MM:SS, 实际呢？\r\n%s\r\n", ptest.storerecord[0].record[0].BeginTime);
    //printf("第一个storerecord的record[2]的BeginTime 应该空, 实际呢？\r\n%s\r\n", ptest.storerecord[0].record[2].BeginTime);
    //printf("第二个storerecord的record[1]的BeginTime 应该是4-MM-DD HH:MM:SS, 实际呢？\r\n%s\r\n", ptest.storerecord[1].record[1].BeginTime);

    //printf("\r\ndo 2------------------findxml_string\r\n");
    ret_err = findxml_string( Buff2, strlen(Buff2), "DevType", ret_string );
    //printf( "查找到的(string) DevType=%s\r\n", ret_string );

    //printf("\r\ndo 3------------------findxml_int\r\n");
    ret_err = findxml_int( Buff2, strlen(Buff2), "DevType", &n );
    //printf( "查找到的(int) DevType=%d\r\n", n );

    //printf("\r\ndo 4------------------struct2xml\r\n");
    memset( Buff2,0,BUFFSIZE );
    ret_err = struct2xml( (char*)&ptest, Buff2, test_xml_INDEX, find_sub_strcut_desc );
    n = strlen(Buff2);

    //printf( "利用上面struct数据，生成的xml为：\r\n%s\r\n", Buff2 );
    //printf("--------- 返回所需长度为 return len = %d,   实际生成的xml长度为 strlen( new_xml_buf ) = %d \r\n", ret_err, n );

    free( Buff2 );


    //printf("end------------------\n");
    return 0;
}

/************************************************************************
* 函数名: my_xml_parse
* 作  者: sx
* 描  述: 1.封装了expat的解析函数,供xml2struct内部调用
*         2.内部函数
* 输入参数: PARSER_FUN_PARAM *pf,数据集合
*           char *xml_string,顾名思义
* 输出参数: 无
* 返回值: 0表示成功,>0表示错误编号
************************************************************************/
int my_xml_parse(PARSER_FUN_PARAM *pf, char *xml_string, int xml_len)
{
    XML_Parser p;

    p = XML_ParserCreate( NULL );
    if (!p)
    {
        fprintf( stderr, "无法创建XML解析器\n" );
        return -1;
    }

    /* 向句柄上设定准备开始和结束签条 */
    XML_SetElementHandler( p, start_xml, end_xml );

    /* 设置元素内文本的处理函数 */
    XML_SetCharacterDataHandler( p, chardatahandle );

    /* 一次性解析完 */
    //xml_len = strlen( xml_string );

    /* 设置一个共享的数据结构给各各处理函数使用 */
    XML_SetUserData( p, pf );

    /* 把GBK,GB2312等不识别的编码，过滤掉，因为目前仅认为传过来的是英文 */
    XML_SetUnknownEncodingHandler( p, XMLEncodingHandle, 0 );
    if (XML_Parse( p, xml_string, xml_len, 1 ) == XML_STATUS_ERROR)
    {
#if 0
        fprintf(stderr, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
            XML_GetCurrentLineNumber( p ),
            XML_ErrorString(XML_GetErrorCode( p ) ) );
        /* 释放被解析器使用的内存。*/
#endif
        XML_ParserFree( p );
        return -1;
    }
    /* 释放被解析器使用的内存。*/
    XML_ParserFree( p );
    return pf->ret_err;
}

/************************************************************************
* 函数名: xml2struct
* 作  者: sx
* 描  述: 将xml语句解析成struct中的内容
* 输入参数: char *xml_string,一段XML语句
*           char *outbuf,最后返回的结构体内部空间
*           int struct_index,主结构体在描述函数中的索引
*           __find_sub_strcut_desc fun,自定义的结构体描述函数
* 输出参数: 无
* 返回值: 0表示成功,>0表示错误编号
************************************************************************/
int xml2struct( char *xml_string, int xml_len, char *outbuf, int struct_index, __find_sub_strcut_desc fun )
{
    int fdcount;
    int ret_err;
    PARSER_FUN_PARAM pf;
    SAVE_XML_PARSE_DATA savept;

    FDARRAY_PTR * fdarrayptr;

    if ( xml_string == NULL || outbuf == NULL || fun == NULL )
        return -1;

    memset( &pf, 0, sizeof(PARSER_FUN_PARAM) );
    memset( &savept, 0, sizeof(SAVE_XML_PARSE_DATA) );

    pf.fun = (void *)fun;                           // 记住调用函数
    fdarrayptr = find_fdarray( &pf, struct_index, fun );
    if ( fdarrayptr != NULL && fdarrayptr->fdarray != NULL )
        fdcount = fdarrayptr->inputmsg.input_fdarray_count;
    else
    {
        free_pf( &pf );                       // 在find_fdarray()里有申请内存,这里要free
        free_fdarray( &pf );
        return XML_ERR_NO_FIND_DESC;        // 未找到结构体定义
    }

    savept.offset = 0;
    savept.fdarray = fdarrayptr->fdarray;
    savept.fdcount = fdcount;
    savept.in_fdarray_index = -1;
    pf.outbuf = outbuf;                        // 最后返回到此处
    pf.head = &savept;
    pf.current = pf.head;

    ret_err = my_xml_parse( &pf, xml_string, xml_len );         // 调用expat,将xml解析到pf中

    free_pf( &pf );
    free_fdarray( &pf );

    return ret_err;
}

/************************************************************************
* 函数名: findxml_int
* 作  者: sx
* 描  述: 将xml语句解析成struct中的内容
* 输入参数: char *xml_string,一段XML语句
*           find_str,将要搜索的关键词
* 输出参数: int *out,返回的关键词的值
* 返回值: 0表示成功,>0表示错误编号
************************************************************************/
int findxml_int( char *xml_string, int xml_len, char *find_str, int *out )
{
    char outbuf[50];
    int ret_err;

    ret_err = findxml_string( xml_string, xml_len, find_str, outbuf );
    if (ret_err == 0)
        *out = atoi(outbuf);
    else
        *out = -1;

    return ret_err;
}

/************************************************************************
* 函数名: findxml_string
* 作  者: sx
* 描  述: 将xml语句解析成struct中的内容
* 输入参数: char *xml_string,一段XML语句
*           find_str,将要搜索的关键词
* 输出参数: char *outbuf,最后返回的字符串
            如: xml中有:  BeginTime = "YYYY-MM-DD HH:MM:SS"
                find_str: BeginTime
            则: 返回值 0， outbuf中存放YYYY-MM-DD HH:MM:SS
* 返回值: 0表示成功,>0表示错误编号
************************************************************************/
int findxml_string( char *xml_string, int xml_len, char *find_str, char *outbuf )
{
    PARSER_FUN_PARAM pf;

    memset( &pf, 0, sizeof(PARSER_FUN_PARAM) );
    pf.easymode = 1;
    pf.outbuf = outbuf;
    strcpy( pf.easymode_findname, find_str );
    return my_xml_parse(&pf, xml_string, xml_len);            // 调用expat,将xml解析到pf中
}

/************************************************************************
* 函数名: makexmlstring
* 作  者: sx
* 描  述: 将struct转换成XML语句,此函数用于递归,实现嵌套struct
* 输入参数: PARSER_FUN_PARAM *pf,数据集合
*           char *pstruct,该结构体的指针
*           int struct_index,该结构体的索引
*           __find_sub_strcut_desc fun, 指定的查找结构体描述信息之函数名
* 输出参数: char *outbuf,最后返回的xml字符串
* 返回值: 是否新增加了字符串
************************************************************************/
int makexmlstring( PARSER_FUN_PARAM *pf, char *pstruct, char *outbuf,
                  int struct_index, int offset )
{
    int i;
    int j;
    int fdcount;
    int input_type;
    int in_offset;

    // 0, 尚未结束, 如: <A
    // 1, 结束了一半, 如 <A>
    // 2, 全部结束, 如 <A />, <A></A>
    int closed;
    int loop;
    int attribshow;
    int itemp;
    char *ptemp;
    char cc;

    int after_closed_have;

    FDARRAY_PTR * fdarrayptr;
    char ctemp[256];
    struct in_addr ip_temp;
    int outbuf_len;
    int addnewinfo;
    int old_write_buf_len;

    old_write_buf_len = pf->write_buf_len;
    outbuf_len = 0;
    if (outbuf)
        outbuf_len = strlen(outbuf);    // 记录一下当前长度,如果没能新增内容,则把多余无效的内容删除
    addnewinfo = 0;

    fdarrayptr = find_fdarray( pf, struct_index, pf->fun );
    if ( fdarrayptr == NULL && fdarrayptr->fdarray == NULL )
    {
        return 0;
    }
    fdcount = fdarrayptr->inputmsg.input_fdarray_count;
    input_type = fdarrayptr->inputmsg.input_type;

    closed = 0;
    after_closed_have = 0;
    if (outbuf)
    {
        sprintf( ctemp, "<%s", fdarrayptr->inputmsg.name );
        strcat( outbuf, ctemp );
    }
    pf->write_buf_len = pf->write_buf_len + 1 + strlen(fdarrayptr->inputmsg.name);

    in_offset = 0;
    for ( i = 0; i < fdcount; i ++ )
    {
        // 结构体,进行递归
        if ( fdarrayptr->fdarray[i].type == TYPE_STRUCT )
        {
            if ( closed == 0 )
            {
                if (outbuf)
                {
                    strcat( outbuf, ">\r\n" );
                }
                closed = 1;
                pf->write_buf_len += 3;
            }

            loop = fdarrayptr->fdarray[i].repeat > 1 ? fdarrayptr->fdarray[i].repeat : 1;
            for ( j = 0; j < loop; j ++ )
            {
                itemp = makexmlstring(pf, pstruct, outbuf,
                    fdarrayptr->fdarray[i].only_struct_index,
                    offset + in_offset + j * fdarrayptr->fdarray[i].size);
                addnewinfo += itemp;
            }
        }
        else
        {
            // 强制要求 <A> </A>型时,或强制要求<A>1</A> <B>2<B> 列表显示属性的
            // 如果是数组,也只能把标签按<A> </A>处理
            if ( closed == 0
                && ((input_type & FULL_CLOSE) == FULL_CLOSE
                || (input_type & LIST_SHOW) == LIST_SHOW 
                || fdarrayptr->fdarray[i].repeat > 1 ) )
            {
                if (outbuf)
                {
                    strcat( outbuf, ">\r\n" );
                }
                pf->write_buf_len += 3;
                closed = 1;
            }

            attribshow = 0;
            // 如果现在还是close == 0 且要求<A a=1 b=2 />型显示属性
            if ( closed == 0 && (input_type & ATTRIB_SHOW) == ATTRIB_SHOW )
            {
                attribshow = 1;
            }

            loop = fdarrayptr->fdarray[i].repeat > 1 ? fdarrayptr->fdarray[i].repeat : 1;

            for ( j = 0; j < loop; j ++ )
            {
                switch( fdarrayptr->fdarray[i].type )
                {
                case TYPE_INT:
                    itemp = *(int *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);
                    // 如果不允许显示0
                    if ( ( input_type & SHOW_INT_0 ) != SHOW_INT_0 && itemp == 0 )
                        continue;

                    if (attribshow == 0)
                    {
                        sprintf( ctemp, "<%s>%d</%s>\n", fdarrayptr->fdarray[i].name,
                            itemp, fdarrayptr->fdarray[i].name );
                    }
                    else
                    {
                        sprintf( ctemp, " %s = \"%d\" ", fdarrayptr->fdarray[i].name, itemp );
                    }
                    if(outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    addnewinfo ++;

                    break;
                case TYPE_STRING:
                    ptemp = (char *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);

                    // 如果不允许显示0
                    if ( ( input_type & SHOW_INT_0 ) != SHOW_INT_0 && ptemp[0] == '\0' )
                        continue;

                    if (attribshow == 0)
                    {
                        sprintf( ctemp, "<%s>%s</%s>\n", fdarrayptr->fdarray[i].name,
                            ptemp, fdarrayptr->fdarray[i].name );
                    }
                    else
                    {
                        sprintf( ctemp, " %s = \"%s\" ", fdarrayptr->fdarray[i].name, ptemp );
                    }
                    if (outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    addnewinfo ++;
                    break;

                case TYPE_BOOL:
                    itemp = *(int *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);
                    if (attribshow == 0)
                    {
                        if (itemp)
                        {
                            sprintf( ctemp, "<%s>true</%s>\n", fdarrayptr->fdarray[i].name,
                                fdarrayptr->fdarray[i].name );
                        }
                        else
                        {
                            sprintf( ctemp, "<%s>false</%s>\n", fdarrayptr->fdarray[i].name,
                                fdarrayptr->fdarray[i].name );
                        }
                    }
                    else
                    {
                        if (itemp)
                        {
                            sprintf( ctemp, " %s = \"true\" ", fdarrayptr->fdarray[i].name );
                        }
                        else
                        {
                            sprintf( ctemp, " %s = \"false\" ", fdarrayptr->fdarray[i].name );
                        }
                    }
                    if (outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    addnewinfo ++;

                    break;

                case TYPE_CHAR:
                    itemp = *(int *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);
                    cc = (char)itemp;
                    if (attribshow == 0)
                    {
                        sprintf( ctemp, "<%s>%c</%s>\n", fdarrayptr->fdarray[i].name,
                            cc, fdarrayptr->fdarray[i].name );
                    }
                    else
                    {
                        sprintf( ctemp, " %s = \"%c\" ", fdarrayptr->fdarray[i].name, cc );
                    }

                    if (outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    addnewinfo ++;
                    break;
              case TYPE_IP_ADDR:
                    ip_temp.s_addr = *(unsigned long int *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);

                    if (attribshow == 0)
                    {
                        sprintf( ctemp, "<%s>%s</%s>\n", fdarrayptr->fdarray[i].name,
                            inet_ntoa(ip_temp), fdarrayptr->fdarray[i].name );
                    }
                    else
                    {
                        sprintf( ctemp, " %s = \"%s\" ", fdarrayptr->fdarray[i].name, inet_ntoa(ip_temp) );
                    }
                    if (outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    break;
                    //其它暂不做
                }
            }
        }
        itemp = fdarrayptr->fdarray[i].repeat > 1 ? fdarrayptr->fdarray[i].repeat : 1;
        in_offset += itemp * fdarrayptr->fdarray[i].size;
    }

    if (addnewinfo == 0)        // 如果没有新增内容,则删除刚刚新加的无效内容
    {
        if (outbuf)
            outbuf[outbuf_len] = '\0';
        pf->write_buf_len = old_write_buf_len;
        return 0;
    }
    if ( closed == 0 )
    {
        if ( ( input_type & EASY_CLOSE ) != EASY_CLOSE )        // 简易模式 <A .. />
        {
            if (outbuf)
            {
                strcat( outbuf, " />\r\n" );
            }
            pf->write_buf_len += 5;
        }
        else
        {
            sprintf( ctemp, "></%s>\n", fdarrayptr->inputmsg.name );
            if (outbuf)
            {
                strcat( outbuf, ctemp );
            }
            pf->write_buf_len += strlen(ctemp);
        }
        closed = 2;
    }
    if ( closed == 1 )
    {
        sprintf( ctemp, "</%s>\n", fdarrayptr->inputmsg.name );
        if (outbuf)
        {
            strcat( outbuf, ctemp );
        }
        pf->write_buf_len += strlen(ctemp);
    }
    return 1;
}


/************************************************************************
* 函数名: findxml_string
* 作  者: sx
* 描  述: 将一个结构体转换成XML语句
* 输入参数: char *pstruct,该结构体的指针
*           int struct_index,该结构体的索引
*           __find_sub_strcut_desc fun, 指定的查找结构体描述信息之函数名
* 输出参数: char *outbuf,最后返回的xml字符串
*           outbuf == NULL，表示仅仅希望获取xml字符串的长度
* 返回值: 返回整个xml字符串的所需要长度。即使buf空间不足或buf==NULL,它返回也是 >0的完整xml所需长度
*         这是希望在根本不知道需要多长时，可以先 struct2xml(,NULL,,,)得到xml长度，再分配buf来获取
*         -1: 参数没输好
************************************************************************/
int struct2xml( char *pstruct, char *outbuf,
               int struct_index, __find_sub_strcut_desc fun )
{
    int all_write_len;
    int input_type;
    PARSER_FUN_PARAM pf;
    FDARRAY_PTR * fdarrayptr;

    //FILE *fp;

    all_write_len = 0;
    if (pstruct == NULL || fun == NULL)
    {
        return -1;
    }

    memset( &pf, 0, sizeof(PARSER_FUN_PARAM) );

    pf.fun = (void *)fun;                           // 记住调用函数
    fdarrayptr = find_fdarray( &pf, struct_index, fun );
    if ( fdarrayptr != NULL && fdarrayptr->fdarray != NULL )
    {
        input_type = fdarrayptr->inputmsg.input_type;
    }
    else
    {
        free_pf(&pf);            // 在find_fdarray()里有申请内存,这里要free
        free_fdarray(&pf);  
        return -1;               // 未找到结构体定义
    }

    pf.write_buf_len = 0;
    if ( outbuf )
    {
        outbuf[0] = '\0';            // 清空原来的信息
    }
    // 首先打印头<Message>
    if ( (input_type & HAVE_TITLE) == HAVE_TITLE
        && fdarrayptr->inputmsg.name[0] != '\0' ) // 如果有<Message>这样的头时
    {
        if (outbuf)
        {
            sprintf( outbuf, "<Message>\r\n" );
        }
        pf.write_buf_len += 11;
    }

    // 将此结构体信息打印出xml来
    makexmlstring( &pf, pstruct, outbuf, struct_index, 0 );

    // 最后打印尾</Message>
    if ( (input_type & HAVE_TITLE) == HAVE_TITLE
        && fdarrayptr->inputmsg.name[0] != '\0' ) // 如果有<Message>这样的头时
    {
        if (outbuf)
        {
            strcat( outbuf, "</Message>\r\n" );
        }
        pf.write_buf_len += 12;
    }

    //fp = fopen("c:/1.txt","w");
    //if (fp)
    //{
    //  fprintf(fp, "%s",outbuf);
    //  fclose(fp);
    //}

    all_write_len = pf.write_buf_len;

    free_pf(&pf);
    free_fdarray(&pf);
    return all_write_len;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

