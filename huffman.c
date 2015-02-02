#include <stdio.h>
#include "dlist.h"

typedef struct  Content {
    int count;
    char karaktar;
    dlist_position parent;
    dlist_position childL;
    dlist_position childR;
} Content;

void printText(char *read){
    int a;
    FILE *file=fopen(read, "r");
    while( ( a=fgetc(file))!= EOF){
        printf("%c",a);
    }
    fclose(file);
}

dlist* textToTable(char *read){
    dlist *l = dlist_empty();
    int a;
    FILE *file = fopen(read, "r");
    while ( ( a = fgetc(file ) ) != EOF){
        printf("%c", a);
        if(!dlist_isEmpty(l)){
        dlist_position pos = dlist_first(l);
             int counted=0;
             while(!dlist_isEnd(l,pos)){
                Content *q = dlist_inspect(l,pos);
                if(q->karaktar==a){
                    q->count++;
                    counted=1;
                }
                pos= dlist_next(l,pos);
            }
            if(counted!=1){
                Content *s;
                s = malloc(1*sizeof(Content));
                s->count=1;
                s->childL=NULL;
                s->childR=NULL;
                s->karaktar=a;
                dlist_insert(l,dlist_first(l),s);
            }
        }
        else{
            Content *s = malloc(1*sizeof(Content));
            s->count=1;
            s->karaktar=a;
            s->childL=NULL;
            s->childR=NULL;
            dlist_insert(l,dlist_first(l), s);
        }
    }
    return l;
}

void printDlist(dlist *l) {
    dlist_position p = dlist_first(l);
    while(!dlist_isEnd(l,p)) {
        Content *f = dlist_inspect(l,p);
        printf("%d stycken %c'n  , child->L = %d child->R =%d \n", f->count,f->karaktar,f->childL,f->childR);
        p=dlist_next(l,p);
    }
}
/*
void sortDlistLo2High (dlist* l) {
    dlist_position p = dlist_first(l);
    dlist_position p2 = dlist_next(l,p);
    int highest=0;
    while(!dlist_isEnd(l,p2)) {
        Content *c = dlist_inspect(l,p);
        Content *c2= dlist_inspect(l,p);
        Content temp;
            if(c->count>c2->count) {
                Content temp = *c2;
                c2->count = c->count;
                c->count= temp.count;
            }
            p = p2;
            p2 = dlist_next(l,p);
    }
}*/


void treeSortDlist(dlist* l,dlist_position p,dlist_position x) {
    Content *node=dlist_inspect(l,p);
    Content *input=dlist_inspect(l,x);
    if(input->count<=node->count){
        if(node->childL==NULL){
            node->childL=x;
            input->parent=p;
        }
        else{
            treeSortDlist(l,node->childL, x);
        }
    }
    else if(input->count>node->count){
        if(node->childR==NULL) {
            node->childR=x;
            input->parent=p;
        }
        else{
            treeSortDlist(l,node->childR,x);
        }
    }
}

void collapseTree(dlist* l) {
    dlist_position p=dlist_first(l);
    Content *n=dlist_inspect(l, p);
    while(n->childR!=NULL){
        n=dlist_inspect(l,n->childR);
    }
    n=dlist_inspect(l,n->parent);
    n->childR->next=l->head;
    l->head=n->childR;
    n->childR=NULL;
    if(n->childL!=NULL){

    }


}

int createArrayofDlistPositions (dlist* l){
    dlist_position p=dlist_first(l);
    dlist_position q=dlist_next(l,p);
    int x=1;
    while(!dlist_isEnd(l,q)){
        treeSortDlist(l, p, q);
        q=dlist_next(l, q);
        x++;
        printf("createarray\n");
    }
    return x;
}

void recursiveCollapseTree(dlist * l, dlist_position x){
    Content *n = dlist_inspect(l,x);
    if ( n->childR==NULL ) {
        dlist_insert(l, dlist_first(l), n);
        Content *m=dlist_inspect(l,n->parent);
        if(m->childL == x)
            m->childL=NULL;
        if(m->childR==x)
            m->childR=NULL;
        dlist_remove(l,x);
        if (n->childL!= NULL){
            recursiveCollapseTree(l,n->childL);
        }
    }
    else{
        if(n->childR != NULL) {
            recursiveCollapseTree(l,n->childR);
        }
    }
}

void callTreeCollapse(dlist *l, int n){
    dlist_position p = dlist_first(l);
    for(int a=0;a<n;a++) {
        recursiveCollapseTree(l,p);
        printf("collapsearray\n");
    }
}
int main(int argc, char *argv[]){
   if(argc!= 2){
   printf("Usage Huffman ... huffman flaggor texter\n");
   }
   else{
        FILE *file= fopen(argv[1], "r");
        if(file==0){
            printf("file not found");
        }
        else{
            dlist *l=textToTable(argv[1]);
        //    sortDlistLo2High(l);
            printDlist(l);
          int n=  createArrayofDlistPositions(l);
            printDlist(l);
            callTreeCollapse(l,n);
            printDlist(l);
        }
        fclose(file);
   }

}
