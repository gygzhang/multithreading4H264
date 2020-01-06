#include"extBB.h"
#include"common.h"


extern void signal_handler(int signo);
//extern frame_list *tmp_list;
void print_bytes(void *p, size_t len)
{
    size_t i;
    printf("(");
    for (i = 0; i < len; ++i)
        printf("%02X", ((uchar*)p)[i]);
    printf(")");
}


void ex_endian(void* p,size_t len){
    int i;
    uint32 tmep;
    uchar temp1;
    if(len%2!=0) return;
    for(i=0;i<len/2;i++){
        temp1 = ((uchar*)p)[i];
        ((uchar*)p)[i] = ((uchar*)p)[len-i-1];
        ((uchar*)p)[len-i-1] = temp1;
    }
}

uint32 ibp_type(uchar type){
    return type&~0xfc;
}

uint32 is_fill(uchar type){
    return (type&~0x7f)==128?1:0;
}

uint32 data_len(uint32 len){
    return len;
}

uint32 list_is_full(){
    pthread_mutex_lock(&m);
    int result = (get == (put + 1)%size);
    pthread_mutex_unlock(&m);
    return result;
}

uint32 list_is_empty(){
    pthread_mutex_lock(&m);
    int result = (get == put);
    pthread_mutex_unlock(&m);
    return result;

}

void *producer_carray(void *para){

    //FILE* fh264 = fopen("./BB.h264","wb");
    fin = 0;
    package_head phead;
    uint32 ihead,readbytes,len;
    unsigned short temp1,temp2;
    uchar *data;
    while(fread(&temp1,sizeof(short),1,fbin)==1){
        if(temp1==0x0000){
            if(fread(&temp2,sizeof(short),1,fbin)!=1) return;
            ex_endian((void*)&temp2,2);
            if(temp2==0x00fa){
                fseek(fbin,-4,SEEK_CUR);
                fread(&phead,sizeof(package_head),1,fbin);
                fseek(fbin,-2,SEEK_CUR);
                ex_endian((void*)&(phead.stream),4);

                len = data_len(phead.len);
                data = (uchar*)malloc(sizeof(uchar)*len);
                fread(data,sizeof(uchar)*len,1,fbin);
                //while(list_is_full());
                sem_wait(&s_empty);

                // tmp->data = data;
                // tmp->len = len;

                frams[put].len = len;
                frams[put].data = data;
                put = (put + 1) % size;
                count++;
                sem_post(&s_full);
                free(data);
                //LOG(0,"get: %d  put: %d count: %d\n",get,put,count);
            }
        }

    }
    fin = 1;
    return ;
}




void *consumer_carray(void *para){
    pthread_t id = pthread_self(); 
    while(!fin){

        //while(list_is_empty());
        //avoid comsumer endless waiting
        if(count<=0) continue;
        //LOG(12345,"%x\n",id%0xfffffffff);

        sem_wait(&s_full);
        //LOG(54321,"%x\n",id%0xfffffffff);
        pthread_mutex_lock(&m);       
        fwrite((void*)frams[get].data,frams[get].len,1,fh264);
        //LOG(id,"get:%d put:%d count: %d\n",get,put,count);
        get = (get + 1) % size;
        count--;
        pthread_mutex_unlock(&m);
        sem_post(&s_empty);
    }
    LOG(333,"\n\nconsumer %x finish!\n\n",id%0xfffffffff);

    return ;
}

void *producer_list(void *para){

    //FILE* fh264 = fopen("./BB.h264","wb");
    fin = 0;
    package_head phead;
    uint32 ihead,readbytes,len;
    unsigned short temp1,temp2;
    uchar *data;
    while(fread(&temp1,sizeof(short),1,fbin)==1){
        if(temp1==0x0000){
            if(fread(&temp2,sizeof(short),1,fbin)!=1) return;
            ex_endian((void*)&temp2,2);
            if(temp2==0x00fa){
                fseek(fbin,-4,SEEK_CUR);
                fread(&phead,sizeof(package_head),1,fbin);
                fseek(fbin,-2,SEEK_CUR);
                ex_endian((void*)&(phead.stream),4);

                len = data_len(phead.len);
                data = (uchar*)malloc(sizeof(uchar)*len);
                fread(data,sizeof(uchar)*len,1,fbin);
                //while(list_is_full());
                sem_wait(&s_empty);

                tmp_list = (frame_list*)malloc(sizeof(frame_list));

                tmp_list->fram.data = data;
                tmp_list->fram.len = len;
                printf("--%d\n",len+sizeof(len)+sizeof(struct list_head));

                send(clintConnt , (char*)data, len , 0 );



                list_add_tail(&(tmp_list->list),&(frame_head));
                //LOG(222,"data: %d\n",data);
                frams[put].len = len;
                frams[put].data = data;
                put = (put + 1) % size;
                count++;
                sem_post(&s_full);
                //free(data);
                //LOG(0,"get: %d  put: %d count: %d\n",get,put,count);
            }
            temp2=0;
        }
        temp1=0;

    }
    fin = 1;
    return ;
}


void *consumer_list(void *para){
    pthread_t id = pthread_self(); 
    while(!fin){

        //while(list_is_empty());
        //avoid comsumer endless waiting
        if(count<=0) continue;
        //LOG(12345,"%x\n",id%0xfffffffff);

        sem_wait(&s_full);
        //LOG(54321,"%x\n",id%0xfffffffff);
        pthread_mutex_lock(&m);       
        fwrite((void*)frams[get].data,frams[get].len,1,fh264);
        //LOG(id,"get:%d put:%d count: %d\n",get,put,count);
        get = (get + 1) % size;
        count--;
        pthread_mutex_unlock(&m);
        sem_post(&s_empty);
    }
    LOG(333,"\n\nconsumer %x finish!\n\n",id%0xfffffffff);

    return ;
}

void tcp_ready(){
    //clintConnt = accept(clintListn, (struct sockaddr*)NULL, NULL);

    /**
     *  sockfd: socket descriptor, an integer (like a file-handle)
     *   domain: integer, communication domain e.g., AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol)
     *   type: communication type
     *   SOCK_STREAM: TCP(reliable, connection oriented)
     *   SOCK_DGRAM: UDP(unreliable, connectionless)
     *   protocol: Protocol value for Internet Protocol(IP), which is 0. 
     *   This is the same number which appears on protocol field in the 
     *   IP header of a packet.(man protocols for more details)
     * 
     */

    clintListn = socket(AF_INET, SOCK_STREAM, 0); // creating socket
    // memset(&ipOfServer, '0', sizeof(ipOfServer));
    // memset(dataSending, '0', sizeof(dataSending));
    ipOfServer.sin_family = AF_INET;
    ipOfServer.sin_addr.s_addr = htonl(INADDR_ANY);
    ipOfServer.sin_port = htons(2020); 		// this is the port number of running server

    /*
     * After creation of the socket, bind function binds the socket to the 
     * address and port number specified in addr(custom data structure). 
     * In the example code, we bind the server to the localhost, hence we 
     * use INADDR_ANY to specify the IP address.
     */
    bind(clintListn, (struct sockaddr*)&ipOfServer , sizeof(ipOfServer));

    /*
     * It puts the server socket in a passive mode, where it waits for 
     * the client to approach the server to make a connection. The backlog, 
     * defines the maximum length to which the queue of pending connections 
     * for sockfd may grow. If a connection request arrives when the queue 
     * is full, the client may receive an error with an indication of ECONNREFUSED.
     */
    listen(clintListn , 20);
    /*
     * It extracts the first connection request on the queue of pending 
     * connections for the listening socket, sockfd, creates a new 
     * connected socket, and returns a new file descriptor referring 
     * to that socket. At this point, connection is established between 
     * client and server, and they are ready to transfer data.
     */
    clintConnt = accept(clintListn, (struct sockaddr*)NULL, NULL);


    // if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    // { 
    //     printf("\n Socket creation error \n"); 
    //     return -1; 
    // } 

    // serv_addr.sin_family = AF_INET; 
    // serv_addr.sin_port = htons(PORT); 

    // // Convert IPv4 and IPv6 addresses from text to binary form 
    // if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    // { 
    //     printf("\nInvalid address/ Address not supported \n"); 
    //     return -1; 
    // } 

    // if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    // { 
    //     printf("\nConnection Failed \n"); 
    //     return -1; 
    // } 




}

pthread_attr_t attr;
pthread_attr_t *attrp;
int s;
int i,j;
void thread_init(){
    int stack_size;
    void *sp;



    //struct list_head frame_head;

    attrp = &attr;


    //stack_size=0x40;
    s = pthread_attr_init(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_init");

    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (s != 0)
        handle_error_en(s, "pthread_attr_setdetachstate");

    s = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (s != 0)
        handle_error_en(s, "pthread_attr_setinheritsched");

    s = posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size);
    if (s != 0)
        handle_error_en(s, "posix_memalign");

    //stack_size = strtoul(argv[1], NULL, 0);
    // stack_size = 0x40800;

    // s = pthread_attr_setstack(&attr, sp, stack_size);
    // if (s != 0)
    //     handle_error_en(s, "pthread_attr_setstack");


    if (sem_init(&s_full, 0, 0) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&s_empty, 0, 20) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }


}

void print_list(){
    // 下面把这个链表中各个节点的值打印出来
    printf("\n");
    printf("   data       len\n");

    list_for_each(pos, &frame_head)
    {
        // list_entry来取得pos所在的结构的指针(go back)
        tmp_list = list_entry(pos, struct frame_list, list);
        printf("%d,  %d \n", tmp_list->fram.data, tmp_list->fram.len);
        //fwrite((void*)tmp_list->fram.data,tmp_list->fram.len,1,fh264);
    }
    printf("\n");

    // while(1){


    // }


}

void thread_start(){
    //pthread_create(&prod,NULL,producer,NULL);
    //slep(3);
    //int i,j;
#if defined(CARRAY)
    LOG(000,"CARRAY!\n");
    s = pthread_create(&prod, NULL, &producer_carray, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_create");

    for(i=0;i<num_threads;i++){
        //pthread_create(&cons[i],NULL,consumer,NULL);
        s = pthread_create(&cons[i], NULL, &consumer_carray, NULL);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }
#elif defined(LINUX_LIST)
    LOG(000,"LINUX_LIST\n");
    s = pthread_create(&prod, NULL, &producer_list, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_create");

    for(i=0;i<num_threads;i++){
        //pthread_create(&cons[i],NULL,consumer,NULL);
        s = pthread_create(&cons[i], NULL, &consumer_list, NULL);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }

#else 

    LOG(000,"\nERROROUS DATA STRUCTURE!\n");

#endif


}

void thread_attr_destroy(){
    //pthread_join(prod,NULL);

    for(j=0;j<num_threads;j++){
        //  pthread_join(cons[j],NULL);
        //printf("aaa" "bbb");
        //LOG(444,"\n\nIN JOIN!\n\n");
    }

    if (attrp != NULL) {
        s = pthread_attr_destroy(attrp);
        if (s != 0)
            handle_error_en(s, "pthread_attr_destroy");
    }

    s = sem_destroy(&s_full);
    if(s!=0) 
        handle_error_en(s, "sem_destroy");

    sem_destroy(&s_empty);
    if(s!=0) 
        handle_error_en(s, "sem_destroy");


}

void multiprocesses(){
    char * ffmpeg_args[] = { "/usr/local/bin/ffmpeg" , "-i", "BB.h264", "BB.mp4", NULL};
    printf( "The process identifier (pid) of the parent process is %d\n", (int)getpid() );
    int pid = fork();

    if(pid<0){
        perror("fork error!\n");
        exit(0);
    }
    
    if ( pid == 0 ) {
        execv(ffmpeg_args[0],ffmpeg_args);
        printf("never arrive here!\n\n");
    } else {
        //waitpid(-1,0,WNOHANG);
        waitpid(-1,0,WUNTRACED);
        printf("pararent wait...3\n\n");
        sleep(1);
        printf("pararent wait...2\n\n");
        sleep(1);
        printf("pararent wait...1\n\n");
        sleep(1);
        print_list();
    }
}

int main(int argc, char *argv[]){

    frame_list *frame_tmp;

    fbin = fopen("./BB.bin","rb");
    fh264 = fopen("./BB.h264","wb");

    count = 0;
    get = put = 0;

    INIT_LIST_HEAD(&frame_head);   //初始化链表头
    signal(SIGINT, signal_handler);  //注册信号处理

    

    tcp_ready();    

    thread_init();
    display_pthread_attr(&attr,"\t");
    thread_start();
    getchar();
    multiprocesses();
    return 0;

}
