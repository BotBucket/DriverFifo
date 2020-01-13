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
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/unistd.h>
#include <linux/wait.h>

MODULE_AUTHOR("ESME_3S3");
MODULE_LICENSE("Dual BSD/GPL");

#define fifoSize 52


static int fifo_major = 0; // MAJOR Number
static char fifoArray[fifoSize];
char * readPtr = fifoArray;
char * writePtr = fifoArray;
static int readPos = 0, writePos = 0;


static int occupiedFifoSpace = 0;
struct semaphore orMutex;
struct semaphore rMutex;
struct semaphore wMutex;

/*Wait queue for WRITE happening without a READ*/ 
static DECLARE_WAIT_QUEUE_HEAD(wqW);
/*Wait queue for READ happening without a WRITE*/ 
static DECLARE_WAIT_QUEUE_HEAD(rqW);

/*Variables counting the number of threads of each type*/ 
static char reader = 0, writer = 0;

/*
//Function used to shift to the left the elements of the array after READ
int arrayLeftShift(int characterRead){
	memmove((void*)fifoArray, (const void*)&fifoArray[characterRead], (occupiedFifoSpace-characterRead)*sizeof(char));
	memset((void*)fifoArray+(occupiedFifoSpace-characterRead),0,characterRead);
	occupiedFifoSpace -= characterRead;
	return 0;
}

//Functions to circularly READ/WRITE the array
void arrayReadWrite(char __user *kBuffer, char nRW){
char i;
static char readPos = 0, writePos = 0;
	if (nRW == 0){
		readPos = (readPos + fifoSize -1) % fifoSize;
		*kBuffer+i = *readPtr+readPos;
		occupiedFifoSpace--;
	}
	else if ( nRW == 1){
		writePos = (writePos + 1)%fifoSize;
		*writePtr+writePos = *kBuffer+i;
		occupiedFifoSpace++;
	}
}
*/
ssize_t fifo_read(struct file *fp, char __user *uBuffer, size_t nbc, loff_t *pos){
char i;
	/*NONBLOCKING operations*/
	if(fp->f_flags & O_NONBLOCK){

		/*If no writer or data to read are present exits*/
		if ((writer == 0) | (occupiedFifoSpace == 0)){return -EAGAIN;}

	        /*Tries to Lock READ mutex, exits on failure*/
		if (down_trylock(&rMutex)){return -ERESTARTSYS;}
	}

	else{
	        /*Locking READ mutex*/
		if (down_interruptible(&rMutex)){return -ERESTARTSYS;}
	}
	/*Enter READ wait queue if no writer is present*/
	wait_event_interruptible(rqW, writer);

	/*Wake up WRITE thread if present before a READ thread*/
//	if (writer != 0){wake_up_interruptible(&wqW);}
//Modif
	/*Lock READ after this one while the fifo hasn't been Written*/
//	if (down_interruptible(&rSem)){return -ERESTARTSYS;}
	if (strlen(fifoArray) == 0){
		up(&rMutex);
		printk("FIFO was empty, READ exited");
		return -1;
	}

	/*Lock READ after this one while the fifo hasn't been Written*/
//	if (down_interruptible(&rSem)){return -ERESTARTSYS;}
//Modif

	/*The 2 cases of READ*/
	if (nbc <= occupiedFifoSpace){
		for(i = 0; i < nbc; i++){
	               	if (copy_to_user((void * __user)uBuffer+i, (void *)fifoArray+readPos,1)){printk(KERN_DEBUG "ERROR copy_to_user");return -ENOMEM;}
			/*Shifting left READ data*/
	//		arrayLeftShift(nbc);
	//		printk ("uBuffer %s , readPos %d , fifoArray[readPos] %c, fifoArray %s",uBuffer,readPos,fifoArray[readPos], fifoArray );
//			fifoArray[readPos] = 0;
			memset((void*)fifoArray+readPos,0,1);

			readPos = (readPos + 1)%fifoSize;
//			readPos = (readPos + fifoSize -1) % fifoSize;
			printk ("uBuffer %s , readPos %d , fifoArray[readPos] %c, fifoArray %s",uBuffer,readPos,fifoArray[readPos], fifoArray );
			occupiedFifoSpace--;
		}
		printk("READ '%s' , FIFO '%s'", uBuffer, fifoArray);


        	/*Lock READ after this one while the fifo hasn't been Written*/
//      	if (down_interruptible(&rSem)){return -ERESTARTSYS;}
//		if (strlen(fifoArray) != 0){
//			if (down_interruptible(&rSem)){return -ERESTARTSYS;}
//			return -1;
//		}
		/*Releasing WRITE lock*/
//		up(&wSem);

		/*Releasing READ mutex*/
		up(&rMutex);
		printk(KERN_DEBUG "len uBuffer %ld", strlen(uBuffer));
		return strlen(uBuffer);
        }
	else if ( nbc > occupiedFifoSpace && nbc < fifoSize && occupiedFifoSpace != 0){

		for(i = 0; i < occupiedFifoSpace; i++){
	               	if (copy_to_user((void * __user)uBuffer+i, (void *)fifoArray+readPos,1)){printk(KERN_DEBUG "ERROR copy_to_user");return -ENOMEM;}
			/*Shifting left READ data*/
	//		arrayLeftShift(nbc);
//			fifoArray[readPos] = 0;
			memset((void*)fifoArray+readPos,0,1);
			readPos = (readPos + 1)%fifoSize;
//			readPos = (readPos + fifoSize -1) % fifoSize;
			occupiedFifoSpace--;
		}

		printk("READ '%s' , FIFO '%s'", uBuffer, fifoArray);

		/*Releasing WRITE lock*/
//		up(&wSem);

		/*Releasing READ mutex*/
		up(&rMutex);
		printk(KERN_DEBUG "len uBuffer %ld", strlen(uBuffer));
		return strlen(uBuffer);
	}

	/*ERROR handling*/
	else{

	/*Releasing WRITE lock*/
//	up(&wSem);

	/*Releasing READ mutex*/
        up(&rMutex);
	return -ENOMEM;
	}


}

ssize_t fifo_write(struct file *fp, const char __user *uBuffer, size_t nbc, loff_t *pos){
char i;
	if(fp->f_flags & O_NONBLOCK){

		/*If no reader are present exits*/
		if ( reader == 0){return -EAGAIN;}

		/*Tries locking WRITE mutex, exits on failure*/
		if (down_trylock(&wMutex)){return -ERESTARTSYS;}
	}
	else{
		/*Locking WRITE mutex*/
		if (down_interruptible(&wMutex)){return -ERESTARTSYS;}
	}
	/*Enter WRITE wait queue if no reader is present*/
	wait_event_interruptible(wqW, reader);

	/*WRITE case*/
	if ((occupiedFifoSpace + nbc)<= fifoSize){


		/*Lock WRITE after this one while the fifo hasn't been read*/
//		if (down_interruptible(&wSem)){return -ERESTARTSYS;}
		for (i = 0; i < nbc;i++){
			if (copy_from_user((void *)fifoArray+writePos, (void * __user)uBuffer+i,1)){printk(KERN_DEBUG "ERROR copy_from_user");return -ENOMEM;}
			writePos = (writePos + 1)%fifoSize;
			occupiedFifoSpace++;
		}
		/*Release READ lock*/
//		up(&rSem);

		printk(KERN_DEBUG "NBC = %d",(int)nbc);
		printk(KERN_DEBUG "Recieved '%s', occupiedFifoSpace : %d", fifoArray,occupiedFifoSpace);

		/*Release WRITE mutex*/
		up(&wMutex);
		return nbc;
	}

	/*ERROR Handling*/
	else{
		/*Release WRITE mutex*/
		up(&wMutex);
		return -ENOMEM;
	}

}


int fifo_open(struct inode *inode, struct file *fp)
{
	/*Locking OPEN/RELEASE mutex*/
	if (down_interruptible(&orMutex)){return -ERESTARTSYS;}

	/*Only allow up to 5 interfaces*/
	if( iminor(fp->f_path.dentry->d_inode)> 4){return -ENODEV;}

	/*Checks how the FIFO was opened and updates reader or writer accordingly*/
	if( fp->f_mode & FMODE_READ ){
		reader++;
		/*Wake up WRITE threads if present before a READ thread*/
		if (writer != 0 && reader == 1){wake_up_interruptible(&wqW);}
		printk(KERN_DEBUG "READER %d",reader); 
	}
	else if( fp->f_mode & FMODE_WRITE){
		writer++;
		/*Wake up READ threads if present before a WRITE thread*/
		if (reader != 0 && writer == 1){wake_up_interruptible(&rqW);}
		printk(KERN_DEBUG "WRITER %d",writer);
	}

	/*Handles unauthorized open type O_RDWR*/
	else{

		/*Release OPEN/RELEASE mutex*/
		up(&orMutex);

		/*Not a read or write action, exiting*/
		return -1;
	}

	/*Release OPEN/RELEASE mutex*/
	up(&orMutex);
	return 0;
}

int fifo_release(struct inode *inode, struct file *fp)
{

	/*Locking OPEN/RELEASE mutex*/
	if (down_interruptible(&orMutex)){return -ERESTARTSYS;}

	/*Remove reader/writer before deallocating the file pointer*/
        if( fp->f_mode & FMODE_READ ){
                reader--;
        }
        else if( fp->f_mode & FMODE_WRITE){
                writer--;
        }

	/*If none the the FIFO exits are opened, clears the FIFO */
	if(reader == 0 && writer == 0){
		memset((void*)fifoArray,0,fifoSize);
		occupiedFifoSpace = 0;
		printk(KERN_DEBUG "FIFO cleared");
	}

	/*Release OPEN/RELEASE mutex*/
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
        int result;

	 /*Initialisation des mutexs*/
	sema_init(&orMutex, 1);
	sema_init(&rMutex, 1);
	sema_init(&wMutex, 1);

	/*Registering the character device*/
        result = register_chrdev(fifo_major, "fifo", &fifo_fops);
        if(result < 0){
                return result;
        }
	/*Forcing KERNEL to attribute the MAJOR number*/
        if(fifo_major == 0){
                fifo_major = result;
        }
	/*Clearing allocated FIFO space*/
	memset((void*)fifoArray,0,fifoSize);

	return 0;
}

// Fonction d'exit du fifo
void fifo_exit(void){

	/*Unregistering the character device*/
        unregister_chrdev(fifo_major, "fifo");
}



// Initialisation du module
module_init(fifo_init);
module_exit(fifo_exit);

