#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/printk.h>

static char *charp_param = NULL;
module_param(charp_param, charp, 0444);
static int int_param = 0;
module_param(int_param, int, 0444);

__init int kmod_params_init(void)
{
        printk(KERN_INFO "charp parameter is %s .", charp_param);
        printk(KERN_INFO "int parameter is %d .", int_param);

        size_t size_of_folio = sizeof(struct folio);
        size_t size_of_page = sizeof(struct page);

        printk(KERN_INFO "size of folio object is %u .", size_of_folio);
        printk(KERN_INFO "size of page object is %u .", size_of_page);

        return 0;
}

__exit void kmod_params_exit(void)
{
}

MODULE_LICENSE("GPL v2");
MODULE_VERSION("v0.1");
MODULE_AUTHOR("Zephyr");
MODULE_DESCRIPTION("Just test parameters,and print size of folio and page.");


module_init(kmod_params_init);
module_exit(kmod_params_exit);
