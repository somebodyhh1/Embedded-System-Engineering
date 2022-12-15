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
    
    return (void*)NULL;

}

void* change_maually(void* data){
	int fd=open("/dev/s3c2440_led0",O_RDWR);
    if (fd < 0) {
        printf("####Led device open fail####\n");
        return (-1);
    }
	while(1){
		getchar();
		cur=1-cur;
		unsigned int LEDWORD=state[cur]<<8|0xff;
		ioctl(fd,0x12,LEDWORD);
        jmdelay(3000);
	}

}

int main(void)
{
	printf("123");
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
	pthread_create(&th_b, NULL, change_maually, 0);
	while (1) {
		LEDWORD=state[cur]<<8|0xff;
		ioctl(fd,0x12,LEDWORD);
        jmdelay(3000);
		cur=1-cur;
	}
    close(fd);
  	/*pthread_t th_a, th_b;
 	pthread_create(&th_a, NULL, run_automatically, 0);
  	*/
  /* Wait until producer and consumer finish. */
  	/*pthread_join(th_a, &retval);
  	pthread_join(th_b, &retval);*/

  	return 0;
}
