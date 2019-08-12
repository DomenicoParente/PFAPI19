#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DEBUG
#define HASHSIZE 2000
#define MAX 40
#define XL 100
#define XXL 150
#define N 4
									// STRUTTURE DATI	
struct TREL{
	char rel_t[MAX];
	int nrel;
	struct TREL *next_rt;
};
typedef struct TREL trelation;

struct TREE{
	char id_r[XXL];
	trelation *rdest;
	struct TREE *parent;
	struct TREE *right;
	struct TREE *left;
};
typedef struct TREE treenode;

treenode *r_tree;			// albero relazioni

struct ENT {
	char id_ent[MAX];
	trelation *t_rel;
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

void initiate(){						// FUNZIONI DI SUPPORTO
	int i;
	for(i=0; i<HASHSIZE; i++){
		ent_table[i]=NULL; 
	}
	ntype= NULL;
	r_tree= NULL;
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

treenode *search_tree(char *string){
	treenode *cursor;
	cursor=r_tree;
	while(cursor!= NULL){
		if(strcmp(string,cursor->id_r)==0){
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

treenode *insert_tree(char *string){
	treenode *cursor,*pre,*new;
	cursor=r_tree;
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
		r_tree=new;
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

void delete_node(treenode *node){
	treenode *del,*temp,*temp2,*min,*minc;
	del=node->parent;
	if(node==NULL){
		return;
	}
	if(del==NULL){			//caso radice
		// 0 figli
        if(node->left==NULL && node->right==NULL){
            free(node);
            r_tree=NULL;
        }
        // 1 figlio
        else if(node->left==NULL || node->right==NULL){
            if(node->left==NULL)
                temp=node->right;
            else
                temp=node->left;
            free(node);
            r_tree=temp;
            temp->parent=NULL;
        }
        // 2 figli
        else
        {
            minc=node->right;
            min=minc;
            while(minc != NULL){
				min=minc;
				minc=minc->left;
			}
            strcpy(node->id_r,min->id_r);
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
				temp2=min->right;
				temp2->parent=temp;
				free(min);
			}
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
        }
        // 2 figli
        else
        {
            minc=node->right;
            min=minc;
            while(minc != NULL){
				min=minc;
				minc=minc->left;
			}
            strcpy(node->id_r,min->id_r);
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
				temp2=min->right;
				temp2->parent=temp;
				free(min);
			}
        }
	}
}

void freetree(char *string,treenode *node){
	treenode *cursor,*temp;
	cursor=node;
	if(cursor!=NULL){
		freetree(string,node->right);
		freetree(string,node->left);
		if(strstr(cursor->id_r,string)!=NULL){
			cursor->rdest->nrel--;
			temp=cursor;
			delete_node(temp);
		}
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
	elenco_type *current,*past,*r;
	bool f=false,h;
	entity *cursor;
	char id_rel[XXL];
	strcpy(id_rel,id_orig);
	strcat(id_rel,id_dest);
	strcat(id_rel,t_rel);
	#ifdef DEBUG
		printf("ID_REL: %s\n",id_rel);
	#endif
	k=hashint(id_orig);						// verifica presenza enti
	j=hashint(id_dest);
	if(ent_table[k]== NULL || ent_table[j]== NULL){
		return 1;
	}
	cursor=ent_table[k];
	while(cursor != NULL && f== false){
		if(strcmp(cursor->id_ent,id_orig)==0){
			f=true;
		}
		if(f==false){
			cursor=cursor->next_e;
		}
	}
	if(f==false){
		return 1;
	}
	f=false;
	cursor=ent_table[j];
	while(cursor != NULL && f==false){
		if(strcmp(cursor->id_ent,id_dest)==0){
			f=true;
		}
		if(f==false){
			cursor=cursor->next_e;
		}
	}
	if(f==false){
		return 1;
	}
	cs=cursor->t_rel;
	if(r_tree != NULL){						// cerca se la relazione già esiste
		node=search_tree(id_rel);
		if(node != NULL){
			return 1;
		}
	}
	while(cs!= NULL && f==false){
			if(strcmp(cs->rel_t,t_rel)==0){
				f=true;
			}
			if(f==false){
				cs=cs->next_rt;
			}
		}
	if(h==false){						// aggiunto elemento tipo relazione
		p=malloc(sizeof(trelation));
		p->next_rt=cs;
		cs=p;
		strcpy(p->rel_t,t_rel);
		p->nrel=1;	
	}
	else{
		cs->nrel++;
	}
	node=insert_tree(id_rel);			// inserimento relazione nell'albero
	node->rdest=cs;
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
		r->dest= NULL;
	}	
	#ifdef DEBUG
		trelation *w;
		w=cursor->t_rel;
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
	treenode *node;
	elenco_type *cursor2;;
	bool f=false;
	entity *cursor;
	char id_rel[XXL];
	strcpy(id_rel,id_orig);
	strcat(id_rel,id_dest);
	strcat(id_rel,t_rel);
	#ifdef DEBUG
		printf("ID_REL: %s\n",id_rel);
	#endif
	k=hashint(id_orig);						// verifica presenza enti
	j=hashint(id_dest);
	if(ent_table[k]== NULL || ent_table[j]== NULL){
		return 1;
	}
	cursor=ent_table[k];
	while(cursor != NULL && f== false){
		if(strcmp(cursor->id_ent,id_orig)==0){
			f=true;
		}
		if(f==false){
			cursor=cursor->next_e;
		}
	}
	if(f==false){
		return 1;
	}
	f=false;
	cursor=ent_table[j];
	while(cursor != NULL && f==false){
		if(strcmp(cursor->id_ent,id_dest)==0){
			f=true;
		}
		if(f==false){
			cursor=cursor->next_e;
		}
	}
	if(f==false){
		return 1;
	}
	if(r_tree != NULL){						// cerca se la relazione già esiste
		node=search_tree(id_rel);
		if(node == NULL){
			return 1;
		}
	}
	else{
		return 1;
	}
	node->rdest->nrel--;
	node->rdest=NULL;
	delete_node(node);
	cursor2=ntype;
	while(cursor2!= NULL){
		if(strcmp(cursor2->type_rel,t_rel)==0){
			cursor2->n_rel--;
		break;
		}
		cursor2=cursor2->next_t;
	}
	return 0;
}

int delent(char *name_e){
	bool i=false;
	long int k;
	int j;
	entity *cursor,*past;
	trelation *cs2,*temp;
	elenco_type *cs1;
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

	cs1=ntype;
	if(cursor->t_rel != NULL){					// eliminazione relazioni ente
		temp=cursor->t_rel;
		while(cs1 != NULL){
			cs2=cursor->t_rel;
			j=0;
			while(cs2!= NULL && j==0){
				if(strcmp(cs1->type_rel,cs2->rel_t)==0){
					cs1->n_rel-=cs2->nrel;
					j=1;
				}
				cs2=cs2->next_rt;
			}
			cs1=cs1->next_t;
		}
		freetree(name_e,r_tree);
		freelist(temp);
		cursor->t_rel=NULL;
	}
	else{
		return 1;
	 }
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
	elenco_type *cursor;
	entity *cs1;
	trelation *cs2;
	max_d *cs3,*pre,*q,*temp;
	int i,j;
	typecheck();
	cursor=ntype;
	if(ntype==NULL){					// verifica se il numero di relazioni e' 0 
		printf("none\n");
		return 0;
	}
	while(cursor!= NULL){
		for(i=0;i<HASHSIZE;i++){
			if(ent_table[i]!= NULL){
				cs1=ent_table[i];
				while(cs1 != NULL){
					cs2=cs1->t_rel;
					j=0;
					while(cs2!= NULL && j==0){
						if(strcmp(cursor->type_rel,cs2->rel_t)==0){
							j=1;
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
