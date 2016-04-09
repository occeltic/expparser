/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*linklist.c implements linklist.h, a generic linklist struct          */
/*---------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include "linklist.h"
#include <stdio.h>

/*---------------------------------------------------------------------*/
/*LinkList's have a value and a pointer the next element in the list   */
/*---------------------------------------------------------------------*/
struct LinkList
{
      /*value*/
      void* pvValue;
      /*next*/
      LinkList_T pllNext;
};

/*---------------------------------------------------------------------*/
/*Return a new empty LinkList object, or NULL if memory is insufficient*/
/*---------------------------------------------------------------------*/
LinkList_T LinkList_new(void){
   LinkList_T oLinkList = (LinkList_T)malloc(sizeof(struct LinkList));
   if(oLinkList == NULL)
      return NULL;
   oLinkList -> pvValue = NULL;
   oLinkList -> pllNext = NULL;
   return oLinkList;
}

/*---------------------------------------------------------------------*/
/*Free all the memory associated with this LinkList element. NOTE: This*/
/*is not recursive, and will not free all of the memory in the list,   */
/*just the memory of this element. It is up the user to free all memory*/
/*associated with pvValue, and to properly handle freeing all elements */
/*in the list.                                                         */
/*---------------------------------------------------------------------*/
void LinkList_free(LinkList_T oLinkList){
   assert(oLinkList != NULL);
   oLinkList -> pvValue = NULL;
   oLinkList -> pllNext = NULL;
   free(oLinkList);
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the value of oLinkList                           */
/*---------------------------------------------------------------------*/
void* LinkList_getValue(LinkList_T oLinkList){
   assert(oLinkList != NULL);
   return oLinkList -> pvValue;
}

/*---------------------------------------------------------------------*/
/*Set the value of oLinkList                                           */
/*---------------------------------------------------------------------*/
void LinkList_setValue(LinkList_T oLinkList, void* pvValue){
   assert(oLinkList != NULL);
   oLinkList -> pvValue = pvValue;
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the next element in the list. This will be NULL  */
/*if oLinkList is the last element                                     */
/*---------------------------------------------------------------------*/
LinkList_T LinkList_getNext(LinkList_T oLinkList){
   assert(oLinkList != NULL);
   return oLinkList -> pllNext;
}

/*---------------------------------------------------------------------*/
/*Set the next element of oLinkListThis to oLinkListNext. Provides no  */
/*protection against losing data previously pointed to by oLinkListThis*/
/*---------------------------------------------------------------------*/
void LinkList_setNext(LinkList_T oLinkListThis,\
                      LinkList_T oLinkListNext){
   assert(oLinkListThis != NULL);
   oLinkListThis -> pllNext = oLinkListNext;
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the last element in the list                     */
/*---------------------------------------------------------------------*/
LinkList_T LinkList_getLast(LinkList_T oLinkList){
   LinkList_T oLinkListCurrent;
   assert(oLinkList != NULL);
   for(oLinkListCurrent = oLinkList;\
       oLinkListCurrent -> pllNext != NULL;\
       oLinkListCurrent = oLinkListCurrent -> pllNext){}
   return oLinkListCurrent;
}

/*---------------------------------------------------------------------*/
/*Check whether a list is empty, as in it has no value and no next.    */
/*Return 0 if true, 1 if false                                         */
/*---------------------------------------------------------------------*/
int LinkList_isEmpty(LinkList_T oLinkList){
  assert(oLinkList != NULL);
  if(oLinkList -> pvValue != NULL)
    return 1;
  if(oLinkList -> pllNext != NULL)
    return 1;
  return 0;
}

/*---------------------------------------------------------------------*/
/*Return the number of linkages in a list                              */
/*---------------------------------------------------------------------*/
unsigned int LinkList_getLength(LinkList_T oLinkList){
  LinkList_T oLinkListProbe;
  unsigned int uiNumLinkages = 0u;
  for(oLinkListProbe = oLinkList;
      oLinkListProbe != NULL;
      oLinkListProbe = oLinkListProbe -> pllNext)
    uiNumLinkages++;
  return uiNumLinkages;
}

/*---------------------------------------------------------------------*/
/*Recursively free the linkages in oLinkList. Does not free the values */
/*---------------------------------------------------------------------*/
void LinkList_freeRecursive(LinkList_T oLinkList){
  LinkList_T oLinkListCurrent, oLinkListNext;
  for(oLinkListCurrent = oLinkList;
      oLinkListCurrent != NULL;
      oLinkListCurrent = oLinkListNext){
    oLinkListNext = oLinkListCurrent -> pllNext;
    LinkList_free(oLinkListCurrent);
  }
}

/*---------------------------------------------------------------------*/
/*Test linklist.c                                                      */
/*---------------------------------------------------------------------*/
/*int main(void){
  LinkList_T oLinkList1;
  LinkList_T oLinkList2;
  int i1;
  int i2;
  int* pi1;
  int* pi2;
  i1 = 1;
  i2 = 2;
  pi1 = &i1;
  pi2 = &i2;
  oLinkList1 = LinkList_new();
  oLinkList2 = LinkList_new();
  printf("-------------------------------------------\n");
  printf("Testing linklist.c\n");
  printf("-------------------------------------------\n");
  if(oLinkList1 == NULL | oLinkList2 == NULL)
    printf("Memory is full before testing\n");
  if(LinkList_getValue(oLinkList1) != NULL)
    printf("Failed Test 1\n");
  if(LinkList_getNext(oLinkList1) != NULL)
    printf("Failed Test 2\n");
  LinkList_setValue(oLinkList1, pi1);
  LinkList_setValue(oLinkList2, pi2);
  if((int*)LinkList_getValue(oLinkList1)!=pi1)
    printf("Failed Test 3\n");
  if((int*)LinkList_getValue(oLinkList2)!=pi2)
    printf("Failed Test 4\n");
  LinkList_setNext(oLinkList1, oLinkList2);
  if(LinkList_getNext(oLinkList1) != oLinkList2)
    printf("Failed Test 5\n");
  if(LinkList_getLast(oLinkList1) != oLinkList2)
    printf("Failed Test 6\n");
*/
  /*properly free and clear the lists*/
/*
  LinkList_free(LinkList_getLast(oLinkList1));
  LinkList_setNext(oLinkList1, NULL);
  LinkList_free(oLinkList1);
  return 0;
}
*/
