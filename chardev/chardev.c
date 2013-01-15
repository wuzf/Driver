//=======================字符设备驱动模板开始 ===========================//
#define CHAR_DEV_DEVICE_NAME   "char_dev"   // 设备名
struct class *char_dev _class;  // class结构用于自动创建设备结点 
static int major = 0;/* 0表示动态分配主设备号，可以设置成未被系统分配的具体的数字。*/
static struct cdev char_dev_devs;// 定义一个cdev结构
// 设备建立子函数，被char_dev_init函数调用  
static void char_dev_setup_cdev(struct cdev *dev, int minor, struct file_operations *fops)
{
    int err, devno = MKDEV(major, minor);
    
    cdev_init(dev, fops);
    dev->owner = THIS_MODULE;
    dev->ops = fops;
    
    err = cdev_add(dev, devno, 1);
    if( err )
    {
        printk(KERN_NOTICE "Error %d adding char_dev %d\n", err, minor);
    }
}

//  file_operations 结构体设置，该设备的所有对外接口在这里明确，此处只写出了几常用的 
static struct file_operations char_dev_fops = 
{
    .owner = THIS_MODULE,
    .open  = char_dev_open,      // 打开设备 
    .release = char_dev_release, // 关闭设备 
    .read  = char_dev_read,      // 实现设备读功能 
    .write = char_dev_write,     // 实现设备写功能 
    .ioctl = char_dev_ioctl,     //实现设备控制功能 
};

// 进行初始化设置，打开设备，对应应用空间的open 系统调用 
int char_dev_open (struct inode *inode, struct file *filp)
{
//  这里可以进行一些初始化
return 0;
}
 
// 释放设备，关闭设备，对应应用空间的close 系统调用
static int char_dev_release (struct inode *node, struct file *file)
{
    //  这里可以进行一些资源的释放
    return 0;
}
// 实现读功能，读设备，对应应用空间的read 系统调用
ssize_t char_dev_read (struct file *file,char __user *buff,size_t count,loff_t *offp)
{
    return 0;
}
// 实现写功能，写设备，对应应用空间的write 系统调用
ssize_t char_dev_write(struct file *file,const char __user *buff,size_t count,loff_t *offp)
{
    return 0;
}
 
// 实现主要控制功能，控制设备，对应应用空间的ioctl 系统调用
static int char_dev_ioctl(struct inode *inode,struct file *file,unsigned int cmd,unsigned long arg)
{  
    return 0;
}
//   设备初始化 
static int char_dev_init(void)
{
    int result;
    dev_t dev = MKDEV(major, 0);
    
    if( major )
    {
        // 给定设备号注册 
        result = register_chrdev_region(dev, 1, CHAR_DEV_DEVICE_NAME);
    }
    else
    {
        // 动态分配设备号 
        result = alloc_chrdev_region(&dev, 0, 1, CHAR_DEV_DEVICE_NAME);
        major = MAJOR(dev);
    }
    
    char_dev_setup_cdev(&char_dev_devs, 0, &char_dev_fops);
    printk("The major of the char_dev device is %d\n", major);
    //==== 有中断的可以在此注册中断：request_irq，并要实现中断服务程序 ===//
    // 创建设备结点
    char_dev_class = class_create(THIS_MODULE,"ad_class");
    if (IS_ERR(char_dev_class))
    {
        printk("Err: failed in creating class.\n");
        return 0;
    }
    device_create(char_dev_class, NULL, dev, NULL, "char_dev");
    return 0;
}

// 设备注销 
static void char_dev_cleanup(void)
{
    device_destroy(adc_class,dev);
    class_destroy(adc_class);
    cdev_del(&char_dev_devs);//字符设备的注销*/
    unregister_chrdev_region(MKDEV(major, 0), 1);//设备号的注销
    //========  有中断的可以在此注销中断：free_irq  ======//

    printk("char_dev device uninstalled\n");
}

module_init(char_dev_init);//模块初始化接口
module_exit(char_dev_cleanup);//模块注销接口

// 以下两句不能省略，否则编译不通过 
MODULE_AUTHOR("wzf");
MODULE_LICENSE("GPL");
