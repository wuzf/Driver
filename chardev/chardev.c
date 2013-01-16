#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

//=======================字符设备驱动模板开始 ===========================//
#define CHAR_DEV_DEVICE_NAME   "char_dev"   // 是应当连接到这个编号范围的设备的名字，出现在/proc/devices和sysfs中
#define CHAR_DEV_NODE_NAME   "char_dev"   // 节点名，出现在/dev中
#define CHAR_DEV_CLASS_NAME   "char_dev_class"   //出现在/sys/devices/virtual/和/sys/class/中
struct class *char_dev_class;  // class结构用于自动创建设备结点 
static int major = 0;// 0表示动态分配主设备号，可以设置成未被系统分配的具体的数字。
static struct cdev char_dev_cdev;// 定义一个cdev结构

// 进行初始化设置，打开设备，对应应用空间的open 系统调用 
int char_dev_open(struct inode *inode, struct file *filp)
{
    //  这里可以进行一些初始化
    printk("char_dev device open.\n");
    return 0;
}
 
// 释放设备，关闭设备，对应应用空间的close 系统调用
static int char_dev_release (struct inode *node, struct file *file)
{
    //  这里可以进行一些资源的释放
    printk("char_dev device release.\n");
    return 0;
}
// 实现读功能，读设备，对应应用空间的read 系统调用
/*__user. 这种注解是一种文档形式, 注意, 一个指针是一个不能被直接解引用的
用户空间地址. 对于正常的编译, __user 没有效果, 但是它可被外部检查软件使
用来找出对用户空间地址的错误使用.*/
ssize_t char_dev_read(struct file *file,char __user *buff,size_t count,loff_t *offp)
{
    printk("char_dev device read.\n");
    return 0;
}
// 实现写功能，写设备，对应应用空间的write 系统调用
ssize_t char_dev_write(struct file *file,const char __user *buff,size_t count,loff_t *offp)
{
    printk("char_dev device write.\n");
    return 0;
}
 
// 实现主要控制功能，控制设备，对应应用空间的ioctl系统调用
static int char_dev_ioctl(struct inode *inode,struct file *file,unsigned int cmd,unsigned long arg)
{  
    printk("char_dev device ioctl.\n");
    return 0;
}

//  file_operations 结构体设置，该设备的所有对外接口在这里明确，此处只写出了几常用的
static struct file_operations char_dev_fops = 
{
    .owner = THIS_MODULE,
    .open  = char_dev_open,      // 打开设备 
    .release = char_dev_release, // 关闭设备 
    .read  = char_dev_read,      // 实现设备读功能 
    .write = char_dev_write,     // 实现设备写功能 
    .ioctl = char_dev_ioctl,     // 实现设备控制功能 
};

// 设备建立子函数，被char_dev_init函数调用  
static void char_dev_setup_cdev(struct cdev *dev, int minor, struct file_operations *fops)
{
    int err, devno = MKDEV(major, minor);
    cdev_init(dev, fops);//对cdev结构体进行初始化
    dev->owner = THIS_MODULE;
    dev->ops = fops;
    err = cdev_add(dev, devno, 1);//参数1是应当关联到设备的设备号的数目. 常常是1
    if(err)
    {
        printk(KERN_NOTICE "Error %d adding char_dev %d.\n", err, minor);
    }
    printk("char_dev device setup.\n");
}

//   设备初始化 
static int char_dev_init(void)
{
    int result;
    dev_t dev = MKDEV(major, 0);//将主次编号转换为一个dev_t类型
    if(major)
    {
        // 给定设备号注册
        result = register_chrdev_region(dev, 1, CHAR_DEV_DEVICE_NAME);//1是你请求的连续设备编号的总数
        printk("char_dev register_chrdev_region.\n");
    }
    else
    {
        // 动态分配设备号 
        result = alloc_chrdev_region(&dev, 0, 1, CHAR_DEV_DEVICE_NAME);//0是请求的第一个要用的次编号，它常常是 0
        printk("char_dev alloc_chrdev_region.\n");
        major = MAJOR(dev);
    }
    if(result < 0)//获取设备号失败返回
    {
        printk(KERN_WARNING "char_dev region fail.\n");
        return result;
    }
    char_dev_setup_cdev(&char_dev_cdev, 0, &char_dev_fops);
    printk("The major of the char_dev device is %d.\n", major);
    //==== 有中断的可以在此注册中断：request_irq，并要实现中断服务程序 ===//
    // 创建设备节点
    char_dev_class = class_create(THIS_MODULE,CHAR_DEV_CLASS_NAME);
    if (IS_ERR(char_dev_class))
    {
        printk("Err: failed in creating char_dev class.\n");
        return 0;
    }
    device_create(char_dev_class, NULL, dev, NULL, CHAR_DEV_NODE_NAME);
    printk("char_dev device installed.\n");
    return 0;
}

// 设备注销 
static void char_dev_cleanup(void)
{
    device_destroy(char_dev_class,MKDEV(major, 0));
    class_destroy(char_dev_class);
    cdev_del(&char_dev_cdev);//字符设备的注销
    unregister_chrdev_region(MKDEV(major, 0), 1);//设备号的注销
    //========  有中断的可以在此注销中断：free_irq  ======//
    printk("char_dev device uninstalled.\n");
}

module_init(char_dev_init);//模块初始化接口
module_exit(char_dev_cleanup);//模块注销接口
//所有模块代码都应该指定所使用的许可证，该句不能省略，否则模块加载会报错
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Author");
MODULE_DESCRIPTION("Driver Description");
