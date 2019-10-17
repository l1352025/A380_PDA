#include "List.h"


#include "Common.h"

static void * List_newItem(uint16 size)
{
    return memAlloc(size);
}

static void List_add(void *this, void *node, uint16 size)
{
    TList *list = (TList *)this;
    TNode *pp, *p;
    uint8 *src, *dst;

    LogPrint(" list add in...");

    src = (uint8 *)node;
    dst = (uint8 *)memAlloc(size);
    p = (TNode *)dst;
    
    while(size > 0){
        *dst++ = *src++;
        size--;
    }

    LogPrint(" malloc ok ...");
    
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

    LogPrint(" add ok ...");
}

static void List_remove(void *this, void *node)
{
    uint16 idx = 0;
    TList *list = (TList *)this;
    TNode *pp, *p = list->head;

    if(p == NULL) return;

    while(p){
        p = p->next;
        if(p == node){
            break;
        }
        idx++;
    }

    if(p != NULL){
        if(p->prev != NULL){
            pp = p->prev;
            pp->next = p->next;
        }
        if(p->next != NULL){
            pp = p->next;
            pp->prev = p->prev;

            list->curr = p->next;
            list->currIdx = idx;
        }
        else{
            list->curr = list->head;
            list->currIdx = 0;
        }

        list->cnt--;
        memFree(p);
    }
}

static void List_removeAt(void *this, uint16 index)
{
    uint16 idx = 0;
    TList *list = (TList *)this;
    TNode *pp, *p = list->head;

    if(p == NULL) return;

    if(index >= list->currIdx){
        idx = list->currIdx;
        p = list->curr;
    }

    while(p && idx < index){
        p = p->next;
        idx++;
    }

    if(p != NULL){
        if(p->prev != NULL){
            pp = p->prev;
            pp->next = p->next;
        }

        if(p->next != NULL){
            pp = p->next;
            pp->prev = p->prev;

            list->curr = p->next;
            list->currIdx = idx;
        }
        else{
            list->curr = list->head;
            list->currIdx = 0;
        }

        list->cnt--;
        memFree(p);
    }
}

static void * List_ElementAt(void *this, uint16 index)
{
    uint16 idx = 0;
    TList *list = (TList *)this;
    TNode *p = list->head;

    if(p == NULL) return NULL;

    if(index >= list->currIdx){
        idx = list->currIdx;
        p = list->curr;
    }

    while(p && idx < index){
        p = p->next;
        idx++;
    }

    if(p != NULL){
        list->curr = p;
        list->currIdx = idx;
    }
    else{
        p = NULL;
    }
    
    return p;
}

static void * List_find(void *this, bool (*cmpFunc)(void *node))
{
    TList *list = (TList *)this;
    TNode *p = list->head;

    if(p == NULL) return NULL;

    while(p){
        if(true == cmpFunc(p)){
            return p;
        }
        p = p->next;
    }

    return NULL;
}

static void List_clear(void *this)
{
    TList *list = (TList *)this;
    TNode *pp, *p = list->head;

    if(list->head == NULL) return;

    while(p){
        pp = p->next;
        memFree(p);
        p = pp;
    }

    list->head = NULL;
    list->tail = NULL;
    list->curr = NULL;
    list->currIdx = 0;
    list->cnt = 0;
}

extern void List_Init(TList *this)
{
    this->head = NULL;
    this->tail = NULL;
    this->curr = NULL;
    this->currIdx = 0;
    this->cnt = 0;

    this->newItem = List_newItem;
    this->add = List_add;
    this->remove = List_remove;
    this->removeAt = List_removeAt;
    this->ElementAt = List_ElementAt;
    this->find = List_find;
    this->clear = List_clear;
}