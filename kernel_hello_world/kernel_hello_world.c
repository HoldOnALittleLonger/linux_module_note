#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/types.h>

__init int hello_world(void)
{
        printk(KERN_INFO "fedora module : Hello World!");
        return 0;
}

__exit void exit_module(void)
{
}

module_init(hello_world);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_VERSION("v0.1");
MODULE_AUTHOR("Zephyr");
MODULE_DESCRIPTION("Hello World module");


