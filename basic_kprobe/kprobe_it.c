#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/printk.h>
#include <linux/gfp.h>
#include <linux/slab.h>

static int kprobe_handler_func(struct kprobe *kb, struct pt_regs *regs)
{
        printk(KERN_INFO "kprobe hit,now print stack.");
        return 0;
}

#define KPROBE_TARGET_SYMBOL "print_and_dumpstack"
static struct kprobe *local_kb = NULL;

__init int kprobe_it_init(void)
{
        local_kb = kmalloc(sizeof(struct kprobe), GFP_KERNEL);
        if (!local_kb) {
                printk(KERN_ERR "%s: failed to allocate memory for kprobe object.",
                       module_name(THIS_MODULE));
                return -1;
        }

        local_kb->symbol_name = KPROBE_TARGET_SYMBOL;
        local_kb->pre_handler = kprobe_handler_func;
        local_kb->post_handler = NULL;
        local_kb->addr = NULL;
        local_kb->offset = 0;
        local_kb->flags = 0;

        /* register kprobe */
        if (register_kprobe(local_kb) < 0) {
                printk(KERN_ERR "%s: register kprobe failed.",
                       module_name(THIS_MODULE));
                goto bad_exit_free;
        }

        printk(KERN_INFO "%s: registered kprobe.", module_name(THIS_MODULE));
        return 0;

bad_exit_free:
        kfree(local_kb);
        return -1;
}

__exit void kprobe_it_exit(void)
{
        unregister_kprobe(local_kb);
        kfree(local_kb);
}

MODULE_LICENSE("GPL v2");
MODULE_VERSION("v0.1");
MODULE_AUTHOR("Zephyr");
MODULE_DESCRIPTION("This module kprobe the module to_be_kprobed.");

module_init(kprobe_it_init);
module_exit(kprobe_it_exit);
