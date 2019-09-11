#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//#define DEBUG
#define HASHSIZE 300
#define HASHSIZE_REL 50
#define MAX 40
#define XL 50
#define N 4
									// STRUTTURE DATI	
struct REL{
	struct REL *next;
	struct REL *prev;
	struct RELTABLE *rel;
	struct RELTABLE **root;
};
typedef struct REL relation;

struct TREL{
	char rel_t[MAX];
	int nrel;
	struct TREL *next_rt;
};
typedef struct TREL trelation;

struct RELTABLE{
	char id_r[XL];
	trelation *rdest;
	relation *ent;
	relation **first;
	struct ELENCO *tty;
	struct RELTABLE *next;
	struct RELTABLE	*prev;
};
typedef struct RELTABLE trb;
typedef trb *trbp;

struct ENT {
	char id_ent[MAX];
	trelation *t_rel;
	relation *rlist;
	trbp *rel_table;
	struct ENT *next_e;
};
typedef struct ENT entity;

entity *ent_table[HASHSIZE];		// tabella hash per le entità

struct M {
	char max_dest[MAX];
	struct M *next_m;
};
typedef struct M max_d;

struct ELENCO{
	char type_rel[MAX];
	max_d *dest;
	int max;
	int n_rel;
	struct ELENCO *next_t;
};
typedef struct ELENCO elenco_type;
elenco_type *ntype;						// lista tipi relazioni

bool enable= false;

void initiate(){						// FUNZIONI DI SUPPORTO
	int i;
	for(i=0; i<HASHSIZE; i++){
		ent_table[i]=NULL; 
	}
	ntype= NULL;
}

int hashint(char *name_e){
	unsigned long int hash=0;
    unsigned int c,i;
    c=strlen(name_e);
    for(i=0;i<c;i++){
		hash=(hash*5381)+(int)name_e[i];
	}
	return hash%HASHSIZE;
}

int hashintrel(char *name_e){
	unsigned long int hash=0;
    unsigned int c,i;
    c=strlen(name_e);
    for(i=0;i<c;i++){
		hash=(hash*5381)+(int)name_e[i];
	}
	return hash%HASHSIZE_REL;
}

void freetrel(trb *start){
	trb *l;
	relation *temp;
	l=start;
	if(l != NULL){
		freetrel(l->next);
		l->rdest->nrel--;
		l->tty->n_rel--;
		if(l->ent->prev==NULL){
			*l->first=l->ent->next;
			temp=l->ent->next;
			if(temp!=NULL)
				temp->prev=NULL;
		}
		else{
			temp=l->ent->prev;
			temp->next=l->ent->next;
			temp=l->ent->next;
			if(temp!=NULL)
				temp->prev=l->ent->prev;
		}
		free(l->ent);
		l->ent=NULL;
		free(l);
	}
}
void freelistrel(relation *rel){
	relation *l;
	trb *temp;
	l=rel;
	if (l != NULL) {
		freelistrel(l->next);
			#ifdef DEBUG
			printf("Eliminato: %s\n",l->rel->id_r);
			#endif
			if(l->rel->prev==NULL){
				*l->root=l->rel->next;
				if(l->rel->next!=NULL)
					l->rel->next->prev=NULL;
			}
			else{
				temp=l->rel->prev;
				temp->next=l->rel->next;
				temp=l->rel->next;
				if(temp!=NULL)
					temp->prev=l->rel->prev;
			}
		free(l->rel);	
		free(l);
   }
}

void freelist(trelation *rel_ent){
	trelation *l;
	l=rel_ent;
	if (l != NULL) {
		freelist(l->next_rt);
		free(l);
   }
}

void freemax(max_d *max){
	max_d *l;
	l=max;;
	if (l != NULL) {
		freemax(l->next_m);
		free(l);
   }
}

void typecheck(){
	elenco_type *cursor,*pre,*temp;
	bool i=false;
    cursor=ntype;
	pre=NULL;
	while(cursor != NULL){
		i=false;
		cursor->max=0;
		if(cursor->n_rel<=0){
			if(cursor->dest != NULL){
				freemax(cursor->dest);
				cursor->dest=NULL;
			}
			if(pre==NULL){
				temp=cursor;
				ntype=cursor->next_t;
				free(temp);
				cursor=ntype;
				i=true;
			}
			else{
				temp=cursor;
				pre->next_t=cursor->next_t;
				free(temp);
				cursor=pre;
			}
		}
		if(i==false){
			pre=cursor;
			cursor=cursor->next_t;
		}
	}
}

int addent(char *name_e){ 							// FUNZIONI
	int k;
	entity *cursor,*p;
	k=hashint(name_e);						// verifica presenza ente
	cursor= ent_table[k];
	while(cursor != NULL){
		if(strcmp(cursor->id_ent,name_e)==0){
			return 1;
		}
		cursor=cursor->next_e;
	}
	p=malloc(sizeof(entity));			// inserimento nuovo ente
	strcpy(p->id_ent,name_e);
	p->t_rel=NULL;
	p->rlist=NULL;
	p->rel_table=NULL;
	p->next_e=ent_table[k];
	ent_table[k]=p;		
	#ifdef DEBUG
		int q;
		printf("Valore hash: %ld \n",k);
		for(q=0;q<HASHSIZE;q++){
			if(ent_table[q]!= NULL){
				cursor=ent_table[q];
				while(cursor !=NULL){
					printf("%s\n",cursor->id_ent);
					cursor=cursor->next_e;
				}
			}
		}
	#endif	
	return 0;
}

int addrel(char *id_orig, char *id_dest,char *t_rel){
	int k,j;
	trelation *p,*cs;
	elenco_type *current,*past,*r;
	trb *csr,*x;
	trbp *relt;
	relation *s;
	max_d *cs3,*pre,*q;
	bool f=false;
	entity *cursor1,*cursor2;
	char id_rel[XL];
	strcpy(id_rel,id_dest);
	strcat(id_rel,t_rel);
	#ifdef DEBUG
		printf("ID_REL: %s\n",id_rel);
	#endif
	k=hashint(id_orig);						// verifica presenza enti
	j=hashint(id_dest);
	if(ent_table[k]== NULL || ent_table[j]== NULL){
		return 1;
	}
	cursor1=ent_table[k];
	while(cursor1 != NULL && f== false){
		if(strcmp(cursor1->id_ent,id_orig)==0){
			f=true;
		}
		if(f==false){
			cursor1=cursor1->next_e;
		}
	}
	if(f==false){
		return 1;
	}
	f=false;
	cursor2=ent_table[j];
	while(cursor2 != NULL && f==false){
		if(strcmp(cursor2->id_ent,id_dest)==0){
			f=true;
		}
		if(f==false){
			cursor2=cursor2->next_e;
		}
	}
	if(f==false){
		return 1;
	}
	cs=cursor2->t_rel;
	k=hashintrel(id_rel);							// cerca se la relazione già esiste
	#ifdef DEBUG
		printf("HASH VALUE: %ld\n",k);
	#endif
	if(cursor1->rel_table!=NULL){
		if(cursor1->rel_table[k]!=NULL){
			csr=cursor1->rel_table[k];
			while(csr != NULL){
				if(strcmp(csr->id_r,id_rel)==0){
					return 1;
				}
				csr=csr->next;
			}
		}
	}
	else{
		relt=malloc(HASHSIZE_REL*(sizeof(trbp)));		// inserimento tabella relazioni
		cursor1->rel_table=relt;
		for(int l=0;l<HASHSIZE_REL;l++){
			cursor1->rel_table[l]=NULL;
		}
	}
	f=false;
	while(cs!= NULL && f==false){
		if(strcmp(cs->rel_t,t_rel)==0){
			f=true;
		}
		if(f==false){
			cs=cs->next_rt;
		}
	}
	if(f==false){						// aggiunto elemento tipo relazione
		p=malloc(sizeof(trelation));
		p->next_rt=cursor2->t_rel;
		cursor2->t_rel=p;
		strcpy(p->rel_t,t_rel);
		#ifdef DEBUG
			printf("Type: %s\n",p->rel_t);
		#endif
		p->nrel=1;	
		cs=p;
	}
	else{
		cs->nrel++;
	}
	s=malloc(sizeof(relation));
	s->next=cursor2->rlist;
	s->prev=NULL;
	if(cursor2->rlist !=NULL){
		cursor2->rlist->prev=s;
	}
	cursor2->rlist=s;
	x=malloc(sizeof(trb));
	x->next=cursor1->rel_table[k];
	x->prev=NULL;
	if(cursor1->rel_table[k] !=NULL){
		cursor1->rel_table[k]->prev=x;
	}
	cursor1->rel_table[k]=x;
	s->rel=x;
	x->ent=s;
	x->rdest=cs;
	strcpy(x->id_r,id_rel);
	x->first=&cursor2->rlist;
	s->root=&cursor1->rel_table[k];
	current=ntype; 								 		// verifica e inserimento tipo relazione 
	past= NULL;
	while(current !=NULL && strcmp(current->type_rel,t_rel)<0){
		past=current;
		current=current->next_t;
	}
	if(current!= NULL && strcmp(current->type_rel,t_rel)== 0){
		current->n_rel++;
		x->tty=current;
		if(enable==false){
			if(cs->nrel> current->max){
				current->max=cs->nrel;
				freemax(current->dest);
				current->dest=NULL;
			}
			if(cs->nrel >= current->max && cs->nrel!=0){
				cs3=current->dest;
				pre=NULL;
				while(cs3 !=NULL && strcmp(cs3->max_dest,id_dest)<0){
					pre=cs3;
					cs3=cs3->next_m;
				}
				if(cs3== NULL || strcmp(cs3->max_dest,id_dest)!=0){
					q=malloc(sizeof(max_d));
					q->next_m=cs3;
					if (pre != NULL){
						pre->next_m=q;
					}
					else{
						current->dest= q;
					} 
					strcpy(q->max_dest,id_dest);
				}
			}
		}
	}
	else if(current == NULL || strcmp(current->type_rel,t_rel)!= 0){
		r=malloc(sizeof(elenco_type));
		r->next_t=current;
		x->tty=r;
		if (past != NULL){
			past->next_t=r;
		}
		else{
			ntype=r;
		}
		strcpy(r->type_rel,t_rel);
		r->max=1;
		q=malloc(sizeof(max_d));
		q->next_m=NULL;
		r->dest=q;
		strcpy(q->max_dest,id_dest);
		r->n_rel=1;
	}	
	#ifdef DEBUG
		trelation *w;
		w=cursor2->t_rel;
		while(w != NULL){
			printf("%s %d\n",w->rel_t,w->nrel);
			w=w->next_rt;
		}
		current=ntype;
		while(current !=NULL){
				printf("|%s %d %d|\n",current->type_rel,current->n_rel,current->max);
				current=current->next_t;
		}
	#endif
	return 0;
}

int delrel(char *id_orig, char *id_dest, char *t_rel){
	long int k,j;
	relation *temp;
	trb *csr;
	bool f=false;
	entity *cursor1,*cursor2;
	char id_rel[XL];
	strcpy(id_rel,id_dest);
	strcat(id_rel,t_rel);
	#ifdef DEBUG
		printf("ID_REL: %s\n",id_rel);
	#endif
	k=hashint(id_orig);						// verifica presenza enti
	j=hashint(id_dest);
	if(ent_table[k]== NULL || ent_table[j]== NULL){
		return 1;
	}
	cursor1=ent_table[k];
	while(cursor1 != NULL && f== false){
		if(strcmp(cursor1->id_ent,id_orig)==0){
			f=true;
		}
		if(f==false){
			cursor1=cursor1->next_e;
		}
	}
	if(f==false){
		return 1;
	}
	f=false;
	cursor2=ent_table[j];
	while(cursor2 != NULL && f==false){
		if(strcmp(cursor2->id_ent,id_dest)==0){
			f=true;
		}
		if(f==false){
			cursor2=cursor2->next_e;
		}
	}
	if(f==false){
		return 1;
	}
	j=hashintrel(id_rel);				// cerca se la relazione già esiste
	#ifdef DEBUG
		printf("HASH VALUE: %ld\n",j);
	#endif
	f=false;
	if(cursor1->rel_table==NULL){
		return 1;
	}
	if(cursor1->rel_table[j]!=NULL){
		csr=cursor1->rel_table[j];
		while(csr != NULL && f==false){
			if(strcmp(csr->id_r,id_rel)==0){
				f=true;
			}
			if(f==false){
				csr=csr->next;
			}
		}
		if(f==false){
			return 1;
		}
	}
	else{
		return 1;
	}
	enable=true;
	csr->rdest->nrel--;
	csr->tty->n_rel--;
	if(csr->ent->prev==NULL){
		cursor2->rlist=csr->ent->next;
		temp=csr->ent->next;
		if(temp!=NULL)
			temp->prev=NULL;
		free(csr->ent);
	}
	else{
		temp=csr->ent->prev;
		temp->next=csr->ent->next;
		temp=csr->ent->next;
		if(temp!=NULL)
			temp->prev=csr->ent->prev;
		free(csr->ent);
	}
	if(csr->prev !=NULL)
		csr->prev->next=csr->next;
	else
		cursor1->rel_table[j]=csr->next;
	if(csr->next!=NULL)
		csr->next->prev=csr->prev;
	free(csr);
	return 0;
}

int delent(char *name_e){
	bool f=false;
	int i,k;
	bool j=false;
	entity *cursor,*past;
	elenco_type *cs1;
	trelation *cs2;
	k=hashint(name_e);
	#ifdef DEBUG
		printf("%ld\n",k);
	#endif
	if(ent_table[k]!= NULL){					// verifica presenza ente
		cursor=ent_table[k];
		past=NULL;
		while(cursor != NULL  && f==false ){
			if(strcmp(cursor->id_ent,name_e)==0){
				f=true;
			}
			if(f==false){
				past=cursor;
				cursor=cursor->next_e;
			}
		}
		if(f==false){
			return 1;
		}
	} 
	else{
		return 1;
	}
	cs2=cursor->t_rel;
	while(cs2 != NULL){
		j=false;
		cs1=ntype;
		while(cs1!= NULL && j==false){
			if(strcmp(cs1->type_rel,cs2->rel_t)==0){
				cs1->n_rel-=cs2->nrel;
				j=true;
			}
			cs1=cs1->next_t;
		}
		cs2=cs2->next_rt;
	}				
	freelistrel(cursor->rlist);
	#ifdef DEBUG
		printf("Relazioni eliminate\n");
	#endif
	if(cursor->rel_table != NULL){	
		for(i=0;i<HASHSIZE_REL;i++){
			if(cursor->rel_table[i]!= NULL){
				freetrel(cursor->rel_table[i]);
			}
		}
		free(cursor->rel_table);
	}
	freelist(cursor->t_rel);
	if(past== NULL){							// eliminazione ente
		ent_table[k]=cursor->next_e;
		free(cursor);
	}
	else{							
		past->next_e=cursor->next_e;
		free(cursor);
	}
	enable=true;
	#ifdef DEBUG
		int l;
		for(l=0;l<HASHSIZE;l++){
			if(ent_table[l]!= NULL){
				cursor=ent_table[l];
				while(cursor !=NULL){
					printf("%s\n",cursor->id_ent);
					cursor=cursor->next_e;
				}
			}
		}
	#endif
	return 0;
}

int report(){
	elenco_type *cursor;
	entity *cs1;
	trelation *cs2;
	max_d *cs3,*pre,*q,*temp;
	int i;
	bool j;
	if(enable==true){
		typecheck();
	}
	cursor=ntype;
	if(ntype==NULL){					// verifica se il numero di relazioni e' 0 
		printf("none\n");
		return 0;
	}
	if(enable ==true){
		while(cursor!= NULL){
			for(i=0;i<HASHSIZE;i++){
				if(ent_table[i]!= NULL){
					cs1=ent_table[i];
					while(cs1 != NULL){
						cs2=cs1->t_rel;
						j=false;
						while(cs2!= NULL && j==false){
							if(strcmp(cursor->type_rel,cs2->rel_t)==0){
								j=true;
								cs3=cursor->dest;
								if(cs2->nrel > cursor->max){
									cursor->max=cs2->nrel;
									if(cursor->dest!=NULL){
										temp=cursor->dest;
										freemax(temp);
										cursor->dest= NULL;
										cs3=cursor->dest;
									}
								}
								if(cs2->nrel >= cursor->max && cs2->nrel!=0){
									pre=NULL;
									while(cs3 !=NULL && strcmp(cs3->max_dest,cs1->id_ent)<0){
										pre=cs3;
										cs3=cs3->next_m;
									}
									if(cs3== NULL || strcmp(cs3->max_dest,cs1->id_ent)!=0){
										q=malloc(sizeof(max_d));
										q->next_m=cs3;
										if (pre != NULL){
											pre->next_m=q;
										}
										else{
											cursor->dest= q;
										} 
										strcpy(q->max_dest,cs1->id_ent);
									}
								}
							}
							cs2=cs2->next_rt;
						}
						cs1=cs1->next_e;
					}
				}
			}
			cursor=cursor->next_t;
		}
	}
	cursor=ntype;
	while(cursor != NULL){				//stampa report
		printf("%s ",cursor->type_rel);
		cs3=cursor->dest;
		while(cs3 != NULL){
			printf("%s ",cs3->max_dest);
			cs3=cs3->next_m;
		}
		printf("%d; ",cursor->max);
		cursor=cursor->next_t;
	}
	printf("\n");
	enable=false;
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
