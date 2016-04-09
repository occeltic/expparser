/*---------------------------------------------------------------------*/
/*exptokenstack.c implements exptokenstack.h, a simple stack struct    */
/*that holds exptokens                                                 */
/*---------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "exptokenstack.h"
#include "linkstack.h"

/*---------------------------------------------------------------------*/
/*ExpTokenStack's have a stack that stores exptokens                   */
/*---------------------------------------------------------------------*/
struct ExpTokenStack
{
  /*the stack*/
  LinkStack_T plsStack;
};

/*---------------------------------------------------------------------*/
/*Return a new empty ExpTokenStack, or NULL if memory is insufficient  */
/*---------------------------------------------------------------------*/
ExpTokenStack_T ExpTokenStack_new(void){
  ExpTokenStack_T oExpTokenStack =\
    (ExpTokenStack_T)malloc(sizeof(struct ExpTokenStack));
  if(oExpTokenStack == NULL)
    return NULL;
  oExpTokenStack -> plsStack = LinkStack_new();
  if(oExpTokenStack -> plsStack == NULL){
    free(oExpTokenStack);
    return NULL;
  }
  return oExpTokenStack;
}

/*---------------------------------------------------------------------*/
/*Free all the memory associated with oExpTokenStack. This includes the*/
/*memory associated with all of the ExpTokens in the stack. Print a    */
/*warning to stderr if there were still items left in the stack when it*/
/*was freed                                                            */
/*---------------------------------------------------------------------*/
void ExpTokenStack_free(ExpTokenStack_T oExpTokenStack){
  void* pvPopped;
  unsigned int uiItemsRemaining = 0u;
  assert(oExpTokenStack != NULL);
  //check if there was still anything in the stack
  for(pvPopped = LinkStack_pop(oExpTokenStack -> plsStack);
      pvPopped != NULL;
      pvPopped = LinkStack_pop(oExpTokenStack -> plsStack)){
    uiItemsRemaining ++;
    //free the memory associated with the exptoken
    ExpToken_free((ExpToken_T)pvPopped);
  }
  //print a warning if there were still items in the stack
  if(uiItemsRemaining > 0u){
    //fprintf(stderr,							\
    //"ExpTokenStack_free: %u ExpTokens remained on the stack\n",	\
    //	    uiItemsRemaining);
  }
  //free the ExpTokenStack
  LinkStack_free(oExpTokenStack -> plsStack);
  oExpTokenStack -> plsStack = NULL;
  free(oExpTokenStack);
}

/*---------------------------------------------------------------------*/
/*Push a new ExpToken onto the stack. Return 0 if successful, 1 if     */
/*memory is insufficient                                               */
/*---------------------------------------------------------------------*/
int ExpTokenStack_push(ExpTokenStack_T oExpTokenStack,\
		       ExpToken_T oExpToken){
  assert(oExpTokenStack != NULL);
  assert(oExpToken != NULL);
  return LinkStack_push(oExpTokenStack -> plsStack, (void*)oExpToken);
}

/*---------------------------------------------------------------------*/
/*Pop the top ExpToken off the stack. This free the memory involved in */
/*storing the ExpToken, but not the memory of the ExpToken itself,     */
/*which the user must free. NULL is returned if the stack is empty.    */
/*---------------------------------------------------------------------*/
ExpToken_T ExpTokenStack_pop(ExpTokenStack_T oExpTokenStack){
  assert(oExpTokenStack != NULL);
  ExpToken_T oExpToken =\
    (ExpToken_T)LinkStack_pop(oExpTokenStack -> plsStack);
  return oExpToken;
}

/*---------------------------------------------------------------------*/
/*Return the number of items on the stack                              */
/*---------------------------------------------------------------------*/
unsigned int ExpTokenStack_howMany(ExpTokenStack_T oExpTokenStack){
  assert(oExpTokenStack != NULL);
  return LinkStack_howMany(oExpTokenStack -> plsStack);
}

/*---------------------------------------------------------------------*/
/*Compare the precedence of oExpToken with the ExpToken on top of      */
/*oExpTokenStack                                                       */
/*---------------------------------------------------------------------*/
int ExpTokenStack_precedence(ExpTokenStack_T oExpTokenStack,\
			     ExpToken_T oExpToken){
  assert(oExpTokenStack != NULL);
  assert(oExpToken != NULL);
  return ExpToken_precedence\
    (oExpToken,LinkStack_getValue(oExpTokenStack -> plsStack));
}

/*---------------------------------------------------------------------*/
/*Return the type of the ExpToken on top of the stack. Do not call on  */
/*an empty stack as it will cause an error                             */
/*---------------------------------------------------------------------*/
enum ExpTokenType ExpTokenStack_getType(ExpTokenStack_T oExpTokenStack){
  assert(oExpTokenStack != NULL);
  return ExpToken_getType\
    (LinkStack_getValue(oExpTokenStack -> plsStack));
}

/*---------------------------------------------------------------------*/
/*Print a representation of oExpTokenStack to StdOut                   */
/*---------------------------------------------------------------------*/
void ExpTokenStack_print(ExpTokenStack_T oExpTokenStack){
  LinkList_T oLinkListStack;
  unsigned int uiIndex = 0u;
  assert(oExpTokenStack != NULL);
  for(oLinkListStack = LinkStack_getList(oExpTokenStack -> plsStack);
      uiIndex < LinkStack_howMany(oExpTokenStack -> plsStack);
      oLinkListStack = LinkList_getNext(oLinkListStack)){
    fprintf(stdout, "%s\n", ExpToken_toString\
	    ((ExpToken_T)LinkList_getValue(oLinkListStack)));
    uiIndex ++;
  }
}

/*---------------------------------------------------------------------*/
/*test exptokenstack.c                                                 */
/*---------------------------------------------------------------------*/
/*
int main(void){
  ExpTokenStack_T oExpTokenStack;
  ExpToken_T oExpToken1, oExpToken2, oExpTokenPop;
  printf("----------------------------------------------\n");
  printf("Testing exptokenstack.c:\n");
  printf("----------------------------------------------\n");
  oExpToken1 = ExpToken_new();
  oExpToken2 = ExpToken_new();
  oExpTokenStack = ExpTokenStack_new();
  ExpToken_setType(oExpToken1, OPERAND);
  ExpToken_setType(oExpToken2, MULTIPLY);
  ExpToken_appendChar(oExpToken1, 'x');
  ExpTokenStack_push(oExpTokenStack, oExpToken1);
  ExpTokenStack_push(oExpTokenStack, oExpToken2);
  printf("An error message should appear below:\n");
  ExpTokenStack_free(oExpTokenStack); 
  oExpToken1 = ExpToken_new();
  oExpToken2 = ExpToken_new();
  oExpTokenStack = ExpTokenStack_new();
  ExpToken_setType(oExpToken1, OPERAND);
  ExpToken_setType(oExpToken2, MULTIPLY);
  ExpToken_appendChar(oExpToken1, 'x');
  ExpTokenStack_push(oExpTokenStack, oExpToken1);
  ExpTokenStack_push(oExpTokenStack, oExpToken2);
  printf("The printout should have * on top of x:\n");
  ExpTokenStack_print(oExpTokenStack);
  oExpTokenPop = ExpTokenStack_pop(oExpTokenStack);
  if(oExpTokenPop != oExpToken2)
    printf("Failed Test 1\n");
  ExpToken_free(oExpTokenPop);
  oExpTokenPop = ExpTokenStack_pop(oExpTokenStack);
  if(oExpTokenPop != oExpToken1)
    printf("Failed Test 2\n");
  ExpToken_free(oExpTokenPop);
  printf("No error message should appear below:\n");
  ExpTokenStack_free(oExpTokenStack);
  printf("----------------------------------------------\n");
  return 0;

}
*/  
