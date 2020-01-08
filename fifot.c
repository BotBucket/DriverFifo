/*
* Réalisation d'un Driver de type 'car' auquel on a access
*   en lecture et en ecriture dans l'interface /dev
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/semaphore.h> 
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/ftrace.h>
#include <linux/sched.h>

MODULE_AUTHOR("ESME_3S3");
MODULE_LICENSE("Dual BSD/GPL");

#define fifoSize 52


static int fifo_major = 0; // MAJOR Number
static char fifoArray[fifoSize];
static int occupiedFifoSpace = 0;
struct semaphore Sem;

int arrayLeftShift(int characterRead){
	int i;
//	printk(KERN_DEBUG " %d %d",characterRead, occupiedFifoSpace);
//        for(i = 0; i<fifoSize; i++){ printk("%c",fifoArray[i]);}
	memmove((void*)fifoArray, (const void*)&fifoArray[characterRead], (occupiedFifoSpace-characterRead)*sizeof(char));
	memset((void*)fifoArray+(occupiedFifoSpace-characterRead),0,characterRead);
	occupiedFifoSpace -= characterRead;
	printk(KERN_DEBUG " TEST4 %d ",occupiedFifoSpace);
//        for(i = 0; i<fifoSize; i++){ printk("%c",fifoArray[i]);}
//	printk(KERN_DEBUG "\n");
	//TODO add error handling
	return 0;
}

ssize_t fifo_read(struct file *fp, char __user *uBuffer, size_t nbc, loff_t *pos){

        int minor,i=0;
        minor = iminor(fp->f_path.dentry->d_inode);
	printk(KERN_DEBUG " TEST1 ");
        //TODO
	for(i = 0; i<fifoSize; i++){ printk("%c",uBuffer[i]);}
        if (nbc <= occupiedFifoSpace){
		down_interruptible(&Sem);
		printk(KERN_DEBUG " TEST2 ");
               	if (copy_to_user((void * __user)uBuffer, (void *)fifoArray,nbc)){printk(KERN_DEBUG "ERROR copy_to_user");return -ENOMEM;}
		arrayLeftShift(nbc);
		up(&Sem);
//		for(i = 0; i<fifoSize; i++){ printk("%c",uBuffer[i]);}
        }
/*	else{
		i = nbc;
		printk(KERN_DEBUG " TEST3 ");
		while (i > 0){
			printk(KERN_DEBUG " TEST6 ");
			if (i >= occupiedFifoSpace && occupiedFifoSpace != 0){
				down_interruptible(&Sem);
				printk(KERN_DEBUG " TEST7 ");
	                	if (copy_to_user((void * __user)uBuffer, (void *)fifoArray,occupiedFifoSpace)){printk(KERN_DEBUG "ERROR copy_to_user");return -ENOMEM;}
	               		i-=occupiedFifoSpace;
				arrayLeftShift(occupiedFifoSpace);
				up(&Sem);
			}
			else if (i < occupiedFifoSpace){
				down_interruptible(&Sem);
		                printk(KERN_DEBUG " TEST5 ");
				if (copy_to_user((void * __user)uBuffer, (void *)fifoArray,i)){printk(KERN_DEBUG "ERROR copy_to_user");return -ENOMEM;}
                                arrayLeftShift(i);
				break;
				up(&Sem);
			}
			break;
		}
	}*/
	return i;

}

ssize_t fifo_write(struct file *fp, const char __user *uBuffer, size_t nbc, loff_t *pos){

        int minor,i;
        minor = iminor(fp->f_path.dentry->d_inode);
        //TODO
	if ((occupiedFifoSpace + nbc)<= fifoSize){
		down_interruptible(&Sem);
		printk(KERN_DEBUG "WRITE1");
		if (copy_from_user((void *)fifoArray+occupiedFifoSpace, (void * __user)uBuffer,nbc)){printk(KERN_DEBUG "ERROR copy_from_user");return -ENOMEM;}
	//	for (i =0; i<nbc; i++){
	//		if (copy_from_user((void *)fifoArray,(void * __user)uBuffer,1)){break;}
	//	}
		occupiedFifoSpace +=nbc;
		printk(KERN_DEBUG "WRITE2");
		up(&Sem);
	}
//	for(i = 0; i<fifoSize; i++){ printk("%c",fifoArray[i]);}
	printk(KERN_DEBUG "NBC = %d",(int)nbc);
	printk(KERN_DEBUG "Recieved '%s', occupiedFifoSpace : %d", fifoArray,occupiedFifoSpace);

	return i;
}


int fifo_open(struct inode *inode, struct file *fp)
{
    return 0;
}

int fifo_release(struct inode *inode, struct file *fp)
{
    return 0;
}

// Structure personnalisee des operations sur les fichiers
struct file_operations fifo_fops = {
        .owner = THIS_MODULE,
        .read = fifo_read,
        .write = fifo_write,
	.open = fifo_open,
	.release = fifo_release,
};


// Fonction d'init du fifo
int fifo_init(void){
        int result,i;
        result = register_chrdev(fifo_major, "fifo", &fifo_fops);
        if(result < 0){
                return result;
        }
        if(fifo_major == 0){
                fifo_major = result;
        }
//	for(i = 0; i<fifoSize/2; i++){ fifoArray[i] = 'a'+i; fifoArray[(fifoSize/2)+i] = 'A'+i;}
//	for(i = 0; i<fifoSize; i++){ printk("%c",fifoArray[i]);}
//	occupiedFifoSpace = fifoSize;
	memset((void*)fifoArray,0,fifoSize);

	 /*Initialisation du semaphore*/
        sema_init(&Sem, 1);
//	init_MUTEX(&Sem);
        return 0;
}

// Fonction d'exit du fifo
void fifo_exit(void){
        unregister_chrdev(fifo_major, "fifo");
}



// Initialisation du module
module_init(fifo_init);
module_exit(fifo_exit);

