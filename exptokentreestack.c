/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*exptokentreestack.c implements exptokentreestack.h                   */
/*---------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "exptokentreestack.h"
#include "linkstack.h"

/*---------------------------------------------------------------------*/
/*ExpTokenTreeStacks have a stack that stores ExpTokenTrees            */
/*---------------------------------------------------------------------*/
struct ExpTokenTreeStack
{
  /*the stack*/
  LinkStack_T plsStack;
};

/*---------------------------------------------------------------------*/
/*Return a new empty ExpTokenTreeStack, or NULL if memory is           */
/*insufficient                                                         */
/*---------------------------------------------------------------------*/
ExpTokenTreeStack_T ExpTokenTreeStack_new(void){
  ExpTokenTreeStack_T oExpTokenTreeStack =\
    (ExpTokenTreeStack_T)malloc(sizeof(struct ExpTokenTreeStack));
  if(oExpTokenTreeStack == NULL)
    return NULL;
  oExpTokenTreeStack -> plsStack = LinkStack_new();
  if(oExpTokenTreeStack -> plsStack == NULL){
    free(oExpTokenTreeStack);
    return NULL;
  }
  return oExpTokenTreeStack;
}

/*---------------------------------------------------------------------*/
/*Free all the memory associated with oExpTokenTreeStack. This includes*/
/*the memory associated with all of the ExpTokenTrees in the stack.    */
/*Print a warning to stderr if there were still items in the stack when*/
/*it was freed                                                         */
/*---------------------------------------------------------------------*/
void ExpTokenTreeStack_free(ExpTokenTreeStack_T oExpTokenTreeStack){
  void* pvPopped;
  unsigned int uiItemsRemaining = 0u;
  assert(oExpTokenTreeStack != NULL);
  //pop everything from the stack
  for(pvPopped = LinkStack_pop(oExpTokenTreeStack -> plsStack);
      pvPopped != NULL;
      pvPopped = LinkStack_pop(oExpTokenTreeStack -> plsStack)){
    uiItemsRemaining ++;
    //free the memory of the exptokentree
    ExpTokenTree_free((TreeNode_T)pvPopped);
  }
  //print a warning if there were still items in the stack
  if(uiItemsRemaining > 0u){
    //fprintf					\
    //(stderr,								\
    // "ExpTokenTreeStack_free: %u ExpTokenTrees still on the stack\n",	\
	 // uiItemsRemaining);
  }
  //free the ExpTokenTreeStack
  LinkStack_free(oExpTokenTreeStack -> plsStack);
  oExpTokenTreeStack -> plsStack = NULL;
  free(oExpTokenTreeStack);
}

/*---------------------------------------------------------------------*/
/*Push a new ExpTokenTree onto the stack. Return 0 if successful, 1 if */
/*memory is insufficient                                               */
/*---------------------------------------------------------------------*/
int ExpTokenTreeStack_push(ExpTokenTreeStack_T oExpTokenTreeStack,\
			   TreeNode_T oExpTokenTree){
  assert(oExpTokenTreeStack != NULL);
  assert(oExpTokenTree != NULL);
  return LinkStack_push(oExpTokenTreeStack -> plsStack,\
			(void*)oExpTokenTree);
}

/*---------------------------------------------------------------------*/
/*Pop the top ExpTokenTree off the stack. This frees the memory        */
/*involved in storing the ExpTokenTree but not the memory of the       */
/*ExpTokenTree itself, which the user must free. NULL is returned if   */
/*the stack is empty                                                   */
/*---------------------------------------------------------------------*/
TreeNode_T ExpTokenTreeStack_pop\
(ExpTokenTreeStack_T oExpTokenTreeStack){
  TreeNode_T oExpTokenTree;
  assert(oExpTokenTreeStack != NULL);
  oExpTokenTree = (TreeNode_T)LinkStack_pop(oExpTokenTreeStack ->\
						plsStack);
  return oExpTokenTree;
}

/*---------------------------------------------------------------------*/
/*Return the number of items on the stack                              */
/*---------------------------------------------------------------------*/
unsigned int ExpTokenTreeStack_howMany\
(ExpTokenTreeStack_T oExpTokenTreeStack){
  assert(oExpTokenTreeStack != NULL);
  return LinkStack_howMany(oExpTokenTreeStack -> plsStack);
}

/*---------------------------------------------------------------------*/
/*Print a representation of oExpTokenTreeStack to StdOut               */
/*---------------------------------------------------------------------*/
void ExpTokenTreeStack_print(ExpTokenTreeStack_T oExpTokenTreeStack){
  LinkList_T oLinkListStack;
  unsigned int uiIndex = 0u;
  assert(oExpTokenTreeStack != NULL);
  for(oLinkListStack = LinkStack_getList(oExpTokenTreeStack -> plsStack);
      uiIndex < LinkStack_howMany(oExpTokenTreeStack -> plsStack);
      oLinkListStack = LinkList_getNext(oLinkListStack)){
    ExpTokenTree_print\
      ((TreeNode_T)LinkList_getValue(oLinkListStack));
    printf("\n\n");
    uiIndex++;
  }
}

/*---------------------------------------------------------------------*/
/*test exptokentreestack.c                                             */
/*---------------------------------------------------------------------*/
/*
int main(void){
  ExpToken_T oExpToken1, oExpToken2;
  TreeNode_T oTreeNode1, oTreeNode2, oTreeNode3;
  ExpTokenTreeStack_T oExpTokenTreeStack;
  
  printf("----------------------------------\n");
  printf("Testing exptokentreestack.c:\n");
  printf("----------------------------------\n");
  
  oExpToken1 = ExpToken_new();
  oExpToken2 = ExpToken_new();
  oTreeNode1 = ExpTokenTree_new();
  oTreeNode2 = ExpTokenTree_new();
  oExpTokenTreeStack = ExpTokenTreeStack_new();
  
  ExpToken_setType(oExpToken1, ADD);
  ExpToken_setType(oExpToken2, MULTIPLY);
  ExpTokenTree_setValue(oTreeNode1, oExpToken1);
  ExpTokenTree_setValue(oTreeNode2, oExpToken2);

  ExpTokenTreeStack_push(oExpTokenTreeStack, oTreeNode1);
  ExpTokenTreeStack_push(oExpTokenTreeStack, oTreeNode2);

  if(ExpTokenTreeStack_howMany(oExpTokenTreeStack) != 2u)
    printf("Failed Test 1\n");

  printf("The printout below should have * on top of +\n");
  ExpTokenTreeStack_print(oExpTokenTreeStack);

  oTreeNode3 = ExpTokenTreeStack_pop(oExpTokenTreeStack);
  if(oTreeNode3 != oTreeNode2)
    printf("Failed Test 2\n");
  ExpTokenTree_free(oTreeNode3);
  
  printf("A warning should appear below:\n");
  ExpTokenTreeStack_free(oExpTokenTreeStack);
  
  printf("----------------------------------\n");
  return 0;
}

*/
