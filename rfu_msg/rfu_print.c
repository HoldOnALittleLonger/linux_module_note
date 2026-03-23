#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/stat.h>

static const char *RFU_MSG_TO_USER = "please write something into this file.";
static size_t RFU_MSG_LENGTH = 0;

static int rfu_proc_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "rfu_proc_open() is called");
        return 0;
}

static ssize_t rfu_proc_read(struct file *file, char __user *user_buffer,
                             size_t len, loff_t *pos)
{
        if (len <= RFU_MSG_LENGTH) {
                printk(KERN_INFO "invalid length of buffer for read(),"
                       "at least is %hu bytes",
                       RFU_MSG_LENGTH + 1);
                return -EINVAL;
        }

        /* EOF */
        if (*pos >= RFU_MSG_LENGTH)
                return 0;
        size_t rest = copy_to_user(user_buffer, RFU_MSG_TO_USER, RFU_MSG_LENGTH);
        size_t readed = RFU_MSG_LENGTH - rest;

        if (unlikely(rest)) {
                printk(KERN_INFO "copy_to_user() failed to copy everything,"
                       "copied %d",
                       readed);
                return -EAGAIN;
        }

        /* even if we are not readed all data,we still prevent read again.
         * actually,read a partial of the message is the unlikely case.
         */
        *pos += RFU_MSG_LENGTH;
        return readed;
}

static ssize_t rfu_proc_write(struct file *file, const char __user *user_buffer,
                              size_t data_length, loff_t *pos)
{
        void *kbuffer = kcalloc(data_length + 1, sizeof(char), GFP_KERNEL);
        if (!kbuffer) {
                printk(KERN_ERR "attempt to allocate memory space for size "
                       "is %d bytes with GFP_KERNEL was failed",
                       data_length + 1);
                return -ENOMEM;
        }

        size_t rest = copy_from_user(kbuffer, user_buffer, data_length);
        size_t written = data_length - rest;
        if (unlikely(rest))
                printk(KERN_INFO "failed to copy everything from user,"
                       "copied %u",
                       written);

        printk(KERN_INFO "received msg from user : %s", (char *)kbuffer);
                
        kfree(kbuffer);
        return written;
}

static struct proc_ops rfu_proc_ops = {
        .proc_flags = PROC_ENTRY_PERMANENT,
        .proc_open = rfu_proc_open,
        .proc_read = rfu_proc_read,
        .proc_write = rfu_proc_write
};

static struct proc_dir_entry *rfu_proce = NULL;

#define RFU_PROCE_NAME "rfu_proc_entry"

__init int rfu_init(void)
{
        printk(KERN_INFO "rfu_init(),ready to create proc entry");
        RFU_MSG_LENGTH = strlen(RFU_MSG_TO_USER);

        rfu_proce = proc_create(RFU_PROCE_NAME, S_IRUSR | S_IWUSR, NULL, &rfu_proc_ops);
        if (!rfu_proce) {
                printk(KERN_ERR "failed to create proc entry - %s", RFU_PROCE_NAME);
                return -EPERM;
        }

        printk(KERN_INFO "registered rfu entry - %s", RFU_PROCE_NAME);
        return 0;
}

__exit void rfu_exit(void)
{
        printk(KERN_INFO "rfu_exit(),ready to remove proc entry");
        proc_remove(rfu_proce);
        printk(KERN_INFO "removed proc entry - %s", RFU_PROCE_NAME);
}

module_init(rfu_init);
module_exit(rfu_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION("v0.1");
MODULE_AUTHOR("Zephyr");
MODULE_DESCRIPTION("Allow user write msg into proc file and printk msg.");


