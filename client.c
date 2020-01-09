#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

void send(char *P[]);
void receive(char *P[]);

int main(int N, char *P[])
{

	int f1, f2, i;
	char flag = 0;
	char c[22],*p,options;
	char r[10] = "Hello_dear";
	memset(c, 0, sizeof(c));
        if (N != 2 ) {
                fprintf(stderr,"Utilisation : %s fichier_ou_device !\n",P[0]);
                exit(1);
        }

	printf("\tOptions\n1 : Read information\n2 : Write information\n3 : Exit\n");

	while (flag == 0){
		printf("Choose an option : ");
		scanf("%d",&options);
		switch(options){
			case 1:
			receive(P);
			break;
			case 2:
			send(P);
			break;
			case 3:
			flag = 1;
			break;
		}
	}

	return 0;
}


void receive(char *P[]){

	char buffer[22],nbc;
	memset(buffer, 0 , sizeof(buffer));
	printf("Number of characters to read : ");
	scanf("%d", &nbc);
	int f1;
	if ((f1 = open(P[1],O_RDONLY|O_NONBLOCK)) < 0) {
                perror("open 1");
		exit(2);
        }

        if (read(f1,buffer,nbc) < 0 ) {
                perror("read");
		exit(5);
        }
        printf("Les car lu sont '%s'\n", buffer);
        close(f1);

}

void send(char *P[]){

        char buffer[22];
	int length;
        memset(buffer, 0 , sizeof(buffer));
	printf("Type to send : ");
	//scanf("%s", buffer);
	/*Clearing the newline from the previous scanf*/
	while ((getchar()) != '\n');
	fgets(buffer, 22, stdin);
	strtok(buffer,"\n");
	length = strlen(buffer);
	printf("\nLength of string %d\n", length);
	int f1;
        if ((f1 = open(P[1],O_WRONLY|O_NONBLOCK)) < 0) {
                perror("open 1");
		exit(2);
        }
        if (write(f1,buffer,length) != length) {
                perror("write");
		exit(6);
        }
        close(f1);

}
