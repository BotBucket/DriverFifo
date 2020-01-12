#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

void send_B(char *P[]);
void send_NB(char *P[]);
void receive_B(char *P[]);
void receive_NB(char *P[]);

int main(int N, char *P[])
{

	int f1, f2, i;
	char flag = 0;
	char c[22],*p, options;
	memset(c, 0, sizeof(c));
        if (N != 2 ) {
                fprintf(stderr,"Utilisation : %s fichier_ou_device !\n",P[0]);
                exit(1);
        }

	while (flag == 0){
		printf("\n");
		printf("+------------------------------------+\n");
		printf("|              Options               |\n");
		printf("+------------------------------------+\n");
		printf("| 1 : Read information  (NonBlock)   |\n");
		printf("| 2 : Write information (NonBlock)   |\n");
		printf("| 3 : Read information  (Block)      |\n");
		printf("| 4 : Write information (Block)      |\n");
		printf("| 0 : Exit                           |\n");
		printf("+------------------------------------+\n");

		step_1:
			printf("Choose an option : ");
			goto step_2;

		step_2:
		scanf("%c",&options);
//		printf("DEBUG OPTIONS = %c\n", options);
		switch(options){
			case '1':
				receive_NB(P);
				break;
			case '2':
				send_NB(P);
				break;
			case '3':
				receive_B(P);
				break;
			case '4':
				send_B(P);
				break;
			case '0':
				flag = 1;
				break;
			default :
				goto step_1;
				break;
		}//END SWITCH
	}//END WHILE
	return 0;
}


void receive_NB(char *P[]){

	char buffer[22],nbc;
	memset(buffer, 0 , sizeof(buffer));
	printf("\n\n");
	printf("******************************\n");
	printf("*****  READ (NON_BLOCK)  *****\n");
	printf("******************************\n");
	printf("Enter the number of characters you want to read : ");
	scanf("%d", &nbc);
	int f1;
	if ((f1 = open(P[1],O_RDONLY|O_NONBLOCK)) < 0) {
                perror("open 1");
//		exit(2);
        }

        if (read(f1,buffer,nbc) < 0 ) {
                perror("read");
//		exit(5);
        }
        printf("FIFO say : \"%s\"\n\n", buffer);
	printf("\n********** ********** ********** ********** **********\n\n");
        close(f1);

}

void send_NB(char *P[]){

        char buffer[22];
	int length;
        memset(buffer, 0 , sizeof(buffer));
	printf("\n");
	printf("*******************************\n");
	printf("*****  WRITE (NON_BLOCK)  *****\n");
	printf("*******************************\n");
	printf("Enter the message you want to send : ");
	//scanf("%s", buffer);
	/*Clearing the newline from the previous scanf*/
	while ((getchar()) != '\n');
	fgets(buffer, 22, stdin);
	strtok(buffer,"\n");
	length = strlen(buffer);
	printf("Length of string = %d\n\n", length);
	int f1;
        if ((f1 = open(P[1],O_WRONLY|O_NONBLOCK)) < 0) {
                perror("open 1");
//		exit(2);
        }
        if (write(f1,buffer,length) != length) {
                perror("write");
//		exit(6);
        }
	printf("\n********** ********** ********** ********** **********\n\n");
        close(f1);

}

////////////////////////////////////////////////////////////////////////////////////
void receive_B(char *P[]){

	char buffer[22],nbc;
	memset(buffer, 0 , sizeof(buffer));
	printf("\n\n");
	printf("**************************\n");
	printf("*****  READ (BLOCK)  *****\n");
	printf("**************************\n");
	printf("Enter the number of characters you want to read : ");
	scanf("%d", &nbc);
	int f1;
	if ((f1 = open(P[1],O_RDONLY)) < 0) {
                perror("open 1");
//		exit(2);
        }

        if (read(f1,buffer,nbc) < 0 ) {
                perror("read");
//		exit(5);
        }
        printf("FIFO say : \"%s\"\n\n", buffer);
	printf("\n********** ********** ********** ********** **********\n\n");
        close(f1);

}

void send_B(char *P[]){

        char buffer[22];
	int length;
        memset(buffer, 0 , sizeof(buffer));
	printf("\n");
	printf("***************************\n");
	printf("*****  WRITE (BLOCK)  *****\n");
	printf("***************************\n");
	printf("Enter the message you want to send : ");
	//scanf("%s", buffer);
	/*Clearing the newline from the previous scanf*/
	while ((getchar()) != '\n');
	fgets(buffer, 22, stdin);
	strtok(buffer,"\n");
	length = strlen(buffer);
	printf("Length of string = %d\n\n", length);
	int f1;
        if ((f1 = open(P[1],O_WRONLY)) < 0) {
                perror("open 1");
//		exit(2);
        }
        if (write(f1,buffer,length) != length) {
                perror("write");
//		exit(6);
        }
	printf("\n********** ********** ********** ********** **********\n\n");
        close(f1);

}
