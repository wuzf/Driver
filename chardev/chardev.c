#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
//=======================字符设备驱动模板开始 ===========================//
#define CHAR_DEV_DEVICE_NAME   "char_dev"   // 是应当连接到这个编号范围的设备的名字，出现在/proc/devices和sysfs中
#define CHAR_DEV_NODE_NAME   "char_dev"   // 节点名，出现在/dev中
#define CHAR_DEV_CLASS_NAME   "char_dev_class"   //出现在/sys/devices/virtual/和/sys/class/中
struct class *char_dev_class;  // class结构用于自动创建设备结点 
static int major = 0;// 0表示动态分配主设备号，可以设置成未被系统分配的具体的数字。
static struct cdev char_dev_cdev;// 定义一个cdev结构
static int value = 0;
#define FILENAME "/data/char_dev.dat"//用于保存用户数据
static struct file *filp = NULL;
static int read_from_file(void)
{
    int val=0;
    int* intp=&val;
    mm_segment_t old_fs;
    ssize_t ret;
    printk("read_from_file.\n");
    filp = filp_open(FILENAME, O_RDWR | O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
    if(IS_ERR(filp))
        printk("open %s error.\n",FILENAME);
   
    old_fs = get_fs();
    set_fs(get_ds());
    filp->f_op->llseek(filp,0,0);
    ret = filp->f_op->read(filp, (char __user *)intp, sizeof(int), &filp->f_pos);
    set_fs(old_fs);
   
    if(ret > 0)
        printk("read %d from file.\n",*intp);
    else if(ret == 0)
        printk("read nothing.\n");
    else
    {
        printk("read error.\n");
        return -1;
    }
    filp_close(filp,NULL);
    return (*intp);
}

static void write_to_file(int data)
{
    int* intp=&data;
    mm_segment_t old_fs;
    printk("write_to_file.\n");
    filp = filp_open(FILENAME, O_RDWR | O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
    if(IS_ERR(filp))
        printk("open %s error.\n",FILENAME);
   
    old_fs = get_fs();
    set_fs(get_ds());
    filp->f_op->write(filp, (char __user *)intp, sizeof(int), &filp->f_pos);
    set_fs(old_fs);
    filp_close(filp,NULL);
}

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
    value=read_from_file();
    copy_to_user(buff, &value, sizeof(value));
    return 0;
}
// 实现写功能，写设备，对应应用空间的write 系统调用
ssize_t char_dev_write(struct file *file,const char __user *buff,size_t count,loff_t *offp)
{
    printk("char_dev device write.\n");
    copy_from_user(&value, buff, count);
    write_to_file(value);
    return 0;
}
 
// 实现主要控制功能，控制设备，对应应用空间的ioctl系统调用
static int char_dev_ioctl(struct inode *inode,struct file *file,unsigned int cmd,unsigned long arg)
{  
    switch (cmd)
    {
        case 0://read
            value=read_from_file();
            *((unsigned int *)arg)=value;
            printk(KERN_ALERT"wzf test ioctl read value=%d!\n",value);
            break;
        case 1://write
            value=(int)arg;
            write_to_file(value);
            printk(KERN_ALERT"wzf test ioctl write %d!\n",(int)arg); 
            break;
        default:
            printk(KERN_ALERT"default!"); 
            break;/* Invalid argument */
    }
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
    value=read_from_file();//从文件初始化值
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
