#include "List.h"
#include "Common.h"

static void * List_add(void *this, void *node, uint16 size)
{
    uint8 *src, *dst;
    TList *list;
    TNode *pp, *p;

    if(this == NULL) return NULL;

    list = (TList *)this;
    
    dst = (uint8 *)memAlloc(size);
    p = (TNode *)dst;
    
    if(node != NULL){
        src = (uint8 *)node;
        while(size > 0){
            *dst++ = *src++;
            size--;
        }
    }

    if(list->tail == NULL){
        p->next = NULL;
        p->prev = NULL;
        list->head = p;
        list->tail = p;
        list->cnt = 1;
    }
    else{
        pp = list->tail;
        p->next = NULL;
        p->prev = pp;
        pp->next = p;
        list->tail = p;
        list->cnt++;
    }

    return dst;
}

static void List_remove(void *this, void *node)
{
    uint16 idx = 0;
    TList *list;
    TNode *pp, *p;

    if(this == NULL || node == NULL) return;

    list = (TList *)this;
    p = list->head;

    while(p){
        p = p->next;
        if(p == node){
            break;
        }
        idx++;
    }

    if(p != NULL){
        if(p->prev != NULL){
            pp = (TNode *)p->prev;
            pp->next = p->next;
        }
        if(p->next != NULL){
            pp = (TNode *)p->next;
            pp->prev = p->prev;
        }

        list->cnt--;
        memFree(p);
        
        if(list->cnt == 0){
            list->head = NULL;
            list->tail = NULL;
        }
    }
}

static void List_removeAt(void *this, uint16 index)
{
    uint16 idx = 0;
    TList *list;
    TNode *pp, *p;

    if(this == NULL) return;

    list = (TList *)this;
    p = list->head;

    while(p && idx < index){
        p = p->next;
        idx++;
    }

    if(p != NULL){
        if(p->prev != NULL){
            pp = (TNode *)p->prev;
            pp->next = p->next;
        }

        if(p->next != NULL){
            pp = (TNode *)p->next;
            pp->prev = p->prev;
        }

        list->cnt--;
        memFree(p);

        if(list->cnt == 0){
            list->head = NULL;
            list->tail = NULL;
        }
    }
}

static void * List_ElementAt(void *this, uint16 index)
{
    uint16 idx = 0;
    TList *list;
    TNode *p;

    if(this == NULL) return NULL;

    list = (TList *)this;
    p = list->head;

    while(p && idx < index){
        p = p->next;
        idx++;
    }
    
    return p;
}

static void * List_find(void *this, bool (*cmpFunc)(void *node))
{
    TList *list;
    TNode *p;

    if(this == NULL || cmpFunc == NULL) return NULL;

    list = (TList *)this;
    p = list->head;

    while(p){
        if(true == cmpFunc(p)){
            return p;
        }
        p = (TNode *)p->next;
    }

    return NULL;
}

static void List_clear(void *this)
{
    TList *list;
    TNode *pp, *p;

    if(this == NULL) return;

    list = (TList *)this;
    p = list->head;

    while(p){
        pp = (TNode *)p->next;
        memFree(p);
        p = pp;
    }

    list->head = NULL;
    list->tail = NULL;
    list->cnt = 0;
}

extern void List_Init(TList *this)
{
    if(this == NULL) return;

    this->head = NULL;
    this->tail = NULL;
    this->cnt = 0;

    this->add = List_add;
    this->remove = List_remove;
    this->removeAt = List_removeAt;
    this->ElementAt = List_ElementAt;
    this->find = List_find;
    this->clear = List_clear;
}