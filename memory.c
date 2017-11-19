/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h> /* file_operations */
#include <linux/slab.h> /* kmalloc() */


MODULE_LICENSE("Dual BSD/GPL");



/* Declaration of memory.c functions */

int memory_open(struct inode *inode, struct file *filp);

int memory_release(struct inode *inode, struct file *filp);

ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);

ssize_t memory_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

static int memory_init(void);

static void memory_exit(void);

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

static int  memory_init(void) {
  int result;

  /* Registering device */
  result = register_chrdev(memory_major, "memory", &memory_fops);

  if (result < 0) {
    printk(
      "<1>memory: cannot obtain major number %d\n", memory_major);
    return result;

  }
  /* Allocating memory for the buffer */

  memory_buffer = kmalloc(1, GFP_KERNEL); 

  if (!memory_buffer) { 

    result = -ENOMEM;

    goto fail; 

  } 
  memset(memory_buffer, 0, 1);
  printk("<1>Inserting memory module\n"); 
  return 0;



  fail: 
    memory_exit(); 
    return result;
}

static void memory_exit(void) {

  /* Freeing the major number */
  unregister_chrdev(memory_major, "memory");

  /* Freeing buffer memory */
  if (memory_buffer) {
    kfree(memory_buffer);
  }
  printk("<1>Removing memory module\n");
}

int memory_open(struct inode *inode, struct file *filp){return 0;}

int memory_release(struct inode *inode, struct file *filp){return 0;}

ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){return 0;}

ssize_t memory_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){return 0;}