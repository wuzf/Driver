#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define DEVICE_NAME "/dev/char_dev"
int main(int argc, char** argv)
{
	int fd = -1;
	int val = 0;
	fd = open(DEVICE_NAME, O_RDWR);
	if(fd == -1) {
		printf("Failed to open device %s.\n", DEVICE_NAME);
		return -1;
	}
	
	read(fd, &val, sizeof(val));
	printf("read val=%d.\n", val);

	val = 99;
	printf("write val %d to %s.\n", val, DEVICE_NAME);
        write(fd, &val, sizeof(val));
        read(fd, &val, sizeof(val));
        printf("read val=%d.\n", val);

        ioctl(fd,1,88);
	read(fd,&val,sizeof(val));
	printf("ioctl write val=%d.\n",val);

	ioctl(fd,0,&val);
	printf("ioctl read val=%d.\n", val);

	close(fd);
	return 0;
}
