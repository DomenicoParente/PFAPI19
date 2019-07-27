#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define DEBUG
#define MAX 20
#define XL 60
#define N 4

struct RE{						// dichiarazione strutture
	char id_rel[MAX];
	char origin[MAX];
	struct RE *next_r;	
};
typedef struct RE relation;

struct ENT {
	char id_ent[MAX];
	relation *rel;
	struct ENT *next_e;
};
typedef struct ENT entity;

struct ELENCO{
	char type_rel[MAX];
	char max_dest[XL];
	int max;
	int n_rel;
	struct ELENCO *next_t;
};
typedef struct ELENCO elenco_type;

entity *list_e;
elenco_type *ntype;
									// funzioni di supporto
									
void initiate(){					// inizializzazione strutture
	list_e= NULL;
	ntype= NULL;
}

void typecheck(){
	elenco_type *cursor,*pre,*temp;
	int i=0;
	cursor=ntype;
	pre=NULL;
	while(cursor != NULL){
		if(cursor->n_rel==0){
			if(pre==NULL){
				temp=cursor;
				ntype=temp->next_t;
				free(temp);
				cursor=ntype;
				i=1;
			}
			else{
				temp=cursor;
				pre->next_t=temp->next_t;
				free(temp);
				cursor=pre;
			}
		}
		if(i==0){
			pre=cursor;
			cursor=cursor->next_t;
		}
		i=0;
	}
}

void freelist(relation *rel_ent){
	elenco_type *cs;
	relation *l;
	l=rel_ent;
	int i;
	if (l != NULL) {
    freelist(l->next_r);
    cs=ntype;
    i=0;
	while(cs!= NULL && i==0){
		if(strcmp(l->id_rel,cs->type_rel)==0){
			cs->n_rel--;
			i=1;
		}
		cs=cs->next_t;
	}
    free(l);
   }
}
void searchdelete(char *target){
	entity *cursor1;
	relation *cursor2,*prev,*temp;
	elenco_type *cursor3;
	int i=0;
	cursor1=list_e;
	if(list_e ==NULL){
		return;
	}
	while(cursor1 != NULL){
		if(cursor1->rel == NULL){
			return;
		}
		cursor2=cursor1->rel;
		prev=NULL;
		while(cursor2 !=NULL){
			if(strcmp(target,cursor2->origin)==0){
				cursor3=ntype;
				while(cursor3!= NULL){
					if(strcmp(cursor3->type_rel,cursor2->id_rel)==0){
						cursor3->n_rel--;
						break;
					}
					cursor3=cursor3->next_t;
				}
				if(prev==NULL){
					temp=cursor2;
					cursor1->rel=temp->next_r;
					free(temp);
					cursor2=cursor1->rel;
					i=1;
				}
				else{
					temp=cursor2;
					prev->next_r=temp->next_r;
					free(temp);
					cursor2=prev;
				}
			}
			if(i==0){
				prev=cursor2;
				cursor2=cursor2->next_r;
			}
			i=0;
		}
		cursor1=cursor1->next_e;
	}
}

int addent(char *name_e){ 							// funzioni
	entity *q;
	entity *current,*past;
	current= list_e;
	past= NULL;
	while(current !=NULL && strcmp(current->id_ent,name_e)<0){	// verifica della presenza dell'ente
		past=current;
		current=current->next_e;
	}
	if(current!=NULL && strcmp(current->id_ent,name_e)==0){
			return 1;
	}
	q=malloc(sizeof(entity));						// inserimento nuovo ente
		q->next_e=current;
		if (past != NULL){
			past->next_e=q;
		}
		else{
			 list_e= q;
		 } 
		strcpy(q->id_ent,name_e);
		q->rel=NULL;
	#ifdef DEBUG
		current= list_e;
		while(current !=NULL){
				printf("%s\n",current->id_ent);
				current=current->next_e;
		}
	
	#endif
	return 0;
}
int delent(char *name_e){
	int i=0;
	entity *cursor,*past,*temp;
	cursor= list_e;
	past= NULL;
	while(cursor !=NULL && i==0){					// verifica della presenza dell'ente
		if(strcmp(cursor->id_ent,name_e)==0){
			i=1;
		}
		if(i==0){
			past=cursor;
			cursor=cursor->next_e;
		}
	}
	if(i==0 || list_e== NULL){
		return 1;
	}
	if(cursor->rel != NULL){					// eliminazione ente
		temp=cursor;
		freelist(temp->rel);
		cursor->rel=NULL;
	}
	if(past== NULL){
			list_e=cursor->next_e;
			free(cursor);
		}
	else{							
		past->next_e=cursor->next_e;
		free(cursor);
	}
	searchdelete(name_e);
	typecheck();
	#ifdef DEBUG
		cursor= list_e;
		while(cursor !=NULL){
				printf("%s\n",cursor->id_ent);
				cursor=cursor->next_e;
		}
	elenco_type *current;
		current=ntype;
		while(current !=NULL){
				printf("|%s %d|\n",current->type_rel,current->n_rel);
				current=current->next_t;
		}
	#endif
	return 0;
}
int addrel(char *id_orig, char *id_dest,char *t_rel){
	int j=0,k=0;
	entity *cursor,*dest_found;
	relation *p,*f;
	elenco_type *current,*past,*r;						
	cursor= list_e;									// verifica della presenza degli enti
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
	if(j==0 || k==0){
		return 1;
	}
	f=dest_found->rel;							// verifica esistenza della relazione
	while(f !=NULL){
		if(strcmp(f->id_rel,t_rel)==0 && strcmp(f->origin,id_orig)==0){
			return 1;
		}
		f= f->next_r;
	}
	p=malloc(sizeof(relation));			// inserimento della relazione nella lista
	p->next_r=dest_found->rel;
	dest_found->rel=p;
	strcpy(p->id_rel,t_rel);
	strcpy(p->origin,id_orig);
	
	current=ntype; 								 		// verifica e inserimento tipo relazione 
	past= NULL;
	while(current !=NULL && strcmp(current->type_rel,t_rel)<0){
			past=current;
			current=current->next_t;
		}
	if(current!= NULL && strcmp(current->type_rel,t_rel)== 0){
		current->n_rel++;
	}
	else if(current == NULL || strcmp(current->type_rel,t_rel)!= 0){
		r=malloc(sizeof(elenco_type));
		r->next_t=current;
		if (past != NULL){
			past->next_t=r;
		}
		else{
			ntype=r;
		}
		strcpy(r->type_rel,t_rel);
		r->max=0;
		r->n_rel=1;
	}
	#ifdef DEBUG
		f=dest_found->rel;
		while(f !=NULL){
				printf("<%s %s %s>\n",f->id_rel,f->origin,dest_found->id_ent);
				f=f->next_r;
		}
		current=ntype;
		while(current !=NULL){
				printf("|%s %d|\n",current->type_rel,current->n_rel);
				current=current->next_t;
		}
	#endif
	return 0;
}
int delrel(char *id_orig, char *id_dest, char *t_rel){
	int j=0;
	entity *cursor1;
	relation *cursor2,*past;
	elenco_type *cursor3;
	if(list_e != NULL){						// verifica della presenza della relazione
		cursor1= list_e;
		while(cursor1 != NULL && j==0){
			if(strcmp(cursor1->id_ent,id_dest)==0){
				j=1;
			}
			if(j==0){
				cursor1=cursor1->next_e;
			}
		}
		if(j==0 || cursor1 == NULL){
			return 1;
		}
		else{									
			cursor2=cursor1->rel;
			past=NULL;
			j=0;
			while(cursor2 != NULL && j==0){
				if(strcmp(cursor2->origin,id_orig)==0 && strcmp(cursor2->id_rel,t_rel)==0){
					j=1;
				}
				if(j==0){
					past=cursor2;
					cursor2=cursor2->next_r;
				}
			}
			if(j==0){
				return 1;
			}
		}
	}
	else{
		return 1;
	}
	cursor3=ntype;
	while(cursor3!= NULL){
		if(strcmp(cursor3->type_rel,cursor2->id_rel)==0){
			cursor3->n_rel--;
			break;
		}
		cursor3=cursor3->next_t;
	}
	if(past == NULL){
		cursor1->rel=cursor2->next_r;
		free(cursor2);
	}
	else{
		past->next_r=cursor2->next_r;						// eliminazione relazione
		free(cursor2);
	}
	typecheck();
	#ifdef DEBUG
	relation *f;
	elenco_type *current;
		f=cursor1->rel;
		while(f !=NULL){
				printf("<%s %s %s>\n",f->id_rel,f->origin,cursor1->id_ent);
				f=f->next_r;
		}
		current=ntype;
		while(current !=NULL){
				printf("|%s %d|\n",current->type_rel,current->n_rel);
				current=current->next_t;
		}
	#endif
	return 0;
}
int report(){
	elenco_type *cursor;
	entity *cs1;
	relation *cs2;
	int i;
	cursor=ntype;
	if(ntype==NULL){					// verifica se il numero di relazioni e' 0 
		printf("none\n");
		return 0;
	}
	while(cursor != NULL){
		cs1=list_e;
		while(cs1 != NULL){
			cs2=cs1->rel;
			i=0;
			while(cs2 != NULL){
				if(strcmp(cs2->id_rel,cursor->type_rel)==0){
					i++;
				}
				printf("Signal report");
				cs2=cs2->next_r;
			}
			if(i>cursor->max){
				cursor->max=i;
				strcpy(cursor->max_dest,cs1->id_ent);
			}
			else if (i==cursor->max){
				strcat(cursor->max_dest," ");
				strcat(cursor->max_dest,cs1->id_ent);
			}
			cs1=cs1->next_e;
		}
		cursor=cursor->next_t;
	}
	cursor=ntype;
	while(cursor != NULL){				//stampa report
		printf("%s %s %d;",cursor->type_rel,cursor->max_dest,cursor->max);
		cursor=cursor->next_t;
	}
	printf("\n");
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
			printf("\n");
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
