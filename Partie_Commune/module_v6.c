/*
* Exemple de fabrication d'un compteur via l'interface /proc
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/version.h>

MODULE_AUTHOR("ESME_3S3");
MODULE_LICENSE("Dual BSD/GPL");



// -------------------------------------------
// | Declaration des fonctions sequentielles |
// -------------------------------------------
// COMPTEUR_SEQ_START
static void* compteur_seq_start(struct seq_file *s, loff_t *p){
	loff_t *sp = kmalloc(sizeof(loff_t), GFP_KERNEL);
	if(!sp){
		return NULL;
	}
	*sp = *p;
	return (sp);
}

// COMPTEUR_SEQ_STOP //
static void compteur_seq_stop(struct seq_file *s, void *v){
	kfree(v);
}

// COMPTEUR_SEQ_NEXT //
static void* compteur_seq_next(struct seq_file *s, void *v, loff_t *p){
	loff_t *sp = (loff_t *)v;
	*p = ++ (*sp);
	return sp;
}

// COMPTEUR_SEQ_SHOW //
static int compteur_seq_show(struct seq_file *s, void *v){
	loff_t *sp = (loff_t *)v;
	seq_printf(s, "%Ld\n", *sp);
	return 0;
}

// ---------------------------------------
// | Declaration des fonctions speciales |
// ---------------------------------------
static struct seq_operations compteur_seq_ops = {
	.start = compteur_seq_start,
	.next = compteur_seq_next,
	.stop = compteur_seq_stop,
	.show = compteur_seq_show
};

// COMPTEUR_OPEN
static int compteur_open(struct inode *inode, struct file *file){
	return seq_open(file, &compteur_seq_ops);
}

// On utilise les fonctions standards sequentielles seq_
static struct file_operations compteur_file_ops = {
	.owner = THIS_MODULE,
	.open = compteur_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release
};
// Fonction d'init du compteur
static int compteur_init(void){
	struct proc_dir_entry *entree;

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,16,0)
	entree = proc_create("my_counter", 0, NULL, &compteur_file_ops);
#else
	entree = create_proc_entry("my_counter", 0, NULL);
	if(entree){ //Affectation de la table des operations
		entree->proc_fops = &compteur_file_ops;
	}
#endif

	return 0;
}

// Fonction d'exit du compteur
static void compteur_exit(void){
	remove_proc_entry("my_counter", NULL);
}

// Initialisation du module
module_init(compteur_init);
module_exit(compteur_exit);
