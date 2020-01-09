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
#include <linux/ftrace.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/unistd.h>
#include <linux/wait.h>
MODULE_AUTHOR("ESME_3S3");
MODULE_LICENSE("Dual BSD/GPL");

#define fifoSize 52


static int fifo_major = 0; // MAJOR Number
static char fifoArray[fifoSize];
static int occupiedFifoSpace = 0;
struct semaphore orMutex;
struct semaphore rMutex;
struct semaphore wMutex;
struct semaphore rSem;
struct semaphore wSem;

static DECLARE_WAIT_QUEUE_HEAD(wqW);
//static DECLARE_WAIT_QUEUE_HEAD(wqW);

static char reader = 0, writer = 0;

int arrayLeftShift(int characterRead){
	memmove((void*)fifoArray, (const void*)&fifoArray[characterRead], (occupiedFifoSpace-characterRead)*sizeof(char));
	memset((void*)fifoArray+(occupiedFifoSpace-characterRead),0,characterRead);
	occupiedFifoSpace -= characterRead;
	return 0;
}
/*
void fifoWait (){

}
*/

ssize_t fifo_read(struct file *fp, char __user *uBuffer, size_t nbc, loff_t *pos){


        /**/
//        wait_event_interruptible(wqR, 1);

	if (down_interruptible(&rMutex)){return -ERESTARTSYS;}
	if (writer != 0){wake_up_interruptible(&wqW);}
	printk(KERN_DEBUG "WAKE");
	if (down_interruptible(&rSem)){return -ERESTARTSYS;}
	if (nbc <= occupiedFifoSpace){
		printk(KERN_DEBUG " TEST2 ");
               	if (copy_to_user((void * __user)uBuffer, (void *)fifoArray,nbc)){printk(KERN_DEBUG "ERROR copy_to_user");return -ENOMEM;}
		arrayLeftShift(nbc);
        }
	else if ( nbc > occupiedFifoSpace && nbc < fifoSize && occupiedFifoSpace != 0){
		if (copy_to_user((void * __user)uBuffer, (void *)fifoArray,occupiedFifoSpace)){printk(KERN_DEBUG "ERROR copy_to_user");return -ENOMEM;}
		arrayLeftShift(occupiedFifoSpace);
	}
	up(&wSem);
	up(&rMutex);
	printk("len uBuffer %ld", strlen(uBuffer));
	return strlen(uBuffer);

}

ssize_t fifo_write(struct file *fp, const char __user *uBuffer, size_t nbc, loff_t *pos){

	if (down_interruptible(&wMutex)){return -ERESTARTSYS;}
        wait_event_interruptible(wqW, reader);
	if ((occupiedFifoSpace + nbc)<= fifoSize){
		if (down_interruptible(&wSem)){return -ERESTARTSYS;}
		printk(KERN_DEBUG "WRITE LOCK");
		if (copy_from_user((void *)fifoArray+occupiedFifoSpace, (void * __user)uBuffer,nbc)){printk(KERN_DEBUG "ERROR copy_from_user");return -ENOMEM;}
		occupiedFifoSpace +=nbc;
		printk(KERN_DEBUG "WRITE UNLOCK");
		up(&rSem);
//		wake_up_interruptible(&wqR);
	}
	printk(KERN_DEBUG "NBC = %d",(int)nbc);
	printk(KERN_DEBUG "Recieved '%s', occupiedFifoSpace : %d", fifoArray,occupiedFifoSpace);
	up(&wMutex);
	return nbc;
}


int fifo_open(struct inode *inode, struct file *fp)
{
	if (down_interruptible(&orMutex)){return -ERESTARTSYS;}
	/*Only up to 5 interfaces are allowed*/
	if( iminor(fp->f_path.dentry->d_inode)> 4){return -ENODEV;}

	if( fp->f_mode & FMODE_READ ){
		reader++;
		printk(KERN_DEBUG "READER %d",reader); 
	}
	else if( fp->f_mode & FMODE_WRITE){
		writer++;
		printk(KERN_DEBUG "WRITER %d",writer);
	}
	else{
		up(&orMutex);
		/*Not a read or write action, exiting*/
		return -1;
	}
	up(&orMutex);
	return 0;
}

int fifo_release(struct inode *inode, struct file *fp)
{
	if (down_interruptible(&orMutex)){return -ERESTARTSYS;}
        if( fp->f_mode & FMODE_READ ){
                reader--;
        }
        else if( fp->f_mode & FMODE_WRITE){
                writer--;
        }

	if(reader == 0 && writer == 0){
		memset((void*)fifoArray,0,fifoSize);
		occupiedFifoSpace = 0;
		printk(KERN_DEBUG "FIFO cleared");
	}

	up(&orMutex);
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

	 /*Initialisation du mutex et semaphores*/
        sema_init(&rSem, 0);
        sema_init(&wSem, 1);
	sema_init(&orMutex, 1);
	sema_init(&rMutex, 1);
	sema_init(&wMutex, 1);

        result = register_chrdev(fifo_major, "fifo", &fifo_fops);
        if(result < 0){
                return result;
        }
        if(fifo_major == 0){
                fifo_major = result;
        }
	memset((void*)fifoArray,0,fifoSize);

        return 0;
}

// Fonction d'exit du fifo
void fifo_exit(void){
        unregister_chrdev(fifo_major, "fifo");
}



// Initialisation du module
module_init(fifo_init);
module_exit(fifo_exit);

