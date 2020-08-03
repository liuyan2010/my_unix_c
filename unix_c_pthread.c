#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

/*
多线程实现打印'ABC'十次

使用函数：
	pthread_create：创建线程
	pthread_join ：启动线程
	pthread_mutex_lock ：加锁
	pthread_cond_wait ：条件等待(等待信号触发)
	pthread_cond_signal : 发信号去出发wait状态
*/

/*
	注意：pthread_cond_signal发出信号前，其他线程必须已经进入wait状态
	因此：
		thread_C加了一个usleep(1)
		每个线程都用了自己的lock

	This prograss is bad, it is not currectly carried for 100% . 	
*/

pthread_mutex_t Alock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Block = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Clock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t AReady = PTHREAD_COND_INITIALIZER;
pthread_cond_t BReady = PTHREAD_COND_INITIALIZER;
pthread_cond_t CReady = PTHREAD_COND_INITIALIZER;

void *thread_A(void *arg)
{
    //A线程
    //printf("thread_A!\n");
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&Alock);
        pthread_cond_wait(&AReady, &Alock);

        printf("A");

        pthread_mutex_unlock(&Alock);
        pthread_cond_signal(&BReady);
    }
    return (void *)0;
}

void *thread_B(void *arg)
{
    //B线程
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&Block);
        pthread_cond_wait(&BReady, &Block);

        printf("B");
		
        pthread_mutex_unlock(&Block);
        pthread_cond_signal(&CReady);
    }
    return (void *)0;
}

void *thread_C(void *arg)
{
    //C线程
    usleep(1);
	pthread_cond_signal(&AReady);
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&Clock);
        pthread_cond_wait(&CReady, &Clock);

        printf("C");
		
        pthread_mutex_unlock(&Clock);
        pthread_cond_signal(&AReady);
    }
    return (void *)0;
}

int main()
{
    pthread_t tid[3];
    void *tret;

	printf("begin thread!\n");
    pthread_create(&tid[0], NULL, thread_C, NULL);
    pthread_create(&tid[1], NULL, thread_B, NULL);
    pthread_create(&tid[2], NULL, thread_A, NULL);

    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], &tret);
    }
	usleep(2);
	pthread_cond_signal(&AReady);

    return 0;
}

