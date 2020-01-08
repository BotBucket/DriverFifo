/* exemple d'utilisation de la fct mmap */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

int main(int N, char *P[])
{
int f1, f2, i;
char c[22],*p;
char r[10] = "Hello_dear";
	memset(c, 0, sizeof(c));
	printf("Le message ecrit est %s, il fait %d char\n",r, sizeof(r));
     if (N != 2 ) {
       fprintf(stderr,"Utilisation : %s fichier_ou_device !\n",P[0]);
       exit(1);
     }
     if ((f1 = open(P[1],O_WRONLY)) < 0) {
        perror("open 1"); exit(2);
     }
	if (write(f1,r,sizeof(r)) == sizeof(r)) {
        perror("write"); exit(6);
     }
     close(f1);

     if ((f1 = open(P[1],O_RDONLY)) < 0) {
        perror("open 1"); exit(2);
     }

	if (read(f1,c,22) == -1) {
        	perror("read"); exit(5);
     	}
	printf("Les car lu sont %s\n", c);
	memset(c, 0, sizeof(c));
        close(f1);
/*
     if ((f1 = open(P[1],O_RDWR)) < 0) {
        perror("open 1"); exit(2);
     }
	if (read(f1,c,4) == -1) {
        	perror("read"); exit(5);
     	}
	printf("Les car lu sont %s\n", c);

     close(f1);
*/
     return 0;
}


