/*from dodng
2016.12.9*/
#include <stdio.h>
#include "ring_queue.h" 
#include <unistd.h>
#include <sys/time.h>


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define DATA_TYPE int

#define THREAD_NUM 1
#define LOOP_SIZE  10000


void *customer(void *arg)
{
	RingQueue queue = *(RingQueue *)arg;
    uchar get_num = 0;
    while(1)
    {
        for(int i = 0;i < LOOP_SIZE; )
        {
            int *p = 0;
            //begin--for notify
            recv(queue.notify_fd[1], &get_num, sizeof(get_num), 0);
            for(int j = 0 ; j < get_num;j++)
            //end----for notify
            {
                p = (DATA_TYPE *)queue.solo_read();
                if (p)
                {
                    assert(i == *p);
                    //				printf("[%d]:%d\n",i,*p);
                    queue.solo_read_over();
                    i++;
                }
            }
        }
    }
}

void *producer(void *arg)
{
    RingQueue queue = *(RingQueue *)arg;

    int loop = 0;
    struct timeval last_time,now_time;
    gettimeofday(&last_time,NULL);
    gettimeofday(&now_time,NULL);
    uchar wrote_num = 0;
    while(1)
    {
        for(int i = 0;i < LOOP_SIZE; )
        {
            int *p = 0;
            p = (DATA_TYPE *)queue.solo_write();
            if (p)
            {
                *p = i;
                queue.solo_write_over();
                i++;
                //begin--for notify
                wrote_num++;
                if (wrote_num >= 200)
                {
                    send(queue.notify_fd[0], &wrote_num, sizeof(wrote_num), MSG_NOSIGNAL);
                    wrote_num = 0;
                }
                //end----for notify
            }
        }
        gettimeofday(&now_time,NULL);
        if (now_time.tv_sec - last_time.tv_sec >= 5)
        {
            printf("LOOP_COUNT is %.2f=[ %d[RING_SIZE] * %.2f[RING_COUNT]] per second\n",(LOOP_SIZE*loop)/5.0,LOOP_SIZE,loop/5.0);
            last_time = now_time;
            loop = 0;
        }
        loop++;
    }
}

int main(int argc,char *argv[])
{
    pthread_t tid_customer[THREAD_NUM];
    pthread_t tid_producer[THREAD_NUM];
    RingQueue *queue = new RingQueue[THREAD_NUM](LOOP_SIZE,sizeof(DATA_TYPE));

    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (pthread_create(&tid_customer[i],NULL,&customer,(void*)&queue[i]) != 0)
        {
            fprintf(stderr,"thread create failed\n");
            return -1;
        }
    }
    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (pthread_create(&tid_producer[i],NULL,&producer,(void*)&queue[i]) != 0)
        {
            fprintf(stderr,"thread create failed\n");
            return -1;
        }
    }

    for (int i = 0 ;i < THREAD_NUM; i++)
        pthread_join(tid_customer[i],NULL);

    for (int i = 0 ;i < THREAD_NUM; i++)
        pthread_join(tid_producer[i],NULL);

}
