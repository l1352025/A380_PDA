#ifndef List_H
#define List_H

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned char bool;
#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif
#ifndef NULL
#define NULL   0
#endif

/**
 * list node struct base, your owr node must be have the point *prev and *next at first
 * like this :
 * typedef struct YourNode{
 *   struct YourNode *prev;
 *   struct YourNode *next;
 *   < node's other vars >;  
 * }YourNode_alias;
*/
typedef struct Tnode{
    struct Tnode *prev;
    struct Tnode *next;
}TNode;

typedef struct{
    uint16 cnt;
    uint16 currIdx;
    TNode *head;
    TNode *tail;
    TNode *curr;

    /**
     * add node in list, and set the value if 'node' not NULL
    */
    void * (*add)(void *this, void *node, uint16 size);

    void (*remove)(void *this, void *node);                 
    void (*removeAt)(void *this, uint16 index);
    void * (*itemAt)(void *this, uint16 index);     

    /**
     * find node contarns the 'value' with function 'cmpFunc'
    */
    void * (*find)(void *this, bool (*cmpFunc)(void *node, void *value), void *value); 

    void (*clear)(void *this);
}TList;

extern void List_Init(TList *this);


#ifdef USE_WIN32    // win32
    #include <stdlib.h>
    #define memFree(p)  free(p);
    #define memAlloc(size)  malloc(size);
#else               // A380-arm
    extern void    _free(void *ptr);
    extern void *  _malloc(size_t len );
    #define memFree(p)  _free(p);
    #define memAlloc(size)  _malloc(size);
#endif

#endif