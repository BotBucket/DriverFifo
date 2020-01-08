
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NBL 70


int main (){
	static char alphabet[NBL];
int i,f, shift = NBL-1;
	/*Innit alphabet*/
        for(i = 0; i<NBL/2; i++){ alphabet[i] = 'a'+i; alphabet[(NBL/2)+i] = 'A'+i;}
	 for(i = 0; i<NBL; i++){ printf("%c",alphabet[i]);}
	printf("\n");
    	/*for (i = 0;  i < shift;  i++)
    	{
        	alphabet[i] = (alphabet[NBL-1-i] << shift);
		printf("%d : ", i);
		for(f = 0; f<NBL; f++){ printf("%c",alphabet[f]);}
		printf("\n");
    	}*/
//	for(i=shift; i < NBL; i++){
		memmove((void*)alphabet, (const void*)&alphabet[shift], (NBL-shift)*sizeof(char));
		memset((void*)alphabet+(NBL-shift),0,shift);
//	}
	for(i = 0; i<NBL; i++){ printf("%c",alphabet[i]);}
        printf("\n");

}
