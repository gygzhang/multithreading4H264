
#ifndef __SEM_H__
#define __SEM_H__

typedef struct tag_syn_ctrl
{
    pthread_mutex_t     mutex; 
    pthread_cond_t      cond;
    int                 flag;
}s_syn_ctrl;

#endif