#include "chardevjni.h"
#include <fcntl.h>

#define  LOG_TAG    "chardevjni"

#include <utils/Log.h>
#define DEVICE_NAME "/dev/char_dev"

JNIEXPORT jint JNICALL Java_com_wzf_chardev_chardevclass_read
  (JNIEnv * env, jobject obj)
{
	int fd = -1;
	int val = 0;
	fd = open(DEVICE_NAME, O_RDWR);
	if(fd == -1) {
		LOGI("Failed to open device %s.\n", DEVICE_NAME);
		return -1;
	}

	read(fd, &val, sizeof(val));
	LOGI("Read original value:%d.\n", val);
        close(fd);
	return -1; 
}
JNIEXPORT jint JNICALL Java_com_wzf_chardev_chardevclass_write
  (JNIEnv * env, jobject obj, jint value)
{
	int fd = -1;
	int val = 0;
	fd = open(DEVICE_NAME, O_RDWR);
	if(fd == -1) {
		LOGI("Failed to open device %s.\n", DEVICE_NAME);
		return -1;
	}
	
	val = 8888;
	LOGI("Write value %d to %s.\n", val, DEVICE_NAME);
        write(fd, &val, sizeof(val));
	close(fd);
	return -1; 
}
