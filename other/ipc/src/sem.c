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

#include "sem.h"

extern s_syn_ctrl syn_ctrl;

int thread_syn_init(pthread_mutex_t *mutex, pthread_cond_t *cond, int *flag)
{
    pthread_mutex_init(mutex, NULL);
    pthread_cond_init(cond, NULL);
    *flag = 0;
    
    return 0;
}

int thread_syn_flag_set(pthread_mutex_t *mutex, pthread_cond_t *cond, int *flag)
{
    pthread_mutex_lock(mutex);
    *flag = 1;
    pthread_cond_signal(cond);
    pthread_mutex_unlock(mutex);
    return 0;
}

int thread_syn_flag_clr(pthread_mutex_t *mutex, pthread_cond_t *cond, int *flag)
{
    pthread_mutex_lock(mutex);
    *flag = 0;
    pthread_cond_signal(cond);
    pthread_mutex_unlock(mutex);
    return 0;
}

int thread_syn_wait(pthread_mutex_t *mutex, pthread_cond_t *cond, int *flag)
{
    pthread_mutex_lock(mutex);
    while(!flag)
        pthread_cond_wait(cond,mutex);
    pthread_mutex_unlock(mutex);
        
    return 0;
}


//创建信号量 
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
void sem_del(int semid) 
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


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */




