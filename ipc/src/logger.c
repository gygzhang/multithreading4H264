#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>

#include "sm_parser.h"
#include "logger.h"




//static TGAP_LIST_HEAD_STATIC(logmsgs, logmsg);

//static pthread_t logthread = -1;
//static pthread_cond_t logcond;
//static pthread_mutex_t flock;

//static int closelogthread;
//static FILE *flog;
int log_file_select = 0;            // 0 : 未知，1: netcrosser_1.log ; 2: netcrosser_2.log

static char *levels[] = {
	"NOTICE",
	"WARNIN",
	"DEBUGG",
	"ERRORR",
	"VERBOS"
};

char *logger_get_level_name(int level)
{
    switch( level ) {
       case 0:
          return "NOTICE";
       case 1:
          return "WARNIN";
       case 2:
          return "DEBUGG";
       case 3:
          return "ERRORR";
       case 4:
          return "VERBOS";
       default:
          return "UNKNOW";
    }
    return "UNKNOW";
}

#ifdef LOGGER_THREAD
static int logger_get_items_counter( void )
{
    int n= 0;
    struct logmsg *p = NULL;
    TGAP_LIST_LOCK(&logmsgs);
    TGAP_LIST_TRAVERSE(&logmsgs, p, list) 
        n++;
    
    TGAP_LIST_UNLOCK(&logmsgs);
    return n;
}
#endif

#ifdef LOGGER_THREAD
static void *logger_thread(void *data)
{
    struct logmsg *next = NULL, *msg = NULL;
//    FILE *fp = NULL;
//    int counter;

	for (;;) {
		/* We lock the message list, and see if any message exists... if not we wait on the condition to be signalled */
		TGAP_LIST_LOCK(&logmsgs);
		if (TGAP_LIST_EMPTY(&logmsgs))
			pthread_cond_wait(&logcond, &logmsgs.lock);
		next = TGAP_LIST_FIRST(&logmsgs);
		TGAP_LIST_HEAD_INIT_NOLOCK(&logmsgs);
         while((msg = next)) {
            next = TGAP_LIST_NEXT(msg, list);
            if ( flog > 0 ) {
               fprintf(flog, "%s %s # %s \n", msg->timestamp, logger_get_level_name(msg->level), msg->buf);
               fflush(flog);
            }
            fprintf(stdout, "%s %s # %s \n", msg->timestamp, logger_get_level_name(msg->level), msg->buf);
            fflush(stdout);
            free(msg);
         }
       
		/* If we should stop, then stop */
		if (closelogthread)
			break;


	}

	return NULL;
}

#endif





void close_logger()
{
   /*
    TGAP_LIST_LOCK(&logmsgs);
    closelogthread = 1;
    pthread_cond_signal(&logcond);
    TGAP_LIST_UNLOCK(&logmsgs);
    if ( logthread != -1 ) 
        pthread_join(logthread, NULL);
    TGAP_LIST_HEAD_DESTROY(&logmsgs);  //删除lists of nat_logchannels
    pthread_cond_destroy(&logcond);

    pthread_mutex_destroy(&flock);

    logthread = -1;
    if ( flog > 0 )
        fclose(flog);
    */
    return;
}


int open_loger(int port)
{
   /*
    pthread_mutex_init(&flock, NULL);
    closelogthread = 0;
    pthread_cond_init(&logcond, NULL);
    flog = fopen(NETCROSSER_LOGFILE, "a+");
    if ( NULL == flog )
        return -1;

    TGAP_LIST_HEAD_INIT(&logmsgs);
    if ( pthread_create(&logthread, NULL, logger_thread, NULL)) {
        pthread_cond_destroy(&logcond);
        pthread_mutex_destroy(&flock);
        return -1;
    }
    */
    return 0;
}

// 日志文件检查，够用返0
int logger_size_check(char *logfile, int len)
{
    int file_size;
    
    file_size = logger_size_get(logfile);
    if(-1 == file_size)
        return 0;

    if(file_size + len < (MAX_LEN_XML/2))
        return 0;

    return -1;    
}

// 返回日志文件大小
int logger_size_get(char *logfile)
{
    struct stat filestate;
    if (stat(logfile, &filestate)) {
        return -1;
    }

    return filestate.st_size;
}

int logger_size(char *logfile)
{
    struct stat filestate;
    if (stat(logfile, &filestate)) {
        return -1;
    }
    if (filestate.st_size > MAX_LOG_BUFFER) {
        return 0;
    }
    return -1;
}

void logger1(int level, const char *fmt, ...)
{
    return ;
#if 0
    int s;
    
    va_list vs;
    char buf[MAX_PACKET_LEN] = "";
    char timestamp[TIMESTAMP_LEN] = "";
    //memset(buf, 0, MAX_PACKET_LEN);
    struct logmsg *lmsg = NULL;
    struct tm t;
    time_t tt;
    tt = time(NULL);
    localtime_r(&tt, &t);
  
    if ( (s = logger_size()) == 0 ) {
        fprintf(stdout, "Reload logger!\n");
        fflush(stdout);
        if ( flog > 0 ) {
            fclose(flog);
            flog = NULL;
        }
        unlink(NETCROSSER_LOGFILE_1);
        usleep(20);
        flog = fopen(NETCROSSER_LOGFILE_1, "a");
        if ( NULL == flog )
            return ;
    }

    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d ", 
                        t.tm_year+1900,
                        t.tm_mon + 1,
                        t.tm_mday,
                        t.tm_hour, 
                        t.tm_min,
                        t.tm_sec);
    
    va_start(vs, fmt);
    vsprintf(buf, fmt, vs);
    lmsg = (struct logmsg *)calloc(1, sizeof(struct logmsg));
    if (lmsg == NULL)
        return;
    //memset(&lmsg, 0, sizeof(struct logmsg));
    lmsg->level = level;
    strcpy(lmsg->timestamp, timestamp);
    strcpy(lmsg->buf, buf);


    if (logthread != -1 ) {
        TGAP_LIST_LOCK(&logmsgs);
        TGAP_LIST_INSERT_TAIL(&logmsgs, lmsg, list);
        pthread_cond_signal(&logcond);
        TGAP_LIST_UNLOCK(&logmsgs);
    } else {
     free(lmsg);
    }
    va_end(vs);

#endif

}


void logger(int level, const char *fmt, ...)
{
    
    va_list vs;
    int ret;
    char buf[MAX_LOG_LINE];
    char timestamp[TIMESTAMP_LEN];
    memset(buf, 0, MAX_LOG_LINE);
    struct tm t;
    time_t tt;
    tt = time(NULL);
    localtime_r(&tt, &t);
    char logbuf[MAX_LOG_LINE*2];
    FILE *fp = NULL;
    int logbuf_len;    
    // 当前日志文件的剩余空间

    // 构造写入字符串
    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d ", t.tm_year+1900,
            t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
            t.tm_sec);  
    va_start(vs, fmt);
    vsprintf(buf, fmt, vs);
    va_end(vs);
    switch(level) {
      case 0:  
        sprintf(logbuf, "%s %s # %s \n", timestamp, levels[level], buf);
        break;
      case 1:  
        sprintf(logbuf, "%s %s # %s \n", timestamp, levels[level], buf);
        break;
      case 2:  
        sprintf(logbuf, "%s %s # %s \n", timestamp, levels[level], buf);
        break;
      case 3:  
        sprintf(logbuf, "%s %s # %s \n", timestamp, levels[level], buf);
        break;
      default:
        sprintf(logbuf, "%s %s # %s \n", timestamp, levels[level], buf);
        break;
    }
    logbuf_len = strlen(logbuf);

    /* 写日志
       1. 写入标记为0, 检查log_1空间，够则写入log_1,写入标记为1；
                                    不够则检查log_2,够写则写入log_2, 写入标记为2
                                                  不够则删除log_1,写入log_1, 写入标记为1
       2. 写入标记为1, 检查log_1空间，够则写入log_1,写入标记为1；不够则删除log_2,写入log_2, 写入标记为2
       3. 写入标记为2, 检查log_2空间，够则写入log_2,写入标记为2；不够则删除log_1,写入log_1, 写入标记为1
    */    
    if(0 == log_file_select)
    {
        ret = logger_size_check(NETCROSSER_LOGFILE_1, logbuf_len);
        if(0 == ret)
        {
            fp = fopen(NETCROSSER_LOGFILE_1, "a+");
            if(NULL == fp)
                return;
                
            log_file_select = 1;
        }
        else
        {
            ret = logger_size_check(NETCROSSER_LOGFILE_2, logbuf_len);
            if(0 == ret)
            {
                fp = fopen(NETCROSSER_LOGFILE_2, "a+");
                if(NULL == fp)
                    return;
                    
                log_file_select = 2;
            }
            else
            {
                unlink(NETCROSSER_LOGFILE_1);
                fp = fopen(NETCROSSER_LOGFILE_1, "a+");
                if(NULL == fp)
                    return;
                    
                log_file_select = 1;

            }
        }

        goto write_log;
    }

    if(1 == log_file_select)
    {
       ret = logger_size_check(NETCROSSER_LOGFILE_1, logbuf_len);
       if(0 == ret)
       {
           fp = fopen(NETCROSSER_LOGFILE_1, "a+");
           if(NULL == fp)
               return;
               
           log_file_select = 1;
       }
       else
       {
           unlink(NETCROSSER_LOGFILE_2);
           fp = fopen(NETCROSSER_LOGFILE_2, "a+");
           if(NULL == fp)
               return;
               
           log_file_select = 2;
       }

       goto write_log;
    }

    if(2 == log_file_select)
    {
        ret = logger_size_check(NETCROSSER_LOGFILE_2, logbuf_len);
        if(0 == ret)
        {
            fp = fopen(NETCROSSER_LOGFILE_2, "a+");
            if(NULL == fp)
                return;
                
            log_file_select = 2;
        }
        else
        {
            unlink(NETCROSSER_LOGFILE_1);
            fp = fopen(NETCROSSER_LOGFILE_1, "a+");
            if(NULL == fp)
                return;
                
            log_file_select = 1;
        }

        goto write_log;
    }

write_log:
    // 写日志
    fprintf(fp, "%s", logbuf);
//    fwrite(buf, 1, buf_len, fp);
    fflush(fp);
    fclose(fp);
        
    

    return;
}


int logger_printf(char *buf, int buf_len)
{
    FILE *fp;

    fp = fopen("/usrdata/tmp.log", "w+");
    fwrite(buf, 1, buf_len, fp);
    fclose(fp);

    return 0;

}

