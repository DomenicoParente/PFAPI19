#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define EBUG
#define MAX 20
#define XL 60
#define N 4
#define T 10

typedef struct{						// dichiarazione strutture
	char id_rel[MAX];
	char origin[MAX];
	struct relation *next_r;	
}relation;

typedef struct {
	char id_ent[MAX];
	relation *rel;
	struct entity *next_e;
}entity;

typedef struct{
	char type_rel[MAX];
	char max_dest[XL];
	int max;
} elenco_type;

entity *list_e;
elenco_type ntype[T];

void initiate(){					// inizializzazione strutture
	list_e= NULL;
	for(int i=0;i<T;i++){
		strcpy(ntype[i].type_rel,"null");
		ntype[i].max=0;
	}
}

int addent(char *name_e){
	entity *p;
	p=malloc(sizeof(entity));
	if(list_e == NULL){
		list_e=p;
		p->next_e= NULL;
	}
	else{
		p->next_e=list_e;
		list_e=p;
	}
	strcpy(p->id_ent,name_e);
	p->rel=NULL;
	return 0;
}
int delent(char *id_ent){
	/*void libera_lista(Lista_int l)
{
   if (l != NULL) {
    libera_lista(l->next);
    free(l);
   }
}*/
	return 0;
}
int addrel(char *id_orig, char *id_dest,char *t_rel){
	int j=0,k=0,i=0;
	entity *cursor;
	entity *dest_found;
	relation *p;
	if(list_e != NULL){						// verifica della presenza degli enti
		cursor= list_e;
		while(cursor != NULL && (j==0 || k==0)){
			if(strcmp(cursor->id_ent,id_orig)==0){
				j=1;
			}
			if(strcmp(cursor->id_ent,id_dest)==0){
				k=1;
				dest_found=cursor;
			}
		cursor=cursor->next_e;	
		}
	}
	if(j==0 || k==0){
		return 1;
	}
	p=malloc(sizeof(relation));
	if(dest_found->rel == NULL){			// inserimento della relazione nella lista
		dest_found->rel=p;
		p-next_r= NULL;
	}
	else{
		p-next_r=dest_found->rel;
		dest_found->rel=p;
	}
	strcpy(p->id_rel,t_rel);
	strcpy(p->origin,id_orig);
	while(strcmp(ntype[i].type_rel,"null")!=0 && strcmp(ntype[i].type_rel,id_rel)<0){
			i++;
		}
		strcpy(ntype[i].type_rel,id_rel);
		
	return 0;
}
int delrel(char *id_orig, char *id_dest, char *id_rel){
	return 0;
}
int report(){
	if(strcmp(ntype[0].type_rel,"null")==0){			// verifica se il numero di relazioni e' 0 
		printf("none\n");
	}
	return 0;
}

int main(){
	char inst[XL];
	char *arg[N];
	char *string;
	int i;
	char *c= NULL;
	initiate();							// inizializza strutture dati
	do{
		fgets(inst,XL,stdin);			// legge le istruzioni
		for(i=0;i<XL;i++){
			if(inst[i]=='\n'){
				inst[i]='\0';
				break;
			}
		}
		string=strtok(inst," ");			// separa gli argomenti
		i=0;
		arg[i]=string;
		i++;
		while(string != NULL && i<N){
			string=strtok(NULL," ");
			arg[i]=string;
			i++;
		}
			while(i<N){
				arg[i]=c;
				i++;
			}
		#ifdef DEBUG
			for(i=0;i<N;i++){
				printf("arg %d:|%s| ",i,arg[i]);
			}
		#endif
		
		if(strcmp(arg[0],"addrel")== 0){			// chiamata funzioni
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
