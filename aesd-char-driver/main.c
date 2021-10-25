/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include <linux/slab.h>
#include <linux/string.h>
#include "aesdchar.h"

int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("Rishab Shah"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
	struct aesd_dev *dev;
	PDEBUG("open");
	/**
	 * TODO: handle open
	 */
  //scull code reference

	dev = container_of(inode->i_cdev, struct aesd_dev, cdev);
	
	//will point to aesd_dev struct
	filp->private_data = dev;

	return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
	PDEBUG("release");
	/**
	 * TODO: handle release
	 */
	 
	return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t retval = 0;
	//struct aesd_dev *dev = filp->private_data;
	
	PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
	/**
	 * TODO: handle read
	 */
	 

	 
	return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t retval = -ENOMEM;
	size_t no_of_bytes_not_copied = 0;

	/**
	 * TODO: handle write
	 */
	char* check_newline = NULL;   //will change when \n is detected
	struct aesd_dev *l_dev = NULL;
	const char *data_overflow = NULL;
	
	//fetch the content received from the script
	l_dev = (struct aesd_dev *)(filp->private_data);
	
	PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
	
	//allocate memory as big as count, when the memory is initially empty
	if(l_dev->buffer_entry.size == 0)
	{
      l_dev->buffer_entry.buffptr = kmalloc(count*sizeof(char),GFP_KERNEL);
      
      if(l_dev->buffer_entry.buffptr == NULL)
      {
        //error in allocation
        return retval;
      }
	}
	else
	{
	  //\n wa not detected previously. Hence, a realloc till \n is detected
	  l_dev->buffer_entry.buffptr = krealloc(l_dev->buffer_entry.buffptr, (l_dev->buffer_entry.size + count), GFP_KERNEL);
	  
	  if(l_dev->buffer_entry.buffptr == NULL)
	  {
	     return retval;
	  }
	}
	
	//update content .buffptr
	no_of_bytes_not_copied = copy_from_user((void *)(&l_dev->buffer_entry.buffptr[l_dev->buffer_entry.size]),buf,count);
	if(no_of_bytes_not_copied != 0)
	{
	  PDEBUG("No of bytes failed to copy to kernel space is %ld",no_of_bytes_not_copied);
	}
	
	//update with actual bytes copied subtracting no of bytes faie to copy as well
	retval = count - no_of_bytes_not_copied;
	
	//data copied till now
	l_dev->buffer_entry.size = l_dev->buffer_entry.size + retval;
	
	check_newline = strchr(l_dev->buffer_entry.buffptr,'\n');
	if(check_newline != NULL)
	{
	  //void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
	  //write to the buffer
	  data_overflow = aesd_circular_buffer_add_entry(&l_dev->circ_buffer,&l_dev->buffer_entry);	// two params:  1.content 2.size
	    
	  if(data_overflow != NULL)
	  {
	    kfree(data_overflow);
	  }
	  
	  //reset everything as data is written to one of the circular buffer
	  l_dev->buffer_entry.buffptr = NULL; 
	  l_dev->buffer_entry.size = 0; 
	  
	  //reset the system for new \n
	  check_newline = NULL;
	}
	
	return retval;
}

struct file_operations aesd_fops = {
	.owner   =  THIS_MODULE,
	.read    =  aesd_read,
	.write   =  aesd_write,
	.open    =  aesd_open,
	.release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
	int err, devno = MKDEV(aesd_major, aesd_minor);

	cdev_init(&dev->cdev, &aesd_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &aesd_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	if (err) {
		printk(KERN_ERR "Error %d adding aesd cdev", err);
	}
	return err;
}

int aesd_init_module(void)
{
	dev_t dev = 0;
	int result;
	result = alloc_chrdev_region(&dev, aesd_minor, 1,"aesdchar");
	aesd_major = MAJOR(dev);
	if (result < 0) {
		printk(KERN_WARNING "Can't get major %d\n", aesd_major);
		return result;
	}
	
	//buffer initilaized
	memset(&aesd_device,0,sizeof(struct aesd_dev));

	/**
	 * TODO: initialize the AESD specific portion of the device
	 */
	 //allocate memory, init lock, init the circular buffer
	//aesd_circular_buffer_init(&aesd_device.circ_buffer);
	 
	result = aesd_setup_cdev(&aesd_device);

	if( result ) {
		unregister_chrdev_region(dev, 1);
	}
	return result;

}

void aesd_cleanup_module(void)
{
	dev_t devno = MKDEV(aesd_major, aesd_minor);

	cdev_del(&aesd_device.cdev);

	/**
	 * TODO: cleanup AESD specific poritions here as necessary
	 */
	//free memory, release lock, circular buffer related handling

	unregister_chrdev_region(devno, 1);
}


module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
