#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/printk.h>
#include <linux/preempt.h>
#include <linux/workqueue.h>
#include <linux/types.h>

/* we want the work function to be called once every
 * 500 times timer interrupt.
 */
#define DELAY_JIFFIES 500UL

void print_and_dumpstack(struct work_struct *);

static DECLARE_DELAYED_WORK(local_work, print_and_dumpstack);

/* we just print something and requeue delayed work. */
void print_and_dumpstack(struct work_struct *unused)
{
        printk(KERN_INFO "%s called function %s.",
               module_name(THIS_MODULE),
               __func__);
        dump_stack();
        
        /* we re-schedule a new delayed work */
        schedule_delayed_work(&local_work, DELAY_JIFFIES);
}
EXPORT_SYMBOL(print_and_dumpstack);

__init int tbkprobed_init(void)
{
        printk(KERN_INFO "%s is loaded.", module_name(THIS_MODULE));
        printk(KERN_INFO "current context is %s.",
               (in_interrupt()) ? "interrupt context" : "process context");

        if (!schedule_delayed_work(&local_work, DELAY_JIFFIES)) {
                printk(KERN_ERR "failed to schedule delayed work.");
                return -1;
        }

        printk(KERN_INFO "scheduled delayed work,delay is %u jiffies.",
               DELAY_JIFFIES);

        return 0;
}

__exit void tbkprobed_exit(void)
{
        if (!cancel_delayed_work_sync(&local_work))
                printk(KERN_ERR "failed to cancel the delayed work.");
}

MODULE_LICENSE("GPL v2");
MODULE_VERSION("v0.1");
MODULE_AUTHOR("Zephyr");
MODULE_DESCRIPTION("Module would be kprobed.");

module_init(tbkprobed_init);
module_exit(tbkprobed_exit);

