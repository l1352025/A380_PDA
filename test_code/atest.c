/**
 * 编译时需定义宏 USE_WIN32
*/
#define USE_WIN32

#include <stdio.h>
#include "List.h"
#include "List.c"
#include "Upgrade.h"

void ShowDocList(TList *docs)
{
    uint8 *ptr;
    DocInfo *docItem = (DocInfo *)docs->head;

    if(docItem == NULL){
        printf("List is Empty ! \n");
    }
	
	while(docItem != NULL){

		switch (docItem->state){
		case UpgrdState_NotStart: ptr = "not"; break;
		case UpgrdState_PktWait: ptr = "wait"; break;
		case UpgrdState_Finish: ptr = "ok"; break;
		default: break;
		}

		printf("%s  %s\n", docItem->mtrNo, ptr);

        docItem = docItem->next;
	}
}

bool IsMtrNoEqual(void *node, void *mtrNo)
{
    uint8 *src = ((DocInfo *)node)->mtrNo;
    uint8 *dst = (uint8 *)mtrNo;
    uint16 i;

    if(node == NULL || mtrNo == NULL) return false;

    for(i = 0; i < 20 && *src != 0x00; i++)
    {
        if(*src++ != *dst++) break;
    }
    
    return (*src == *dst);
}

int main()
{
    DocInfo docItem, *doc;
    TList DocList;
    
    // init
    List_Init(&DocList);

    // add 
    sprintf(docItem.mtrNo, "111111111111");
    docItem.state = (uint8)0;
    DocList.add(&DocList, &docItem, sizeof(DocInfo));
    sprintf(docItem.mtrNo, "222222222222");
    docItem.state = (uint8)1;
    DocList.add(&DocList, &docItem, sizeof(DocInfo));
    sprintf(docItem.mtrNo, "333333333333");
    docItem.state = (uint8)2;
    DocList.add(&DocList, &docItem, sizeof(DocInfo));
    sprintf(docItem.mtrNo, "444444444444");
    docItem.state = (uint8)2;
    DocList.add(&DocList, &docItem, sizeof(DocInfo));

    ShowDocList(&DocList);

    // del
    DocList.removeAt(&DocList, 3);
    DocList.removeAt(&DocList, 1);
    DocList.removeAt(&DocList, 0);
    ShowDocList(&DocList);

    // add
    sprintf(docItem.mtrNo, "555555555555");
    docItem.state = (uint8)2;
    DocList.add(&DocList, &docItem, sizeof(DocInfo));
    sprintf(docItem.mtrNo, "666666666666");
    docItem.state = (uint8)2;
    DocList.add(&DocList, &docItem, sizeof(DocInfo));

    // get
    doc = DocList.itemAt(&DocList, 0);
    printf("get Result 0 : %s \n", (doc == NULL ? "failed" : (char *)doc->mtrNo));
    doc = DocList.itemAt(&DocList, 5);
    printf("get Result 5 : %s \n", (doc == NULL ? "failed" : (char *)doc->mtrNo));

    // find
    doc = DocList.find(&DocList, IsMtrNoEqual, "555555555555");
    printf("find Result : %s \n", (doc == NULL ? "failed" : (char *)doc->mtrNo));

    // clear
    DocList.clear(&DocList);
    ShowDocList(&DocList);

    getchar();
}