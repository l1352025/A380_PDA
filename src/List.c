#include "List.h"

static void * List_add(void *this, void *node, uint16 size)
{
    uint8 *src, *dst;
    TList *list;
    TNode *p;

    if(this == NULL) return NULL;

    list = (TList *)this;
    
    src = (uint8 *)node;
    dst = (uint8 *)memAlloc(size);
    p = (TNode *)dst;
    
    if(src != NULL){
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
        p->next = NULL;
        p->prev = list->tail;
        list->tail->next = p;
        list->tail = p;
        list->cnt++;
    }

	list->curr = list->head;
    list->currIdx = 0;

    return dst;
}

static void List_remove(void *this, void *node)
{
    uint16 idx = 0;
    TList *list;
    TNode *p;

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
            (p->prev)->next = p->next;
        }
        else{
            list->head = p->next;
        }
        
        if(p->next != NULL){
            (p->next)->prev = p->prev;
			
            list->curr = p->next;
            list->currIdx = idx;
        }
		else{
            list->tail = p->prev;
            
            list->curr = list->head;
            list->currIdx = 0;
        }

        list->cnt--;
        if(list->cnt == 0){
            list->head = NULL;
            list->tail = NULL;
        }

        memFree(p);
    }
}

static void List_removeAt(void *this, uint16 index)
{
    uint16 idx = 0;
    TList *list;
    TNode *p;

    if(this == NULL) return;

    list = (TList *)this;
    idx = list->currIdx;
    p = list->curr;

    if(index == 0){
        idx = 0;
        p = list->head;
    }

    if(idx <= index){
        while(p && idx != index){
            p = p->next;
            idx++;
        }
    }
    else{
        while(p && idx != index){
            p = p->prev;
            idx--;
        }
    }
    
    if(p != NULL){
        if(p->prev != NULL){
            (p->prev)->next = p->next;
        }
        else{
            list->head = p->next;
        }
        
        if(p->next != NULL){
            (p->next)->prev = p->prev;
			
            list->curr = p->next;
            list->currIdx = idx;
        }
		else{
            list->tail = p->prev;
            
            list->curr = list->head;
            list->currIdx = 0;
        }

        list->cnt--;
        
        if(list->cnt == 0){
            list->head = NULL;
            list->tail = NULL;
        }

        memFree(p);
    }
}

static void * List_ElementAt(void *this, uint16 index)
{
    uint16 idx = 0;
    TList *list;
    TNode *p;

    if(this == NULL) return NULL;

    list = (TList *)this;
    idx = list->currIdx;
    p = list->curr;

    if(index == 0){
        idx = 0;
        p = list->head;
    }

    if(idx <= index){
        while(p && idx != index){
            p = p->next;
            idx++;
        }
    }
    else{
        while(p && idx != index){
            p = p->prev;
            idx--;
        }
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

static void * List_find(void *this, bool (*cmpFunc)(void *node, void *value), void *value)
{
    TList *list;
    TNode *p;

    if(this == NULL || cmpFunc == NULL) return NULL;

    list = (TList *)this;
    p = list->head;

    while(p){
        if(true == cmpFunc(p, value)){
            return p;
        }
        p = p->next;
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
    if(this == NULL) return;

    this->head = NULL;
    this->tail = NULL;
    this->curr = NULL;
    this->currIdx = 0;
    this->cnt = 0;

    this->add = List_add;
    this->remove = List_remove;
    this->removeAt = List_removeAt;
    this->itemAt = List_ElementAt;
    this->find = List_find;
    this->clear = List_clear;
}