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

typedef struct{
    void *prev;
    void *next;
}TNode;

typedef struct{
    uint16 cnt;
    TNode *head;
    TNode *tail;
    TNode *curr;
    uint16 currIdx;
    void * (*newItem)(uint16 size);
    void (*add)(void *this, void *node, uint16 size);
    void (*remove)(void *this, void *node);
    void (*removeAt)(void *this, uint16 index);
    void * (*ElementAt)(void *this, uint16 index);
    void * (*find)(void *this, bool (*cmpFunc)(void *node));
    void (*clear)(void *this);
}TList;

extern void List_Init(TList *this);

extern void    _free(void *ptr);
extern void *  _malloc(size_t len );
#define memFree(p)  _free(p);
#define memAlloc(size)  _malloc(size);

#endif