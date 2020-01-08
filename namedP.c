
/* exemple d'utilisation de la fct mmap */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

int main(int N, char *P[])
{
int f1, f2, i;
char *c,*p;
//char r[] = "Hello dear";

//	printf("%s\n",r);
     if (N != 2 ) {
       fprintf(stderr,"Utilisation : %s fichier_ou_device !\n",P[0]);
       exit(1);
     }
     if ((f1 = open(P[1],O_RDWR)) < 0) {
        perror("open 1"); exit(2);
     }
	if (write(f1,r,sizeof(r)) == -1) {
        perror("write"); exit(6);
     }

	
     	if (read(f1,c,10) == -1) {
        	perror("read"); exit(5);
     	}
	 printf("Les char lu sont %s \n", c);
     close(f1);
     return 0;
}


