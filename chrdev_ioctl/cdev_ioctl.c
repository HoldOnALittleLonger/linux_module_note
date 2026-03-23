#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/printk.h>
#include <asm-generic/ioctl.h>

/* char device */
#define CIOC_CDEV_NAME "cioc_cdev"
#define CIOC_CDEV_MAJOR 52

/* _IO ioctl */
#define CIOC_TYPE 0xa6
#define CIOC_CMD_DS 0x00

/* ioctl cmd */
#define CIOC_DUMPSTACK _IO(CIOC_TYPE, CIOC_CMD_DS)

/**
 * buffer used to saves ioctl number.
 * userland process can invoke sys_read()
 * on the char device to get ioctl number.
 */
#define CIOC_NSTR_LENGTH 8
static char cioc_ioctl_numstring[CIOC_NSTR_LENGTH] = {0};

static int cioc_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "process %s opened device file.", current->comm);
        return 0;
}

/* char device has no EOF */
static ssize_t cioc_read(struct file *file, char __user *ubuffer,
                         size_t ub_length, loff_t *fpos)
{
        if (ub_length < CIOC_NSTR_LENGTH)
                return -EINVAL;

        size_t rest = copy_to_user(ubuffer, cioc_ioctl_numstring, CIOC_NSTR_LENGTH);
        if (unlikely(rest))
                return -EIO;

        return CIOC_NSTR_LENGTH - rest;
}

/* we only handle one command */
static long cioc_ioctl(struct file *file, unsigned int cmd, unsigned long data_ptr)
{
        switch (cmd) {
        case CIOC_DUMPSTACK:
                dump_stack();
                return 0;
        default:
                return -ENOIOCTLCMD;
        }
}

static struct file_operations cioc_fops = {
        .open = cioc_open,
        .read = cioc_read,
        .unlocked_ioctl = cioc_ioctl
};

__init int cioc_cdev_init(void)
{
        printk(KERN_INFO "module initializing.");
        int ret = snprintf(cioc_ioctl_numstring, CIOC_NSTR_LENGTH, "%u", CIOC_DUMPSTACK);
        if (!ret) {
                printk(KERN_ERR "initialize ioctl number string failed.");
                return -1;
        }

        ret = register_chrdev(CIOC_CDEV_MAJOR, CIOC_CDEV_NAME, &cioc_fops);
        if (ret < 0) {
                printk(KERN_ERR "failed to register char device - %s .", CIOC_CDEV_NAME);
                return ret;
        }

        printk(KERN_INFO "registered char device - %s .", CIOC_CDEV_NAME);

        return 0;
}

__exit void cioc_cdev_exit(void)
{
        unregister_chrdev(CIOC_CDEV_MAJOR, CIOC_CDEV_NAME);
}

module_init(cioc_cdev_init);
module_exit(cioc_cdev_exit);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("v0.1");
MODULE_AUTHOR("Zephyr");
MODULE_DESCRIPTION("Just an ioctl test module,major number is 52" 
                   ".read will return ioctl number.");
