/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*linkstack.c implemenets linkstack.h, a generic stack structure       */
/*---------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include "linklist.h"
#include "linkstack.h"
#include <stdio.h>

/*---------------------------------------------------------------------*/
/*LinkStacks just maintain an ordered list of the elements in them     */
/*---------------------------------------------------------------------*/
struct LinkStack
{
  /*the list*/
  LinkList_T pllStack;
  /*how many items are on the stack*/
  unsigned int uiNum;
};

/*---------------------------------------------------------------------*/
/*Return a new empty LinkStack object, or NULL if memory is            */
/*insufficient                                                         */
/*---------------------------------------------------------------------*/
LinkStack_T LinkStack_new(void){
   LinkStack_T oLinkStack = (LinkStack_T)malloc(\
      sizeof(struct LinkStack));
   if(oLinkStack == NULL)
      return NULL;
   oLinkStack -> pllStack = LinkList_new();
   if(oLinkStack -> pllStack == NULL){
      free(oLinkStack);
      return NULL;
   }
   oLinkStack -> uiNum = 0u;
   return oLinkStack;
}

/*---------------------------------------------------------------------*/
/*Free all memory associated with this LinkStack. Note this only frees */
/*the stack and it's pointers. To free the memory associated with the  */
/*elements in oLinkStack is the user's responsibility                  */
/*---------------------------------------------------------------------*/
void LinkStack_free(LinkStack_T oLinkStack){
   assert(oLinkStack != NULL);
   LinkList_free(oLinkStack->pllStack);
   oLinkStack -> pllStack = NULL;
   free(oLinkStack);
}

/*---------------------------------------------------------------------*/
/*Push a new element onto the stack. Return 0 if successful, 1 if      */
/*memory is insufficient.                                              */
/*---------------------------------------------------------------------*/
int LinkStack_push(LinkStack_T oLinkStack, void* pvValue){
   LinkList_T oLinkList;
   assert(oLinkStack != NULL);
   assert(pvValue != NULL);
   oLinkList = LinkList_new();
   if(oLinkList == NULL)
      return 1;
   LinkList_setValue(oLinkList, pvValue);
   LinkList_setNext(oLinkList, oLinkStack->pllStack);
   oLinkStack->pllStack = oLinkList;
   oLinkStack->uiNum++;
   return 0;
}

/*---------------------------------------------------------------------*/
/*Pop the top element off the stack. This frees the memory involved in */
/*storing the element, but not the memory associated with the item     */
/*itself, which must be freed by the user. NULL is returned if the     */
/*stack is empty                                                       */
/*---------------------------------------------------------------------*/
void* LinkStack_pop(LinkStack_T oLinkStack){
   void* pvValue;
   LinkList_T pllNext;
   assert(oLinkStack != NULL);
   pvValue = LinkList_getValue(oLinkStack -> pllStack);
   if(oLinkStack->uiNum == 0u)
      return NULL;
   pllNext = LinkList_getNext(oLinkStack -> pllStack);
   LinkList_free(oLinkStack -> pllStack);
   oLinkStack -> pllStack = pllNext;
   oLinkStack -> uiNum--;
   return pvValue;
}

/*---------------------------------------------------------------------*/
/*Return the number of items on the stack                              */
/*---------------------------------------------------------------------*/
unsigned int LinkStack_howMany(LinkStack_T oLinkStack){
  assert(oLinkStack != NULL);
  return oLinkStack -> uiNum;
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the value of the top element of the stack without*/
/*popping it. NOTE: use with extreme caution, popping is preferrable   */
/*---------------------------------------------------------------------*/
void* LinkStack_getValue(LinkStack_T oLinkStack){
  assert(oLinkStack != NULL);
  return LinkList_getValue(oLinkStack -> pllStack);
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the list that functions as the stack. NOTE: This */
/*should only be used by client functions that will not modify the     */
/*elements in the stack.                                               */
/*---------------------------------------------------------------------*/
LinkList_T LinkStack_getList(LinkStack_T oLinkStack){
  assert(oLinkStack != NULL);
  return oLinkStack -> pllStack;
}

/*---------------------------------------------------------------------*/
/*test linkstack.c                                                     */
/*---------------------------------------------------------------------*/
/*int main(void){
  LinkStack_T oLinkStack;
  int i1, i2;
  int* pi1, pi2;
  printf("----------------------------------------\n");
  printf("Testing linkstack.c\n");
  printf("----------------------------------------\n");
  oLinkStack = LinkStack_new();
  if(oLinkStack == NULL)
    printf("Failed Test 1\n");
  i1 = 1;
  i2 = 2;
  pi1 = &i1;
  pi2 = &i2;
  if(LinkStack_push(oLinkStack, pi1) != 0)
    printf("Failed Test 3\n");
  if(LinkStack_push(oLinkStack, pi2) != 0)
    printf("Failed Test 4\n");
  if((int*)LinkStack_pop(oLinkStack) != pi2)
    printf("Failed Test 5\n");
  if((int*)LinkStack_pop(oLinkStack) != pi1)
    printf("Failed Test 6\n");
  LinkStack_free(oLinkStack);
  printf("----------------------------------------\n");
  return 0;
}
*/
