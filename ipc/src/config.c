#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdlib.h>         /*  for malloc() */
#include <string.h>         /*  for memcpy() */
#include <dirent.h>         /* for opendir() */
#include <sys/vfs.h>        /* for statfs() */
#include <stdio.h>          /* for sprintf() */

#include <sys/stat.h>        /* for stat() */
#include <unistd.h>
#include <sys/types.h>       /* for mkdir() */

#include "vs_main_rountin.h"
#include "vs_timer.h"
#include "vs_config.h"


typedef struct tagConfig_timer_param
{
    int         timer_id_detect;            // 定时移动侦测的定时器编号
    int         timer_id_cap;               // 定时抓拍的定时器编号
}Config_timer_param;
Config_timer_param g_conf_timer;

// sx added auto
#include "vs_config_put_var_auto.h"
// sx added auto end 


// 将weekplan中的数据拷贝到schedule中
int vs_config_weekplan2schedule(int camera_index)
{
    switch (camera_index)
    {
        case 0:
            memcpy(g_config.para_alarm_camera_0.schedule_0.var, g_config.alarm_task.week_plan.var + 24 * 0, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_1.var, g_config.alarm_task.week_plan.var + 24 * 1, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_2.var, g_config.alarm_task.week_plan.var + 24 * 2, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_3.var, g_config.alarm_task.week_plan.var + 24 * 3, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_4.var, g_config.alarm_task.week_plan.var + 24 * 4, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_5.var, g_config.alarm_task.week_plan.var + 24 * 5, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_6.var, g_config.alarm_task.week_plan.var + 24 * 6, 24);
            break;
        case 1:
            memcpy(g_config.para_alarm_camera_1.schedule_0.var, g_config.alarm_task.week_plan.var + 24 * 0, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_1.var, g_config.alarm_task.week_plan.var + 24 * 1, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_2.var, g_config.alarm_task.week_plan.var + 24 * 2, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_3.var, g_config.alarm_task.week_plan.var + 24 * 3, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_4.var, g_config.alarm_task.week_plan.var + 24 * 4, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_5.var, g_config.alarm_task.week_plan.var + 24 * 5, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_6.var, g_config.alarm_task.week_plan.var + 24 * 6, 24);
            break;    
        case 2:
            memcpy(g_config.para_alarm_camera_2.schedule_0.var, g_config.alarm_task.week_plan.var + 24 * 0, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_1.var, g_config.alarm_task.week_plan.var + 24 * 1, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_2.var, g_config.alarm_task.week_plan.var + 24 * 2, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_3.var, g_config.alarm_task.week_plan.var + 24 * 3, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_4.var, g_config.alarm_task.week_plan.var + 24 * 4, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_5.var, g_config.alarm_task.week_plan.var + 24 * 5, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_6.var, g_config.alarm_task.week_plan.var + 24 * 6, 24);
            break;
        case 3:
            memcpy(g_config.para_alarm_camera_3.schedule_0.var, g_config.alarm_task.week_plan.var + 24 * 0, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_1.var, g_config.alarm_task.week_plan.var + 24 * 1, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_2.var, g_config.alarm_task.week_plan.var + 24 * 2, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_3.var, g_config.alarm_task.week_plan.var + 24 * 3, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_4.var, g_config.alarm_task.week_plan.var + 24 * 4, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_5.var, g_config.alarm_task.week_plan.var + 24 * 5, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_6.var, g_config.alarm_task.week_plan.var + 24 * 6, 24);
            break;
    }

    return 0;
}

// 将schedule中的数据拷贝到weekplan中
int vs_config_schedule2weekplan()
{
    memcpy(g_config.alarm_task.week_plan.var + 24 * 0, g_config.para_alarm_camera_0.schedule_0.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 1, g_config.para_alarm_camera_0.schedule_1.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 2, g_config.para_alarm_camera_0.schedule_2.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 3, g_config.para_alarm_camera_0.schedule_3.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 4, g_config.para_alarm_camera_0.schedule_4.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 5, g_config.para_alarm_camera_0.schedule_5.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 6, g_config.para_alarm_camera_0.schedule_6.var, 24);

    return 0;
}

// 读取配置文件
int config_read(char *file_name)
{
    FILE *fp;
    char line[512];
    char name[48];      // 前面的名字，规定最长不超过20
    char var[256];      // 后面的数值
    char note[256];      // 注释文字
    
    char c;
    int start, pos;
	int flag_4_close = 0;       // var部分是否含有"号
	int flag_1_box = 0;
	int flag_1_box_end = 0;
    int flag = 0;
    char *ret = line;

    memset(&g_config, 0, sizeof(AllConfig));
    
    fp = fopen(file_name, "r");
    if ( fp == NULL )
    {
        return -1;
    }
    
    while( !feof(fp) )
    {
        ret = fgets(line, 512, fp);
        if ( ret == NULL )
        {
            break;
        }

        // 进行line的解析, 用=号分隔. 懒得用strstr, 自己来解析. 因为还希望trim,去除"号等等。使用strstr麻烦。

		// 解析的效果： name部分，不允许出现=号
		//              var部分，可以含有=号。
		//              所有空格都忽略。即含有trim功能。var部分，如果希望含有空格或"号，需要用"号括起来(主要是前面部分)
		//              当出现[]时,效果就是name[0]=='\0', 而var赋值为[]中的内部,var是经过trim的。
		// 允许的情况有：
		//aaa=bbbb
		//  aaa   =    bbb
        //  aaa   == bbb
        //  aaa   ===  bb===bb
        //  aaa  ======= "  b ==  b   b  "
        //[abc]
        //  [  abc  ]
        //  [  a  b  c ]

        // 而不恰当的如 aaa = bb bb    就只相当于 aaa = bb了，因为 bb bb 中间有空格，却没有"号包括起来
        
		start = 0;
		flag = 0;
		pos = 0;
		name[0] = '\0';
		var[0] = '\0';
		note[0] = '\0';
		flag_4_close = 0;
		flag_1_box = 0;
		flag_1_box_end = 0;
		c = line[0];

		// |_| AAA |_| = |_| BBB |_|
		// 0   1  2    3     4  5     flag
		while( c != '\0' )
		{
			c = line[pos];
			if ( (( c == ' ' || c == '\t') && flag_4_close == 0 ) || c == '\0' || c == '\n' || c == '\r' || (c == '=' && flag <= 3) || (c == '/' && line[pos+1] == '/') )
			{
				if ( flag == 0 || flag == 3 )
				{
					start = pos;        // 碰到空格，起始段向前走
				}
				if ( flag == 1 )        // 结束
				{
					memcpy( name, line+start, pos-start );
					name[pos-start] = '\0';
					flag ++;
				}
				if ( flag == 4 )
				{
					if (flag_1_box == 1)
					{
						if (start > 0)
						{
							memcpy( var, line+start, flag_1_box_end-start+1);
							var[flag_1_box_end-start+1] = '\0';
						}
						if ( flag_1_box_end-start >= 0 && var[flag_1_box_end-start] == '"' && flag_4_close == 1 )
						{
							var[flag_1_box_end-start] = '\0';  // 去掉 " 号
						}
						break;
					}
					if ( line[start-1] == '/' && line[start] == '/' )       //  a=//ccc 的情况。var是空
					{
						strcpy(note, line+start-1);
						break;
					}
					memcpy( var, line+start, pos-start );
					var[pos-start] = '\0';
					flag ++;
					
					if ( pos-start-1 >= 0 && var[pos-start-1] == '"' && flag_4_close == 1 )
					{
						var[pos-start-1] = '\0';  // 去掉 " 号
					}
					break;
				}
				if ( c == '=' && ( flag == 2 || flag == 5) )
				{
					flag = 3;
				}
			}
			else
			{
				if ( flag_1_box == 1 && c != ' ' && c != '\t' && c != ']' )
				{
					if (start < 0)
					{
						start = pos;
					}
					flag_1_box_end = pos;
				}
				if ( flag == 0 || flag == 3 )
				{
					flag ++;
					start = pos;

					if ( flag == 1 && c == '[' )
					{
						flag = 4;
						flag_1_box = 1;	// 中括号方式
						flag_4_close = 1;		// 含有"号
						start = -1;
					}
					if ( flag == 4 && c == '"' )
					{
						flag_4_close = 1;		// 含有"号
						start ++;
						flag_1_box = 1;
					}
				}
				if ( flag == 4 && flag_1_box == 1 && c == ']' )
				{
					if (start > 0)
					{
						memcpy( var, line+start, flag_1_box_end-start+1);
						var[flag_1_box_end-start+1] = '\0';
					}
					break;
				}
			}
			pos ++;
		}

		// 从pos开始，查找note注释文字
		if (note[0] =='\0')
		{
			while (line[pos] != '\0')
			{
				if ( line[pos] == '/' && line[pos+1] == '/' )
				{
					strcpy(note, line+pos);
					/*
					int note_len = strlen(note);
					if ( note[note_len-1] == '\n' )
					    note[note_len-1] = '\0';
					if ( note[note_len -2] == '\r' )
					    note[note_len -2] = '\0';
					*/
					break;
				}
				pos ++;
			}
		}
		if (name[0] == '\r' || name[0] == '\n')
		{
			continue;
		}
        // 进行全局变量的赋值
        vs_config_put_var(name, var, note);
        
    }

       
    return 0;
}

// 写一行带[]的信息
int vs_config_box_line(FILE *fp, char *line, char *name, char *note)
{
    int max_first_len = 48;     // 前面最长48个字符
    int len = 0;

    //fprintf(fp, "\r\n");
    
    sprintf(line, "[%s]", name);
    if ( note[0] != '\0' )
    {
        len = strlen(line);     // 计划前面的长度，追加一定的空格，对齐
        if (len < max_first_len)
        {
            memset(line+len, ' ', max_first_len - len);
            line[max_first_len] = '\0';
        }
        strcat(line, note);  // 将注释追加
    }
    fprintf(fp, "%s\r\n", line);
    return 0;
}

// 写一项具体的数值. name = var   note(//...)
int vs_config_var_line(FILE *fp, char *line, char *name, char *var, char *note)
{
    int max_first_len = 48;     // 前面最长48个字符
    int len = 0;

    if ( name[0] == '\0' )
    {
        return 0;
    }
    if ( var[0] == '\0' )
    {
        sprintf(line, "  %s = ", name);
    }
    else
    {
        sprintf(line, "  %s = \"%s\"", name, var);
    }
    if ( note[0] != '\0' )
    {
        len = strlen(line);     // 计划前面的长度，追加一定的空格，对齐
        if (len < max_first_len)
        {
            memset(line+len, ' ', max_first_len - len);
            line[max_first_len] = '\0';
        }
        if ( note[0] == '\0')
        {
            strcat(line, "\n");
        }
        else
        {
            strcat(line, note);  // 将注释追加
            if (note[strlen(note)-1] != '\n' )
            {
                strcat(line, "\n");
            }
        }
    }
    fprintf(fp, "%s\r\n", line);
    return 0;
}

// 写一项具体的数值. name = var   note(//...)
int vs_config_var_line2(FILE *fp, char *line, char *name, long var, char *note)
{
    int max_first_len = 48;     // 前面最长48个字符
    int len = 0;

    if ( name[0] == '\0' )
    {
        return 0;
    }

    sprintf(line, "  %s = %ld", name, var);
    if ( note[0] != '\0' )
    {
        len = strlen(line);     // 计划前面的长度，追加一定的空格，对齐
        if (len < max_first_len)
        {
            memset(line+len, ' ', max_first_len - len);
            line[max_first_len] = '\0';
        }
        if ( note[0] == '\0')
        {
            strcat(line, "\n");
        }
        else
        {
            strcat(line, note);  // 将注释追加
            if (note[strlen(note)-1] != '\n' )
            {
                strcat(line, "\n");
            }
        }
    }
    fprintf(fp, "%s\r\n", line);
    return 0;
}


// 下面，全部都是定时器任务。首先从全局变量中分析出定时器任务。再加定时
int vs_config_get_curtime(struct tm *pout)
{
    time_t curtime;
    struct tm *p;
    curtime = vs_get_center_time();         // 得到中心时间
    p = gmtime( &curtime );
    memcpy(pout, p, sizeof(struct tm));
    return 0;
}

// 整点的定时回调
int vs_config_hour_callback(void *data)
{
    struct tm t;
    char *pvar = NULL;
    int i;
    int index;
    
    vs_config_get_curtime(&t);

    if (t.tm_wday == 6) // 星期日
    {
        index = 0;      // 我们协议的布防时间，是从星期日开始
    }
    else
    {
        index = (t.tm_wday +1) * 24;     // 0表示星期一
    }
    index += t.tm_hour;     // 在布防时间里的当前时刻。

    // 报警的布防时间...
    // target... 现在还没做起动报警的程序，以后再加。
    
    // 判断当前是否该启动任务。
    for ( i = 0; i < _MAX_CAMERA; i ++ )
    {
/*        pvar = g_config.week_time.week_alarm_detect_time.var;       // 这是报警之移动侦测布防时间
        if ( strlen(pvar) > index )            // 开始启动定时移动侦测
        {
            if ( pvar[index] > '0' )
            {
                //这里应该王伟的函数增加 target_data数据在此
                vs_vm_set_camera_target(i, RTV_TIME_DETECT, NULL, 1);        // 在这里启动定时移动侦测任务。
            }
            else
            {
                //取消定时移动侦测
                vs_vm_set_camera_target(i, RTV_REMOVE_TIME_DETECT, NULL, 1); // 取消定时移动侦测任务
            }
        }
*/
    }
    // 最后，再次追加下一整点的定时器任务
    vs_config_detect_add_timer();
    return 0;
}
// 移动侦测的追加定时器任务
int vs_config_detect_add_timer()
{
    struct tm t;
    int next_time;

    if (g_conf_timer.timer_id_detect >= 0)
    {
        // 删除原来的定时器
        //vs_timer_chain_node_del_by_timerid(g_conf_timer.timer_id_detect);
    }
    vs_config_get_curtime(&t);

    // 由于定时为每整点小时做一次。此处做判断
    next_time = 3600 - ( t.tm_min * 60 + t.tm_sec );

    // 然后再追加
    //vs_timer_add2(next_time, 1, &g_timer_id, vs_config_hour_callback, NULL);
    //vs_timer_add2(15, 1, &g_timer_id, vs_config_hour_callback, NULL);
    printf("启动整点定时器：15秒之后开始执行(为了测试，改成的15秒!)\n");
    return 0;
}

// 根据全局参数，创建一定的定时器任务
int vs_config_init_timer()
{
    memset(&g_conf_timer, 0, sizeof(Config_timer_param));
    g_conf_timer.timer_id_detect = -1;
    g_conf_timer.timer_id_cap = -1;

    // 我看vs_timer.h中，最多不超过30个定时器结点，也不知道是否会超出。
    
    // 追加移动侦测的定时器任务
    vs_config_detect_add_timer();
    return 0;
}

// sx added auto
#include "vs_config_write_auto.h"
// sx added auto en

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

