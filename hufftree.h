

#include <stdbool.h>

#ifndef __DATA
#define __DATA
typedef void *data;
//typedef int *data;
#endif

#ifndef __MEMFREEDATAFUNC
#define __MEMFREEDATAFUNC
typedef void memFreeFunc(data);
#endif

typedef struct Huff_Node {
    struct Huff_Node *parent;
    struct Huff_Node *leftChild;
    struct Huff_Node *rightChild;
    data character;
    data weight;
} node;

typedef node * huffTree_pos;

typedef struct {
    huffTree_pos root;
    memFreeFunc *freeFunc;
} huff_tree;


/*
Syfte: Skapa ett nytt binärt träd med en rotnod
Returvärde: pekare till det nyskapade trädet
Kommentarer:
*/
huff_tree *huffTree_create();

/*
Syfte: Installera en minneshanterare för trädet så att den kan ta över
       ansvaret för att avallokera minnet för värdena då de ej finns kvar
       i trädet mer.
Parametrar: t - trädet
            f - en funktionspekare till en funktion som tar in ett värde
                av typen data (void pekare) och avallokerar minnet för värdet
                den pekar på (ett av den typ av värden som lagras i trädet).
Kommentarer: Bör anropas direkt efter att trädet skapats om funktionaliteten
             ska användas. Trädet funkar även utan att denna funktion anropas,
             men det är då upp till användaren av datatypen att avallokera allt
             minne för datat som lagras i trädet.
*/
void huffTree_setMemHandler(huff_tree *t, memFreeFunc *f);

/*
Syfte: Hämta positionen för rotnoden
Parametrar: trädet
Returvärde: positionen för rotnden
Kommentarer:
*/
huffTree_pos huffTree_root(huff_tree *tree);

/*
Syfte: Kontrollera om en nod i trädet har ett vänsterbarn
Parametrar: tree - trädet
            n - positionen som man vill undersöka om den har ett vänsterbarn
Returvärde: true om et vänsterbarn fanns, false annars
Kommentarer:
*/
bool huffTree_hasLeftChild(huff_tree *tree,huffTree_pos n);

/*
Syfte: Kontrollera om ett högerbarn finns till en given nod
Parametrar: tree - trädet
            n - positionen som man vill undersöka om den har ett högerbarn
Returvärde: true om et högerbarn fanns, false annars
Kommentarer:
*/
bool huffTree_hasRightChild(huff_tree *tree,huffTree_pos n);

/*
Syfte: Hämta högra barnets position för en given nod
Parametrar: tree - trädet
            n - positionen vars högerbarn man vill hämta
Returvärde:  positionen för högerbarnet
Kommentarer: Ej definierad om ett högerbarn inte finns
*/
huffTree_pos huffTree_rightChild(huff_tree *tree,huffTree_pos n);

/*
Syfte: Hämta vänstra barnets position för en given nod
Parametrar: tree - trädet
            n - positionen vars vänsterbarn man vill hämta
Returvärde: positionen för vänsterbarnet
Kommentarer: Ej definierad om ett vänsterbarn inte finn
*/
huffTree_pos huffTree_leftChild(huff_tree *tree,huffTree_pos n);

/*
Syfte: Hämta positionen för föräldern
Parametrar: tree - trädet
            n - positionen vars förälder man vill hämta
Returvärde: positionen för föräldern
Kommentarer: Ej definierad för rotnoden
*/
huffTree_pos huffTree_parent(huff_tree *tree,huffTree_pos n);

/*
Syfte: Hämta en nods etikett
Parametrar: tree - trädet
            n - positionen för noden
Returvärde: värdet på etiketten
Kommentarer:
*/
data huffTree_inspectCharacter(huff_tree *tree,huffTree_pos n);
data huffTree_inspectWeight(huff_tree *tree,huffTree_pos n);
/*
Syfte: Kontrollera om en etikett finns i noden
Parametrar: tree - trädet
            n - positionen för noden
Returvärde: true om noden har en ettikett annars false
Kommentarer:
*/
bool huffTree_hasCharacter(huff_tree *tree,huffTree_pos n);
bool huffTree_hasWeight(huff_tree *tree,huffTree_pos n);
/*
Syfte: Sätta en etikett för en nod
Parametrar: tree - trädet
            character - etiketten för noden
            n - positionen för noden
Kommentarer:
*/
void huffTree_setValues(huff_tree *tree,data character, data weight,huffTree_pos n);

/*
Syfte: Sätt in ett nytt barn till höger om en nod
Parametrar: tree - trädet
            n - positionen för föräldranoden
Returvärde: positionen för den nya noden
Kommentarer: Om ett högerbarn redan fanns kommer detta tas bort.
*/
huffTree_pos huffTree_insertRight(huff_tree *tree,huffTree_pos n);

/*
Syfte: Sätt in ett nytt barn till höger om en nod
Parametrar: tree - trädet
            n - positionen för föräldranoden
Returvärde: positionen för den nya noden
Kommentarer: Om ett vänsterbarn redan fanns kommer detta tas bort.
*/
huffTree_pos huffTree_insertLeft(huff_tree *tree,huffTree_pos n);


huff_tree *huffTree_merge (huff_tree *treeLeft, huff_tree *treeRight);
/*
Syfte: Ta bort en nod ur trädet
Parametrar: tree - trädet
            n - positionen för noden
Returvärde: position för föräldranoden till noden som togs bort
Kommentarer: Om noden som ska tas bort har barn kommer även dessa tas bort
*/
huffTree_pos huffTree_deleteNode(huff_tree *tree,huffTree_pos n);

/*
Syfte: Avallokera minnet för trädet
Parametrar: tree - trädet
Kommentarer: efter anropet så är tree ej längre definierat
*/
void huffTree_free(huff_tree *tree);


