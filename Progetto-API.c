#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//#define DEBUG
#define HASHSIZE 2000
#define MAX 40
#define XL 100
#define N 4
									// STRUTTURE DATI	
struct REL{
	struct REL *next;
	struct REL *prev;
	struct TREE *rel;
	struct TREE **root;
};
typedef struct REL relation;

struct TREL{
	char rel_t[MAX];
	int nrel;
	struct TREL *next_rt;
};
typedef struct TREL trelation;

struct TREE{
	char id_r[XL];
	trelation *rdest;
	relation *ent;
	struct REL **first;
	struct TREE *parent;
	struct TREE *right;
	struct TREE *left;
};
typedef struct TREE treenode;

struct ENT {
	char id_ent[MAX];
	trelation *t_rel;
	relation *rlist;
	treenode *rtree;
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
	struct ELENCO *next_t;
};
typedef struct ELENCO elenco_type;

void initiate(){						// FUNZIONI DI SUPPORTO
	int i;
	for(i=0; i<HASHSIZE; i++){
		ent_table[i]=NULL; 
	}
}

long int hashint(char *name_e){
	unsigned long int hash = 5381;
    unsigned int c,i,add=0;
    c=strlen(name_e);
    for(i=0;i<c;i++){
		add+=(int)name_e[i];
	}
	while (c>0){
		hash = ((hash << 5) + hash)+add + c;
		c--;
	}
	return hash%HASHSIZE;
}

treenode *search_tree(char *string, treenode *r_tree){
	treenode *cursor;
	cursor=r_tree;
	while(cursor!= NULL){
		if(strcmp(string,cursor->id_r)==0){
			#ifdef DEBUG
				printf("Trovato: %s\n",cursor->id_r);
			#endif
			return cursor;
		}
		if(strcmp(string,cursor->id_r)>0){
			cursor=cursor->right;
		}
		else{
			cursor=cursor->left;
		}
	}
	return cursor;
}

treenode *insert_tree(char *string, treenode **r_tree){
	treenode *cursor,*pre,*new;
	cursor=*r_tree;
	pre=NULL;
	while(cursor!= NULL){
		pre=cursor;
		if(strcmp(string,cursor->id_r)>0){
			cursor=cursor->right;
		}
		else{
			cursor=cursor->left;
		}
	}
	new=malloc(sizeof(treenode));
	new->parent=pre;
	strcpy(new->id_r,string);
	if(pre == NULL){
		*r_tree=new;
	}
	else if(strcmp(string,pre->id_r)>0){
		pre->right=new;	
	}
	else{
		pre->left=new;
	}
	new->left=NULL;
	new->right=NULL;
	return new;
}

void delete_node(treenode *node, treenode **r_tree){
	treenode *del,*temp,*temp2,*min,*minc;
	if(node==NULL){
		return;
	}
	del=node->parent;
	if(del==NULL){			//caso radice
		// 0 figli
        if(node->left==NULL && node->right==NULL){
            free(node);
            *r_tree=NULL;
            return;
        }
        // 1 figlio
        else if(node->left==NULL || node->right==NULL){
            if(node->left==NULL)
                temp=node->right;
            else
                temp=node->left;
            free(node);
            *r_tree=temp;
            temp->parent=NULL;
            return;
        }
        // 2 figli
        else{
            minc=node->right;
            min=minc;
            while(minc != NULL){
				min=minc;
				minc=minc->left;
			}
            strcpy(node->id_r,min->id_r);
            node->ent=min->ent;
            node->rdest=min->rdest;
            node->first=min->first;
            min->ent->rel=node;
            temp=min->parent;
            if(temp->right==min){
				temp->right=min->right;
				if(min->right!= NULL){
					temp2=min->right;
					temp2->parent=temp;
				}
				free(min);
			}
			else{
				temp->left=min->right;
				if(min->right !=NULL){
					temp2=min->right;
					temp2->parent=temp;
				}
				free(min);
			
			}
			return;
        }
	}
	else{
		// 0 figli
        if(node->left==NULL && node->right==NULL){
			temp=node->parent;
			if(temp->right==node){
				temp->right=NULL;
			}
			else{
				temp->left=NULL;
			}
            free(node);
            return;
        }
        // 1 figlio
        else if(node->left==NULL || node->right==NULL){
            if(node->left==NULL)
                temp=node->right;
            else
                temp=node->left;
			temp2=node->parent;
			if(temp2->right==node){
				temp2->right=temp;
			}
			else{
				temp2->left=temp;
			}
			temp->parent=temp2;
            free(node);
            return;
        }
        // 2 figli
        else{
            minc=node->right;
            min=minc;
            while(minc != NULL){
				min=minc;
				minc=minc->left;
			}
            strcpy(node->id_r,min->id_r);
            node->ent=min->ent;
            node->rdest=min->rdest;
            node->first=min->first;
            min->ent->rel=node;
            temp=min->parent;
            if(temp->right==min){
				temp->right=min->right;
				if(min->right != NULL){
					temp2=min->right;
					temp2->parent=temp;
				}
				free(min);
			}
			else{
				temp->left=min->right;
				if(min->right !=NULL){
					temp2=min->right;
					temp2->parent=temp;
				}
				free(min);
			}
        }
	}
}

void freetree(treenode *r_tree){
	treenode *cursor,*del;
	relation *temp;
	cursor=r_tree;
	if(cursor!=NULL){
		freetree(cursor->left);
		freetree(cursor->right);
		#ifdef DEBUG
			printf("Eliminato: %s\n",cursor->id_r);
		#endif
		cursor->rdest->nrel--;
			if(cursor->ent->prev==NULL){
				*cursor->first=cursor->ent->next;
				temp=cursor->ent->next;
				if(temp!=NULL)
					temp->prev=NULL;
			}
			else{
				temp=cursor->ent->prev;
				temp->next=cursor->ent->next;
				temp=cursor->ent->next;
				if(temp!=NULL)
					temp->prev=cursor->ent->prev;
			}
			free(cursor->ent);
			cursor->ent=NULL;
			del=cursor;
			free(del);	
	}
}
void freelistrel(relation *rel){
	relation *l;
	l=rel;
	if (l != NULL) {
		freelistrel(l->next);
		if(l->rel !=NULL){
			#ifdef DEBUG
			printf("Eliminato: %s\n",l->rel->id_r);
		#endif
			delete_node(l->rel,l->root);
		}
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

void freelistr(elenco_type **elen){
	elenco_type *l;
	l=*elen;
	if (l != NULL) {
		freelistr(&l->next_t);
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

int addent(char *name_e){ 							// FUNZIONI
	long int k;
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
	p->rtree=NULL;
	p->next_e=ent_table[k];
	ent_table[k]=p;
		
	#ifdef DEBUG
		int i;
		printf("Valore hash: %ld \n",k);
		for(i=0;i<HASHSIZE;i++){
			if(ent_table[i]!= NULL){
				cursor=ent_table[i];
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
	long int k,j;
	treenode *node;
	trelation *p,*cs;
	relation *s;
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
	if(cursor1->rtree != NULL){						// cerca se la relazione già esiste
		node=search_tree(id_rel,cursor1->rtree);
		if(node != NULL){
			return 1;
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
	node=insert_tree(id_rel,&cursor1->rtree);			// inserimento relazione nell'albero
	s->rel=node;
	s->root=&cursor1->rtree;
	node->ent=s;
	node->rdest=cs;
	node->first=&cursor2->rlist;
	#ifdef DEBUG
		trelation *w;
		w=cursor2->t_rel;
		while(w != NULL){
			printf("%s %d\n",w->rel_t,w->nrel);
			w=w->next_rt;
		}
	#endif
	return 0;
}

int delrel(char *id_orig, char *id_dest, char *t_rel){
	long int k,j;
	treenode *node;
	relation *temp;
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
	if(cursor1->rtree != NULL){						// cerca se la relazione già esiste
		node=search_tree(id_rel,cursor1->rtree);
		if(node == NULL){
			return 1;
		}
	}
	else{
		return 1;
	}
	node->rdest->nrel--;
	if(node->ent->prev==NULL){
		cursor2->rlist=node->ent->next;
		temp=node->ent->next;
		if(temp!=NULL)
			temp->prev=NULL;
		free(node->ent);
	}
	else{
		temp=node->ent->prev;
		temp->next=node->ent->next;
		temp=node->ent->next;
		if(temp!=NULL)
			temp->prev=node->ent->prev;
		free(node->ent);
	}
	node->ent=NULL;
	delete_node(node,&cursor1->rtree);
	return 0;
}

int delent(char *name_e){
	bool i=false;
	long int k;
	int j;
	entity *cursor,*past;
	k=hashint(name_e);
	#ifdef DEBUG
		printf("%ld\n",k);
	#endif
	if(ent_table[k]!= NULL){					// verifica presenza ente
		cursor=ent_table[k];
		past=NULL;
		while(cursor != NULL  && i==false ){
			if(strcmp(cursor->id_ent,name_e)==0){
				i=true;
			}
			if(i==false){
				past=cursor;
				cursor=cursor->next_e;
			}
		}
		if(i==false){
			return 1;
		}
	} 
	else{
		return 1;
	}				
	freelistrel(cursor->rlist);
	#ifdef DEBUG
		printf("Relazioni ente eliminate\n");
	#endif
	freetree(cursor->rtree);
	#ifdef DEBUG
		printf("Relazioni eliminate\n");
	#endif
	freelist(cursor->t_rel);
	cursor->t_rel=NULL;
	if(past== NULL){							// eliminazione ente
		ent_table[k]=cursor->next_e;
		free(cursor);
	}
	else{							
		past->next_e=cursor->next_e;
		free(cursor);
	}
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
	elenco_type *ntype,*cursor,*past,*r;
	int i;
	entity *cs1;
	trelation *cs2;
	max_d *cs3,*pre,*q,*temp;
	bool f;
	ntype=NULL;
	for(i=0;i<HASHSIZE;i++){
		cs1=ent_table[i];
		while(cs1!= NULL){
			cs2=cs1->t_rel;
			while(cs2!= NULL){
				if(cs2->nrel>0){
					cursor=ntype;
					f=false;
					past=NULL;
					while(cursor!= NULL && f==false && strcmp(cursor->type_rel,cs2->rel_t)<=0){
						if(strcmp(cs2->rel_t,cursor->type_rel)==0){
							f=true;
						}
						if(f==false){
							past=cursor;
							cursor=cursor->next_t;
						}
					}
					if(f==false){
						r=malloc(sizeof(elenco_type));
						r->next_t=cursor;
						if(past!= NULL){
							past->next_t=r;
						}
						else{
							ntype=r;
						}
						strcpy(r->type_rel,cs2->rel_t);
						r->max=0;
						r->dest= NULL;
						cursor=r;
					}
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
	if(ntype==NULL){					// verifica se il numero di relazioni e' 0 
		printf("none\n");
		return 0;
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
		cursor->max=0;
		freemax(cursor->dest);
		cursor=cursor->next_t;
	}
	printf("\n");
	freelistr(&ntype);
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
