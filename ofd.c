#include <linux/init.h>

#include <linux/module.h>

#include <linux/kernel.h>







static int hello_init(void) {

  printk(KERN_INFO "Hello: ofd registered");

  return 0;

}



static void hello_exit(void) {

  printk(KERN_INFO "Goodbye: ofd unregistered");

}



module_init(hello_init);

module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Dinh Hung");
MODULE_DESCRIPTION("Our First Driver");