#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG
#define MAX 20
#define XL 60
#define N 4

/*typedef struct{
	char id_relation[MAX];
	relation *next;	
}relation;

typedef struct {
	char id_entity[MAX];
	relation *rel;
}entity;*/

int addent(){
	return 0;
}
int delent(){
	return 0;
}
int addrel(){
	return 0;
}
int delrel(){
	return 0;
}
int report(){
	return 0;
}

int main(){
	char inst[XL];
	char *arg[5];
	char *string;
	int i;
	char *c= NULL;
	do{
		fgets(inst,XL,stdin);											//legge le istruzioni
		for(i=0;i<XL;i++){
			if(inst[i]=='\n'){
				inst[i]='\0';
				break;
			}
		}
		string=strtok(inst," ");										//separa gli argomenti
		i=0;
		arg[i]=string;
		while(string != NULL && i<4){
			i++;
			string=strtok(NULL," ");
			arg[i]=string;
		}
			while(i<4){
				arg[i]=c;
				i++;
			}
		#ifdef DEBUG
			for(i=0;i<N;i++){
				printf("arg %d:|%s| ",i,arg[i]);
			}
		#endif
		
		if(strcmp(arg[0],"addrel")== 0){								//chiamata funzioni
			addrel(arg[1],arg[2],arg[3]);			
		}
		if(strcmp(arg[0],"delrel")== 0){
			delrel(arg[1],arg[2],arg[3]);			
		}
		if(strcmp(arg[0],"addent")== 0){
			addent(arg[1]);			
		}
		if(strcmp(arg[0],"delent")== 0){
			delent(arg[1]);			
		}
		if(strcmp(arg[0],"report")== 0){
			report();			
		}
		
	}while(strcmp(arg[0],"end")!=0);
	return 0;
}
