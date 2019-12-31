#include"common.h"
#include"misc.h"

void display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
    int s, i;
    size_t v;
    void *stkaddr;
    struct sched_param sp;
    /*
    * Create a detached thread when you know you won't want to wait for 
    * it with pthread_join(). The only performance benefit is that when 
    * a detached thread terminates, its resources can be released immediately 
    * instead of having to wait for the thread to be joined before the 
    * resources can be released.
    * 
    * It is 'legal' not to join a joinable thread; but it is not usually 
    * advisable because (as previously noted) the resources won't be released 
    * until the thread is joined, so they'll remain tied up indefinitely (until 
    * the program exits) if you don't join it.
    */


    s = pthread_attr_getdetachstate(attr, &i);
    if (s != 0)
        handle_error_en(s, "pthread_attr_getdetachstate");
    printf("%sDetach state        = %s\n", prefix,
            (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
            (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
            "???");

    /** 
    * PTHREAD_SCOPE_SYSTEM
    * The thread competes for resources with all other threads in
    *  all processes on the system that are in the same scheduling
    *  allocation domain (a group of one or more processors). 
    * PTHREAD_SCOPE_SYSTEM threads are scheduled relative to one 
    * another according to their scheduling policy and priority.
    * 
    * PTHREAD_SCOPE_PROCESS
    * The thread competes for resources with all other threads in 
    * the same process that were also created with the PTHREAD_SCOPE_PROCESS 
    * contention scope. PTHREAD_SCOPE_PROCESS threads are scheduled relative 
    * to other threads in the process according to their scheduling policy 
    * and priority. POSIX.1-2001 leaves it unspecified how these threads contend 
    * with other threads in other process on the system or with other threads 
    * in the same process that were created with the PTHREAD_SCOPE_SYSTEM 
    * contention scope.
    * 
    * POSIX.1-2001 only requires that an implementation support one of these 
    * contention scopes, but permits both to be supported. Linux supports 
    * PTHREAD_SCOPE_SYSTEM, but not PTHREAD_SCOPE_PROCESS. 
    */

    s = pthread_attr_getscope(attr, &i);
    if (s != 0)
        handle_error_en(s, "pthread_attr_getscope");
    printf("%sScope               = %s\n", prefix,
            (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :
            (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :
            "???");

    /*
    * PTHREAD_INHERIT_SCHED
    *          Threads that are created using attr inherit scheduling
    *          attributes from the creating thread; the scheduling attributes
    *          in attr are ignored.
    * 
    *   PTHREAD_EXPLICIT_SCHED
    *            Threads that are created using attr take their scheduling
    *          attributes from the values specified by the attributes object.
    * 
    *   The default setting of the inherit-scheduler attribute in a newly
    *   initialized thread attributes object is PTHREAD_INHERIT_SCHED.
    */

    s = pthread_attr_getinheritsched(attr, &i);
    if (s != 0)
        handle_error_en(s, "pthread_attr_getinheritsched");
    printf("%sInherit scheduler   = %s\n", prefix,
            (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :
            (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :
            "???");

    /*
    * SCHED_OTHER: Default Linux time-sharing scheduling
    * 
    * SCHED_OTHER can only be used at static priority 0. SCHED_OTHER 
    * is the standard Linux time-sharing scheduler that is intended 
    * for all processes that do not require the special real-time mechanisms.
    * The process to run is chosen from the static priority 0 list based on a 
    * dynamic priority that is determined only inside this list. The dynamic 
    * priority is based on the nice value (set by nice(2) or setpriority(2)) 
    * and increased for each time quantum the process is ready to run, but 
    * denied to run by the scheduler. This ensures fair progress among all 
    * SCHED_OTHER processes.
    */

    s = pthread_attr_getschedpolicy(attr, &i);
    if (s != 0)
        handle_error_en(s, "pthread_attr_getschedpolicy");

    printf("%sScheduling policy   = %s\n", prefix,
            (i == SCHED_OTHER) ? "SCHED_OTHER" :
            (i == SCHED_FIFO)  ? "SCHED_FIFO" :
            (i == SCHED_RR)    ? "SCHED_RR" :
            "???");

    s = pthread_attr_getschedparam(attr, &sp);
    if (s != 0)
        handle_error_en(s, "pthread_attr_getschedparam");

    /*
     * For processes scheduled under one of the normal scheduling 
     * policies (SCHED_OTHER, SCHED_IDLE, SCHED_BATCH), sched_priority
     * is not used in scheduling decisions (it must be specified as 0).
     * 
     * Processes scheduled under one of the real-time policies (SCHED_FIFO,
     *  SCHED_RR) have a sched_priority value in the range 1 (low) to 
     * 99 (high). (As the numbers imply, real-time processes always have 
     * higher priority than normal processes.)
     * 
     * 
     */
    printf("%sScheduling priority = %d\n", prefix, sp.sched_priority);

    s = pthread_attr_getguardsize(attr, &v);
    if (s != 0)
        handle_error_en(s, "pthread_attr_getguardsize");
    printf("%sGuard size          = %zu bytes\n", prefix, v);

    s = pthread_attr_getstack(attr, &stkaddr, &v);
    if (s != 0)
        handle_error_en(s, "pthread_attr_getstack");
    printf("%sStack address       = %p\n", prefix, stkaddr);
    printf("%sStack size          = 0x%zx bytes\n", prefix, v);
}

void signal_handler(int signo)   //信号处理函数
{
	printf("\nplease input exit to quit\n");
}