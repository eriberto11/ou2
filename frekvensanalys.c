#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hufftree.h"
#include "dlist.h"
#include "bitset.h"
#include <string.h>
#include <unistd.h>
#define ARR_SIZE 256



typedef struct HuffIntArrayTable {
    int array[ARR_SIZE][2];
    int arrNumber;

} HuffIntArrayTable;


typedef struct {
    char character;
    bitset *bit;
} listElement;

void exchange (int *x, int *y){
    int temp;
    temp = *x;
    *x=*y;
    *y=temp;
}
/*Help functiom for the treesort*/
void siftup (int i, int n, HuffIntArrayTable *h){
    int j = (2 * i)+1;
    int copyV = h->array[i][1];
    int copyC = h->array[i][0];
    while (j <= n-1){
        if (j<n-1 && h->array[j+1][1] > h->array[j][1]){
            j++;
        }
        if (h->array[j][1]<copyV){
            h->array[i][1]=copyV;
            h->array[i][0]=copyC;
            break;
        }
        h->array[i][1]=h->array[j][1];
        h->array[i][0]=h->array[j][0];
        i = j;
            h->array[i][1]=copyV;
            h->array[i][0]=copyC;
        j=(2*i)+1;
    }

}
/*Sorts a HuffIntArrayTable by the number of times each char has appeared in a text,
    lowest first. */
void treesortHuffArr(HuffIntArrayTable *h, int n){
    int i;
    int x=1;
    int temp;
    for (i=(n-1)/2;i>0;i--){

        x++;
        siftup(i,n,h);

    }
    x=1;

    for (i=n;i>0;i--){

        x++;
        siftup(0,i,h);


        temp = h->array[0][1];
        h->array[0][1]=h->array[i-1][1];
        h->array[i-1][1]=temp;

        temp = h->array[0][0];
        h->array[0][0]=h->array[i-1][0];
        h->array[i-1][0]=temp;
    }

}

huff_tree *huffEncode (HuffIntArrayTable *harr){

    const int size = harr->arrNumber;
    huff_tree *huffArr[size];
    for (int i = 0; i<size; i++){
        huffArr[i] = huffTree_create();
        huffArr[i]->root->character=(data)(intptr_t)harr->array[i][0];
        huffArr[i]->root->weight=(data)(intptr_t)harr->array[i][1];
    }

    int n = 1;

    while (n<size){


        huffArr[n]=huffTree_merge(huffArr[n], huffArr[n-1]);

        int t = n;
        int j = n+1;

        if (j<size){

            while (j<size && (int)(intptr_t)huffArr[t]->root->weight >(int)(intptr_t)huffArr[j]->root->weight){

                huff_tree *tempHuff;

                tempHuff=huffArr[t];
                huffArr[t]=huffArr[j];
                huffArr[j]=tempHuff;

                t++;
                j++;
            }
        }
        n++;
    }

    return huffArr[n-1];
}



int compareInt(void *ip,void *ip2){
    return (*(int*)ip) - (*(int*)ip2);
}

void readTextToArray (HuffIntArrayTable *h, FILE *fp){
    char temp;

    while((temp = fgetc(fp)) != EOF){

        if (ferror(fp)) {
            break;
        }
        for (int i=0;i<ARR_SIZE;i++){
            if (h->arrNumber==0){
                h->array[0][0]=(int)temp;
                h->array[0][1]=1;
                h->arrNumber++;

                break;
            }
            else if (h->array[i][0]==temp){
                h->array[i][0]=(int)temp;
                h->array[i][1]=h->array[i][1] + 1;


                break;
            }
            else if (i==h->arrNumber){
                h->array[i][0]=(int)temp;
                h->array[i][1]= 1;
                h->arrNumber++;

                break;
            }
        }
    }
    h->array[h->arrNumber][0]=(int)'\4';
    h->array[h->arrNumber][1]=1;
    h->arrNumber++;
}


/*
    Tar in en pekare till en tom bitset. För varje iteration av treetotable
    blir bitset 1 bit längre, dessa bitar beskriver vägen igenom trädet för
    att återfinna karaktären.
    Då hufftree hittar en bokstav i positionen efter rekursion så läggs bokstaven
    tillsammans med bitset in i tabellen. bokstaven är key, och bitset är value.
    Minnesallokeringen är inte solklar. Helt klart minnesläckor i detta!
*/
void treeToList(huff_tree *h, huffTree_pos p,bitset *b,dlist *l){
   // om karaktär påträffas så är vi i ett löv, då borde bitseten vara spikad och vi kan skriva till table.
    if(huffTree_hasCharacter(h,p)){
        // skapa en bytearray av vägen vandrad genom hufftree
        //bokstaven från hufftree
        char qq = huffTree_inspectCharacter(h,p);
        listElement *le = malloc(1*sizeof(listElement));
        le->character= qq;
        le->bit = b;
     //   printf(" = %c och bytearray = %d\n", le->character, b->array);
        //stoppa in pekare till bokstaven och pekare till bitset.
        dlist_insert(l,dlist_first(l),le);
        // ta bort barnlösa föräldrar som ledde till noden. Stanna vid root, tas den bort tappar åkallande funktion bort sig.
        p = huffTree_deleteNode(h,p);
        while(!huffTree_hasRightChild(h,p) && !huffTree_hasLeftChild(h,p) && p!=h->root){
            p = huffTree_deleteNode(h,p);
        }
    }
        // om ingen bokstav påträffas är vi i en transportsträcka, kalla rekursivt barn till noden, och
        //skriv vägen till bitset.
    else{
        //vi håller vänster genom trädet, så vänsterbarn hittas först.
        if(huffTree_hasLeftChild(h,p)) {
     //       printf("0");
            p = huffTree_leftChild(h,p);
            bitset_setBitValue(b,bitset_size(b),false);
            treeToList(h,p,b,l);
        }
        else{
     //       printf("1");
            p = huffTree_rightChild(h,p);
            bitset_setBitValue(b,bitset_size(b),true);
            treeToList(h,p,b,l);
        }
    }
}
/*
    Här får vi en pekare till en öppnad fil, vi går igenom karaktär för karaktär
    och stegar igenom listan för att återfinna karaktären. Sedan lägger vi till
    karaktärens bitvärde som skapades i treeToList, och lägger till det till
    det till 'bitset ut' som kommer att skrivas till utfilen.
    Avslutar med att leta upp EOT-tecknet, '\4', och passar på att avallokera
    alla bitsets från listan *l.
    Sist skrivs bitset ut till filen med fprintf.
*/

void listtozip(dlist *l, char *utfil, FILE *infil) {
   char temp;
   bitset *ut= bitset_empty();
   unsigned long x=0;
   FILE *utfilen = fopen(utfil, "w");
   while((temp = fgetc(infil)) != EOF){
    int stopp=0;
    listElement *e;
    dlist_position p = dlist_first(l);
        while(!dlist_isEnd(l,p) && stopp==0){
            e=dlist_inspect(l,p);
            p=dlist_next(l,p);
            if(e->character==temp){
                int lengd = bitset_size(e->bit);
    //            printf("matchad karaktär '%c' ::: bitlängd %d\n", e->character,lengd);
                for(int i=0;i<lengd;i++){
                    bitset_setBitValue(ut,x,bitset_memberOf(e->bit,i));
                    x++;
                }
                stopp=1;
            }
        }
   }
   dlist_position p=dlist_first(l);
   listElement *e;
   while(!dlist_isEnd(l,p)){
        e=dlist_inspect(l,p);
        p=dlist_next(l,p);
        if(e->character=='\4'){
            int lengd = bitset_size(e->bit);
    //        printf("matchad karaktär '%c' ::: bitlängd %d\n", e->character,lengd);
            for(int i=0;i<lengd;i++){
                bitset_setBitValue(ut,x,bitset_memberOf(e->bit,i));
                x++;
            }
            bitset_free(e->bit);
            free(e);
        }
        else{
            bitset_free(e->bit);
            free(e);
        }
   }
   char *bytearr = toByteArray(ut);
  // printf(" komplett längd %d bitset, x = %d\n",bitset_size(ut) , x);
    fwrite(bytearr,sizeof(char),ut->capacity/8,utfilen);
    //fprintf(utfilen, "%s" , bytearr);
    printf("%d bytes used in encoded form.\n",bitset_size(ut)/8+1);
    fclose(utfilen);
    free(bytearr);
    bitset_free(ut);
}

/*
    Här stegar vi igenom bitset b som innehåller en hel komprimerad fil,
    och för varje steg tar vi motsvarande steg i trien. När vi nått ett löv
    så läser vi värdet och skriver till fil. Sedan går vi tillbaka till roten
    och fortsätter.
*/
void unzipFromFile(bitset *b, huff_tree *h, char *utfil) {
    FILE *fp = fopen(utfil, "w");
    int x=0;
    int c;
    huffTree_pos p=huffTree_root(h);
    while(x<b->length) {
        if(huffTree_hasCharacter(h,p)){
            c=huffTree_inspectCharacter(h,p);
            if(c=='\4') {
                x=b->length;
               printf("File decoded succesfully.\n");
                }
            else {
                fputc(c,fp);
     //           printf("  HITTADE CHAR I huffträd %c\n", c);
                p=huffTree_root(h);
            }
        }
        else {
            bool boul = bitset_memberOf(b,x);
            x++;
            if(boul) {
                p=huffTree_rightChild(h,p);
    //            printf("1");
            }
            else {
                p=huffTree_leftChild(h,p);
    //            printf("0");
            }
        }
    }
}

int main (int argc, char *argv[]){
    int ziporunzip=0;
    if(argc!= 5){
        printf("USAGE:\nhuffman [OPTION] [FILE0] [FILE1] [FILE2]\n Options:\n-encode encodes FILE1 according to frequence analysis done on FILE0."        " Stores the result in FILE2\n-decode decodes FILE1 according to frequence analysis done on FILE0. Stores the result in FILE2\n");
        exit(0);
    }
    if(strncmp(argv[1],"-encode",7)==0){
        ziporunzip=0;
    }
    else if(strncmp(argv[1],"-decode",7)==0){
        ziporunzip=1;
    }
    else{
        printf("USAGE:\nhuffman [OPTION] [FILE0] [FILE1] [FILE2]\n Options:\n-encode encodes FILE1 according to frequence analysis done on FILE0."        " Stores the result in FILE2\n-decode decodes FILE1 according to frequence analysis done on FILE0. Stores the result in FILE2\n");
        exit(0);
    }
    if(access(argv[2], F_OK) ==-1){
        printf("no access to FILE0");
        exit(0);
    }
    HuffIntArrayTable *h = calloc(1,sizeof(HuffIntArrayTable));
    FILE *fp=fopen(argv[2],"r");
    readTextToArray(h,fp);
    fclose(fp);
    treesortHuffArr(h,h->arrNumber);
    huff_tree *huff = huffEncode(h);
    if(access(argv[3], F_OK) ==-1){
        printf("no access to FILE1");
        exit(0);
    }
    if(ziporunzip==0) {
        FILE *fp0 = fopen(argv[3],"r");
        fseek(fp0, 0L, SEEK_END);
        int fp0size = ftell(fp0);
        fclose(fp0);
        printf("%d bytes read from %s.\n", fp0size,argv[3]);
        huffTree_pos pos= huffTree_root(huff);
        dlist *l = dlist_empty();
        dlist_setMemHandler(l,free);
        while(huffTree_hasRightChild(huff,pos)){
            bitset *b = bitset_empty();
            treeToList(huff,pos,b,l);
        }
        FILE *fp1= fopen(argv[3], "r");
        listtozip(l,argv[4], fp1);
        fclose(fp1);
        dlist_free(l);
    }
    else {
       bitset *b=bitsetFromFile(argv[3]);
       unzipFromFile(b,huff,argv[4]);
        bitset_free(b);
    }
    free(huff->root);
    free(huff);
    free(h);
    return 0;

}
