/************************************************
 *
 *	The classic producer-consumer example.
 * 	Illustrates mutexes and conditions.
 *  by Zou jian guo <ah_zou@tom.com>   
 *  2003-12-22
 *
*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#define BUFFER_SIZE 16


#define TUBE_IOCTROL  0x11
#define DOT_IOCTROL   0x12

void jmdelay(int n) {
    int i,j,k;
    for (i=0;i<n;i++)
        for (j=0;j<100;j++)
            for (k=0;k<100;k++);
}
unsigned char state[2]={ 0xBE, 0xDD};
int cur = 0;
void* run_automatically(void* data) {
    int fd;
    int i,j,k;
    unsigned int LEDWORD;
    unsigned int MLEDA[8];
    unsigned char LEDCODE[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
    unsigned char dd_data[16][10]=
	{
	{0xff,0,0,0,0,0,0,0,0,0},
	{0,0xff,0,0,0,0,0,0,0,0},
	{0,0,0xff,0,0,0,0,0,0,0},
	{0,0,0,0xff,0,0,0,0,0,0},
	{0,0,0,0,0xff,0,0,0,0,0},
	{0,0,0,0,0,0xff,0,0,0,0},
	{0,0,0,0,0,0,0xff,0,0,0},
	{0,0,0,0,0,0,0,0xff,0,0},
	{0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0,0},
	{0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0,0},
	{0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0,0},
	{0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0,0},
	{0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0,0},
	{0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0,0},
	{0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0,0},
	{0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0,0},
    };
    fd=open("/dev/s3c2440_led0",O_RDWR);
    if (fd < 0) {
        printf("####Led device open fail####\n");
        return (-1);
    }

    LEDWORD=0xff00;
	
	__asm
	(
		"MOV R5,#0X011;"
		"STRB R5,[R4];"
			
	);
	unsigned char state[2]={ 0xBE, 0xDD};
	int cur = 0;
	while (1) {
		LEDWORD=state[cur]<<8|0xff;
		ioctl(fd,0x12,LEDWORD);
        jmdelay(3000);
		cur=1-cur;
	}
	printf("will enter TUBE LED  ,please waiting .............. \n");
    LEDWORD=0xff00;
    ioctl(fd,0x12,LEDWORD);
    sleep(1);

    for (j=0;j<2;j++)
        for (i=0;i<10;i++)

        {
            LEDWORD=(LEDCODE[i]<<8)|LEDCODE[9-i];
            ioctl(fd,0x12,LEDWORD);
            jmdelay(1500);
        }

    printf("will enter DIG LED  ,please waiting .............. \n");

    sleep(1);

    for (i=0;i<16;i++) {
		write(fd,dd_data[i],10);
		jmdelay(1000);
    }
    while(1){
		unsigned char a[10];
		for (i=0;i<10;i++)
			a[i] = rand() % 255;
		write(fd,a,10);
		jmdelay(500);
            
    }
    close(fd);
    return NULL;

}






/* Circular buffer of integers. */
struct prodcons {
  int buffer[BUFFER_SIZE];      /* the actual data */
  pthread_mutex_t lock;         /* mutex ensuring exclusive access to buffer */
  int readpos, writepos;        /* positions for reading and writing */
  pthread_cond_t notempty;      /* signaled when buffer is not empty */
  pthread_cond_t notfull;       /* signaled when buffer is not full */
};

/*--------------------------------------------------------*/
/* Initialize a buffer */
void init(struct prodcons * b)
{
  pthread_mutex_init(&b->lock, NULL);
  pthread_cond_init(&b->notempty, NULL);
  pthread_cond_init(&b->notfull, NULL);
  b->readpos = 0;
  b->writepos = 0;
}
/*--------------------------------------------------------*/
/* Store an integer in the buffer */
void put(struct prodcons * b, int data)
{
	pthread_mutex_lock(&b->lock);

  	/* Wait until buffer is not full */
 	while ((b->writepos + 1) % BUFFER_SIZE == b->readpos) {
		printf("wait for not full\n");
    	pthread_cond_wait(&b->notfull, &b->lock);
  	}
  /* Write the data and advance write pointer */
  	b->buffer[b->writepos] = data;
  	b->writepos++;
  	if (b->writepos >= BUFFER_SIZE) b->writepos = 0;
  /* Signal that the buffer is now not empty */
  	pthread_cond_signal(&b->notempty);

	pthread_mutex_unlock(&b->lock);
}
/*--------------------------------------------------------*/
/* Read and remove an integer from the buffer */
int get(struct prodcons * b)
{
  	int data;
	pthread_mutex_lock(&b->lock);

 	/* Wait until buffer is not empty */
  	while (b->writepos == b->readpos) {
    	printf("wait for not empty\n");
		pthread_cond_wait(&b->notempty, &b->lock);
  	}
  	/* Read the data and advance read pointer */
  	data = b->buffer[b->readpos];
  	b->readpos++;
  	if (b->readpos >= BUFFER_SIZE) b->readpos = 0;
  	/* Signal that the buffer is now not full */
  	pthread_cond_signal(&b->notfull);

  	pthread_mutex_unlock(&b->lock);
  	return data;
}
/*--------------------------------------------------------*/
#define OVER (-1)
struct prodcons buffer;
/*--------------------------------------------------------*/
void * producer(void * data)
{
  	int n;
  	for (n = 0; n < 1000; n++) {
    	printf(" put-->%d\n", n);
    	put(&buffer, n);
	}
  put(&buffer, OVER);
  printf("producer stopped!\n");
  return NULL;
}
/*--------------------------------------------------------*/
void * consumer(void * data)
{
  int d;
  while (1) {
    d = get(&buffer);
    if (d == OVER ) break;
    printf("              %d-->get\n", d);
  }
  printf("consumer stopped!\n");
  return NULL;
}
/*--------------------------------------------------------*/

void* change_maually(void* data){

	while(true){
		getchar();
		cur=1-cur;
		LEDWORD=state[cur]<<8|0xff;
		ioctl(fd,0x12,LEDWORD);
        jmdelay(3000);
	}

}

int main(void)
{
  	pthread_t th_a, th_b;
  	void * retval;

  	init(&buffer);
 	pthread_create(&th_a, NULL, run_automatically, 0);
  	pthread_create(&th_b, NULL, change_maually, 0);
  /* Wait until producer and consumer finish. */
  	pthread_join(th_a, &retval);
  	pthread_join(th_b, &retval);
	while(1){

	}
  	return 0;
}
