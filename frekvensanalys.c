#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hufftree.h"
#include "dlist.h"
#include "bitset.h"
#include <string.h>
#include <unistd.h>
#define ARR_SIZE 256


/*
typedef struct HuffIntArrayTable {
    int array[ARR_SIZE][2];
    int arrNumber;

} HuffIntArrayTable;*/


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
void siftup (int i, int n, int h[][2]){
    int j = (2 * i)+1;
    int copyV = h[i][1];
    int copyC = h[i][0];
    while (j <= n-1){
        if (j<n-1 && h[j+1][1] > h[j][1]){
            j++;
        }
        if (h[j][1]<copyV){
            h[i][1]=copyV;
            h[i][0]=copyC;
            break;
        }
        h[i][1]=h[j][1];
        h[i][0]=h[j][0];
        i = j;
            h[i][1]=copyV;
            h[i][0]=copyC;
        j=(2*i)+1;
    }

}
/*Sorts a HuffIntArrayTable by the number of times each char has appeared in a text,
    lowest first. */
void treesortHuffArr(int h[][2], int n){
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
        temp = h[0][1];
        h[0][1]=h[i-1][1];
        h[i-1][1]=temp;
        temp = h[0][0];
        h[0][0]=h[i-1][0];
        h[i-1][0]=temp;
    }

}

huff_tree *buildHuffTree (int harr[][2]){
    //const int size = harr->arrNumber;
    const int size = ARR_SIZE;
    huff_tree *huffArr[size];
    for (int i = 0; i<size; i++){
        huffArr[i] = huffTree_create();
        huffArr[i]->root->character=(data)(intptr_t)harr[i][0];
        huffArr[i]->root->weight=(data)(intptr_t)harr[i][1];
    }

    int n = 1;

    while (n<size){
        huffArr[n]=huffTree_merge(huffArr[n], huffArr[n-1]);
        int t = n;
        int j = n+1;
        if (j<size){

            while (j<size && (int)(intptr_t)huffArr[t]->root->weight >= (int)(intptr_t)huffArr[j]->root->weight){

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
    /*Här skriver jag ut root-värdet, värdet på dom två första barnen och om det högra
    barnet har barn så skrivs dom också ut*/
/*
            printf("\n\n Root %c %d ", (char)(intptr_t)huffArr[n-1]->root->character, (int)(intptr_t)huffArr[n-1]->root->weight);

            huffTree_pos p = huffTree_root(huffArr[n-1]);
            p = huffTree_leftChild(huffArr[n-1],p);
            data testC = huffTree_inspectCharacter(huffArr[n-1],p);
            data testw = huffTree_inspectWeight(huffArr[n-1],p);
            printf("Left %c %d", (int)(intptr_t)testC, (int)(intptr_t)testw);

            p = huffTree_root(huffArr[n-1]);
            p = huffTree_rightChild(huffArr[n-1],p);
            testC = huffTree_inspectCharacter(huffArr[n-1],p);
            testw = huffTree_inspectWeight(huffArr[n-1],p);
            printf("Right %c %d", (int)(intptr_t)testC, (int)(intptr_t)testw);
            printf("\n\n Right Child \n");

            if (huffTree_hasLeftChild(huffArr[n-1],p)){
                p = huffTree_leftChild(huffArr[n-1],p);
                data testC = huffTree_inspectCharacter(huffArr[n-1],p);
                data testw = huffTree_inspectWeight(huffArr[n-1],p);
                printf("Left %c %d", (int)(intptr_t)testC, (int)(intptr_t)testw);
            }

            p = huffTree_root(huffArr[n-1]);
            p = huffTree_rightChild(huffArr[n-1],p);

            if (huffTree_hasRightChild(huffArr[n-1],p)){
                p = huffTree_rightChild(huffArr[n-1],p);
                data testC = huffTree_inspectCharacter(huffArr[n-1],p);
                data testw = huffTree_inspectWeight(huffArr[n-1],p);
                printf("Right %c %d", (int)(intptr_t)testC, (int)(intptr_t)testw);
            }
    printf("\n\n");*/
    return huffArr[n-1];
}


void readTextToArray (int h[][2], FILE *fp){
    char temp;

    for (int i = 0; i<ARR_SIZE;i++){
        h[i][0]=i;
        h[i][1]=0;
    }
    unsigned int c;
    temp = fgetc(fp);
    //while((temp = fgetc(fp)) != EOF){
    while( temp != EOF){
        if (ferror(fp)) {
            break;
        }
        c=(unsigned char)temp;// - '0';
        int t = h[c][1];
        h[c][1]=t+1;
        temp = fgetc(fp);
    }
    int slut = (int)'\4';
    h[slut][0]=slut;
    h[slut][1]=1;



    /*Ett enkelt hack för att inte huffTree_hasCharacter ska tappa det helt
        för att det finns en bokstav som är NULL...*/
   // h[0][0]=666;


    //printf("\n\n BLIRE NÅ FREKVENS?\n");
    //for (int i = 0; i<ARR_SIZE;i++){
    //    printf("%c=%d   ", (char)h[i][0], h[i][1]);
    //}

}


/*
    Tar in en pekare till en tom bitset. För varje iteration av treetotable
    blir bitset 1 bit längre, dessa bitar beskriver vägen igenom trädet för
    att återfinna karaktären.
    Då hufftree hittar en bokstav i positionen vi stegat till så läggs bokstaven
    tillsammans med bitset in i tabellen. bokstaven är key, och bitset är value.
*/
void treeToList(huff_tree *h, huffTree_pos p,bitset *b,dlist *l){
   // om positionen är barnlös så är vi i ett löv, då borde bitseten vara spikad och vi kan skriva till table.
    if(!huffTree_hasLeftChild(h,p) && !huffTree_hasRightChild(h,p)){
        // skapa en bytearray av vägen vandrad genom hufftree
        //bokstaven från hufftree
        char qq = (char)(intptr_t)huffTree_inspectCharacter(h,p);
        listElement *le = malloc(1*sizeof(listElement));
        le->character= qq;
        le->bit = b;
     //   printf(" = %c och bytearray = %d\n", le->character, b);
        //stoppa in pekare till bokstaven och pekare till bitset.
        dlist_insert(l,dlist_first(l),le);
        // ta bort barnlösa föräldrar som ledde till noden. Stanna vid root, tas den bort tappar åkallande funktion bort sig.
        p = huffTree_deleteNode(h,p);
        while(!huffTree_hasRightChild(h,p) && !huffTree_hasLeftChild(h,p) && p!=h->root){
            p = huffTree_deleteNode(h,p);
        }
    }
        // om ingen bokstav påträffas är vi i en transportsträcka, kalla funktionen igen för att hitta barn till noden, och
        //skriv vägen till bitset.
    else{
        //vi håller vänster genom trädet, så vänsterbarn hittas först.
        if(huffTree_hasLeftChild(h,p)) {
            p = huffTree_leftChild(h,p);
            bitset_setBitValue(b,bitset_size(b),false);
            treeToList(h,p,b,l);
        }
        else{
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
    while((temp = fgetc(infil)) != EOF) {
    int stopp=0;
    listElement *e;
    dlist_position p = dlist_first(l);
        while(!dlist_isEnd(l,p) && stopp==0) {
            e=dlist_inspect(l,p);
            p=dlist_next(l,p);
            if(e->character==temp) {
                int lengd = bitset_size(e->bit);
                for(int i=0;i<lengd;i++) {
                    bitset_setBitValue(ut,x,bitset_memberOf(e->bit,i));
                    x++;
                }
                stopp=1;
            }
        }
   }
    dlist_position p=dlist_first(l);
    listElement *e;
    while(!dlist_isEnd(l,p)) {
        e=dlist_inspect(l,p);
        p=dlist_next(l,p);
        if(e->character==(int)'\4') {
            int lengd = bitset_size(e->bit);
            for(int i=0;i<lengd;i++) {
                bitset_setBitValue(ut,x,bitset_memberOf(e->bit,i));
                x++;
            }
            int rest=8 - bitset_size(ut)%8;
            int tot = bitset_size(ut) + rest;

            for(int n= bitset_size(ut); n<tot; n++) {
                bitset_setBitValue(ut,n,true);

            }
            bitset_free(e->bit);
        }
        else {
            bitset_free(e->bit);
        }
   }
    char *bytearr = toByteArray(ut);
    fwrite(bytearr,sizeof(char),bitset_size(ut)/8,utfilen);
    printf("%d bytes used in encoded form.\n",bitset_size(ut)/8);
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
        if(!huffTree_hasLeftChild(h,p) && !huffTree_hasRightChild(h,p)) {
            c=(int)(intptr_t)huffTree_inspectCharacter(h,p);
            if(c==(int)'\4') {
                x=b->length;
               printf("File decoded succesfully.\n");
                }
            else {
                fputc(c,fp);
                p=huffTree_root(h);
            }
        }
        else {
            bool boul = bitset_memberOf(b,x);
            x++;
            if(boul) {
                p=huffTree_rightChild(h,p);
            }
            else {
                p=huffTree_leftChild(h,p);
            }
        }
    }
    fclose(fp);
}

void printUsage() {
    printf("USAGE:\nhuffman [OPTION] [FILE0] [FILE1] [FILE2]\n Options:\n"
           "-encode encodes FILE1 according to frequence analysis done on FILE0."
           " Stores the result in FILE2\n-decode decodes FILE1 according to"
           " frequence analysis done on FILE0. Stores the result in FILE2\n");
}

int main (int argc, char *argv[]) {
    bool ziporunzip=true;
    if(argc!= 5) {
        printUsage();
        exit(0);
    }
    if(strncmp(argv[1],"-encode",7)==0) {
        ziporunzip=true;
    }
    else if(strncmp(argv[1],"-decode",7)==0) {
        ziporunzip=false;
    }
    else {
        printUsage();
        exit(0);
    }
    if(access(argv[2], F_OK) ==-1) {
        printf("no access to FILE0");
        exit(0);
    }
    if(access(argv[3], F_OK) ==-1) {
        printf("no access to FILE1");
        exit(0);
    }
    //int (*h)[2]= malloc(sizeof(int*)*ARR_SIZE);
        int h[ARR_SIZE][2];
    FILE *fp=fopen(argv[2],"r");
    readTextToArray(h,fp);
    fclose(fp);
    treesortHuffArr(h,ARR_SIZE);
    huff_tree *huff = buildHuffTree(h);
 //   free(h);
    if(ziporunzip) {
        FILE *fp0 = fopen(argv[3],"r");
        fseek(fp0, 0L, SEEK_END);
        int fp0size = ftell(fp0);
        fclose(fp0);
        printf("%d bytes read from %s.\n", fp0size,argv[3]);
        huffTree_pos pos= huffTree_root(huff);
        dlist *l = dlist_empty();
        dlist_setMemHandler(l,free);
        while(huffTree_hasRightChild(huff,pos)) {
            bitset *b = bitset_empty();
            treeToList(huff,pos,b,l);
        }
        FILE *fp1= fopen(argv[3], "r");
        listtozip(l,argv[4], fp1);
        fclose(fp1);
        dlist_free(l);
        free(huff->root);
        free(huff);
    }
    else {
        bitset *b=bitsetFromFile(argv[3]);
        unzipFromFile(b,huff,argv[4]);
        bitset_free(b);
        huffTree_free(huff);
    }
    return 0;

}
