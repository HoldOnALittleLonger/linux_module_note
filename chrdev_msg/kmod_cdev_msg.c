#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/gfp.h>
#include <linux/slab.h>

static const u32 MAJOR_DEV_NUMBER = 52;
static const char *cdev_name = "kmod_cdev_msg_chardev";

/* for vfs_read() */
static const char *msg_to_user = "please write something into this cdev.";
static size_t cdev_msg_length = 0;

static int kcmsg_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "the char device is opened by %s",
               current->comm);
        return 0;
}

static ssize_t kcmsg_read(struct file *file, char __user *ubuf,
                          size_t ubuf_len, loff_t *fpos)
{
        if (ubuf_len <= cdev_msg_length)
                return -EINVAL;

        size_t rest = copy_to_user(ubuf, msg_to_user, cdev_msg_length);
        if (unlikely(rest)) {
                printk(KERN_INFO "failed to copy everything to user buffer,"
                       "copied %d",
                       cdev_msg_length - rest);
                return -EIO;
        }

        /* we do not update file position in reading,because character device
         * is a stream on the concept
         */

        return cdev_msg_length - rest;
}

static ssize_t kcmsg_write(struct file *file, const char __user *ubuf,
                           size_t ubuf_len, loff_t *fpos)
{
        void *kbuffer = kzalloc(ubuf_len + 1, GFP_KERNEL);
        if (!kbuffer) {
                printk(KERN_ERR "failed to allocate memory space for size %d",
                       ubuf_len + 1);
                return -ENOMEM;
        }

        size_t rest = copy_from_user(kbuffer, ubuf, ubuf_len);
        if (unlikely(rest)) 
                printk(KERN_ERR "failed to copy everything from user space,"
                       "copied %d",
                       ubuf_len - rest);
        printk(KERN_INFO "received msg from user space - %s", (const char *)kbuffer);

        /* we do not update file position in writing,because character device is
         * a stream on the concept
         */

        kfree(kbuffer);
        return ubuf_len - rest;
}

static struct file_operations kmod_cm_fops = {
        .open = kcmsg_open,
        .read = kcmsg_read,
        .write = kcmsg_write
};

__init int kmod_cdev_msg_init(void)
{
        printk(KERN_INFO "ready to initialize module %s",
               module_name(THIS_MODULE));
        cdev_msg_length = strlen(msg_to_user);

        int ret = register_chrdev(MAJOR_DEV_NUMBER, cdev_name, &kmod_cm_fops);
        if (ret < 0) {
                printk(KERN_ERR "failed to register char device - %s - "
                       "major : %u",
                       cdev_name, MAJOR_DEV_NUMBER);
                return ret;
        }

        printk(KERN_INFO "registered char device");
        return 0;
}

__exit void kmod_cdev_msg_exit(void)
{
        printk(KERN_INFO "unregister char device");
        unregister_chrdev(MAJOR_DEV_NUMBER, cdev_name);
}

MODULE_LICENSE("GPL v2");
MODULE_VERSION("v0.1");
MODULE_AUTHOR("Zephyr");
MODULE_DESCRIPTION("Read from userspace and printk.");

module_init(kmod_cdev_msg_init);
module_exit(kmod_cdev_msg_exit);
