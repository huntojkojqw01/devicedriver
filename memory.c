/* Necessary includes for device drivers */

#include <linux/init.h>

#include <linux/module.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h> /* file_operations */


MODULE_LICENSE("Dual BSD/GPL");



/* Declaration of memory.c functions */

int memory_open(struct inode *inode, struct file *filp);

int memory_release(struct inode *inode, struct file *filp);

ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);

ssize_t memory_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

static int memory_init(void) {

  printk(KERN_INFO "Hello: mem registered");

  return 0;

}



static void memory_exit(void) {

  printk(KERN_INFO "Goodbye: mem unregistered");

}

/* Structure that declares the usual file */

/* access functions */

struct file_operations memory_fops = {

  read: memory_read,

  write: memory_write,

  open: memory_open,

  release: memory_release

};



/* Declaration of the init and exit functions */

module_init(memory_init);

module_exit(memory_exit);



/* Global variables of the driver */

/* Major number */

int memory_major = 60;

/* Buffer to store data */

char *memory_buffer;