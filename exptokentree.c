/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*implements exptokentree.h, a wrapper client for treenode.h designed  */
/*to hold exptokens. If TreeNodes modified by other clients should not */
/*be used by this client                                               */
/*---------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "exptokentree.h"

/*---------------------------------------------------------------------*/
/*Return a new empty TreeNode. If memory is insufficient return NULL   */
/*---------------------------------------------------------------------*/
TreeNode_T ExpTokenTree_new(void){
  return TreeNode_new();
}

/*---------------------------------------------------------------------*/
/*Recursively free oTreeNode, and all of the ExpTokens in it           */
/*---------------------------------------------------------------------*/
void ExpTokenTree_free(TreeNode_T oTreeNode){
  LinkList_T pllChildren, pllNext;
  ExpToken_T petValue;
  assert(oTreeNode != NULL);
  for(pllChildren = TreeNode_getChildren(oTreeNode);
      pllChildren != NULL;
      pllChildren = pllNext){
    ExpTokenTree_free((TreeNode_T)LinkList_getValue(pllChildren));
    pllNext = LinkList_getNext(pllChildren);
    LinkList_free(pllChildren);
  }
  petValue = (ExpToken_T)TreeNode_getValue(oTreeNode);
  if(petValue != NULL)
    ExpToken_free(petValue);
  TreeNode_free(oTreeNode);
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the value of oTreeNode                           */
/*---------------------------------------------------------------------*/
ExpToken_T ExpTokenTree_getValue(TreeNode_T oTreeNode){
  assert(oTreeNode != NULL);
  return (ExpToken_T)TreeNode_getValue(oTreeNode);
}

/*---------------------------------------------------------------------*/
/*Set the value of oTreeNode to oExpToken. If there was already a value*/
/*in oTreeNode free that ExpToken, replace the value, and print a      */
/*warning to stderr                                                    */
/*---------------------------------------------------------------------*/
void ExpTokenTree_setValue(TreeNode_T oTreeNode, ExpToken_T oExpToken){
  assert(oTreeNode != NULL);
  assert(oExpToken != NULL);
  if(TreeNode_getValue(oTreeNode) != NULL){
    ExpToken_free((ExpToken_T)TreeNode_getValue(oTreeNode));
    fprintf(stderr, "%s", "ExpTokenTree_setValue: overwrite exptoken\n");
  }
  TreeNode_setValue(oTreeNode, oExpToken);
}

/*---------------------------------------------------------------------*/
/*Return a list oTreeNode's children                                   */
/*---------------------------------------------------------------------*/
LinkList_T ExpTokenTree_getChildren(TreeNode_T oTreeNode){
  assert(oTreeNode != NULL);
  return TreeNode_getChildren(oTreeNode);
}

/*---------------------------------------------------------------------*/
/*Make oTreeNodeChild a child of oTreeNodeParent. If memory is         */
/*insufficient return 1, otherwise return 0. Children are added to the */
/*end of the list                                                      */
/*---------------------------------------------------------------------*/
int ExpTokenTree_addChild(TreeNode_T oTreeNodeParent,\
			  TreeNode_T oTreeNodeChild){
  assert(oTreeNodeParent != NULL);
  assert(oTreeNodeChild != NULL);
  return TreeNode_addChild(oTreeNodeParent, oTreeNodeChild);
}

/*---------------------------------------------------------------------*/
/*Make oTreeNodeChild a child of oTreeNodeParent. If memory is         */
/*insufficient return 1, otherwise return 0. Children are added to the */
/*front of the list                                                    */
/*---------------------------------------------------------------------*/
int ExpTokenTree_addChildFront(TreeNode_T oTreeNodeParent,\
			       TreeNode_T oTreeNodeChild){
  assert(oTreeNodeParent != NULL);
  assert(oTreeNodeChild != NULL);
  return TreeNode_addChildFront(oTreeNodeParent, oTreeNodeChild);
}

/*---------------------------------------------------------------------*/
/*Compare if oTreeNode1 and oTreeNode2 are equivalent in terms of their*/
/*topology and contents. Return 0 if they are equal, 1 if they are not */
/*---------------------------------------------------------------------*/
int ExpTokenTree_compare(TreeNode_T oTreeNode1, TreeNode_T oTreeNode2){
  LinkList_T pllChild1, pllChild2;
  assert(oTreeNode1 != NULL);
  assert(oTreeNode2 != NULL);
  //recursively compare their children
  pllChild1 = TreeNode_getChildren(oTreeNode1);
  pllChild2 = TreeNode_getChildren(oTreeNode2);
  while((pllChild1 != NULL)|(pllChild2 != NULL)){
    if((pllChild1 == NULL)|(pllChild2 == NULL))
      return 1;
    if(ExpTokenTree_compare((TreeNode_T)LinkList_getValue(pllChild1),\
			    (TreeNode_T)LinkList_getValue(pllChild2))\
       == 1)
      return 1;
    //look at the next branch
    pllChild1 = LinkList_getNext(pllChild1);
    pllChild2 = LinkList_getNext(pllChild2);
  }   
  //compare the two by value
  if(ExpToken_compare((ExpToken_T)TreeNode_getValue(oTreeNode1),	\
		      (ExpToken_T)TreeNode_getValue(oTreeNode2)) != 0)
    return 1;
  return 0;
}

/*---------------------------------------------------------------------*/
/*Look for subtrees within oTreeNodeSearch that match oTreeNodeMatch.  */
/*Return a LinkList containing TreeNode_T objects pointing to the      */
/*matching trees. Return NULL if memory is insufficient, and an empty  */
/*LinkList if no subtrees matched oTreeNodeMatch.                      */
/*---------------------------------------------------------------------*/
LinkList_T ExpTokenTree_subTree(TreeNode_T oTreeNodeSearch,\
				TreeNode_T oTreeNodeMatch){
  LinkList_T oLinkListStorage, oLinkListTemp, oLinkListChild;
  assert(oTreeNodeSearch != NULL);
  assert(oTreeNodeMatch != NULL);
  //setup our storage
  oLinkListStorage = LinkList_new();
  if(oLinkListStorage == NULL)
    return NULL;
  //check every child of oTreeNodeSearch against oTreeNodeMatch
  for(oLinkListChild = TreeNode_getChildren(oTreeNodeSearch);\
      oLinkListChild != NULL;
      oLinkListChild = LinkList_getNext(oLinkListChild)){
    //call it recursively on the children
    oLinkListTemp =\
      ExpTokenTree_subTree((TreeNode_T)\
			   LinkList_getValue(oLinkListChild),\
			   oTreeNodeMatch);
    //if the heap ran out of space
    if(oLinkListTemp == NULL){
      //free all of the already allocated memory then return NULL
      for(; oLinkListStorage != NULL; oLinkListStorage = oLinkListTemp){
	oLinkListTemp = LinkList_getNext(oLinkListStorage);
	LinkList_free(oLinkListStorage);
      }
      return NULL;
    }
    //if there were no matches in the subtree free oLinkListTemp
    else if(LinkList_isEmpty(oLinkListTemp) == 0)
      LinkList_free(oLinkListTemp);
    //else append the results to the end of the current list and continue
    else{
      //if the storage list is empty replace it with this list
      if(LinkList_isEmpty(oLinkListStorage) == 0){
	LinkList_free(oLinkListStorage);
	oLinkListStorage = oLinkListTemp;
	oLinkListTemp = NULL;
      }
      //else just append
      else
	LinkList_setNext(LinkList_getLast(oLinkListStorage),	\
			 oLinkListTemp);
    }
  }
  //now check the current tree for a match
  if(ExpTokenTree_compareAssociative\
     (oTreeNodeSearch, oTreeNodeMatch) == 0){
    //make storage for the match
    oLinkListTemp = LinkList_new();
    //if out of heap memory, free all allocated heap memory
    if(oLinkListTemp == NULL){
      for(; oLinkListStorage != NULL; oLinkListStorage = oLinkListTemp){
	oLinkListTemp = LinkList_getNext(oLinkListStorage);
	LinkList_free(oLinkListStorage);
      }
      return NULL;
    }
    //now store the information about this tree in the new node
    LinkList_setValue(oLinkListTemp, oTreeNodeSearch);
    //if the storage list is empty, replace it with this one
    if(LinkList_isEmpty(oLinkListStorage) == 0){
      LinkList_free(oLinkListStorage);
      oLinkListStorage = oLinkListTemp;
      oLinkListTemp = NULL;
    } 
    //else append it to the end of the storage list
    else
      LinkList_setNext(LinkList_getLast(oLinkListStorage), oLinkListTemp);
  }
  //return the storage list
  return oLinkListStorage;
}

/*---------------------------------------------------------------------*/
/*Return a new copy of oTreeNode and all of it's children, or NULL if  */
/*memory is insufficient                                               */
/*---------------------------------------------------------------------*/
TreeNode_T ExpTokenTree_copy(TreeNode_T oTreeNode){
  TreeNode_T oTreeNodeCopy, oTreeNodeChild;
  ExpToken_T oExpTokenCopy;
  LinkList_T oLinkListChild;
  assert(oTreeNode != NULL);
  oTreeNodeCopy = ExpTokenTree_new();
  if(oTreeNodeCopy == NULL)
    return NULL;
  oExpTokenCopy = ExpToken_copy(ExpTokenTree_getValue(oTreeNode));
  if(oExpTokenCopy == NULL){
    ExpTokenTree_free(oTreeNodeCopy);
    return NULL;
  }
  //copy the value
  ExpTokenTree_setValue(oTreeNodeCopy, oExpTokenCopy);
  oExpTokenCopy = NULL;
  //copy the children
  for(oLinkListChild = ExpTokenTree_getChildren(oTreeNode);\
      oLinkListChild != NULL;\
      oLinkListChild = LinkList_getNext(oLinkListChild)){
    //copy this child tree
    oTreeNodeChild =				\
      ExpTokenTree_copy				\
      (LinkList_getValue(oLinkListChild));
    if(oTreeNodeChild == NULL){
      ExpTokenTree_free(oTreeNodeCopy);
      return NULL;
    }
    //Now add the child tree to the copy
    if(ExpTokenTree_addChild(oTreeNodeCopy, oTreeNodeChild) != 0){
      ExpTokenTree_free(oTreeNodeCopy);
      ExpTokenTree_free(oTreeNodeChild);
      return NULL;
    }
  }
  return oTreeNodeCopy;
}
/*---------------------------------------------------------------------*/
/*A helper function for ExpTokenTree_print. Print the value of this    */
/*node or null if it has not been initialized                          */
/*---------------------------------------------------------------------*/
static void ExpTokenTree_printValue(TreeNode_T oTreeNode){
  if(ExpTokenTree_getValue(oTreeNode) == NULL)
    printf("NULL");
  else
    printf(ExpToken_toString(ExpTokenTree_getValue(oTreeNode)));
  fflush(stdout);
}


/*---------------------------------------------------------------------*/
/*Print an infix representation of oTreeNode to StdOut. NOTE: Infix    */
/*notation is not well defined for operators that operate on more than */
/*two operands. Here if there are operators with three or more operands*/
/*It will print the first operand, then the operator, then the rest    */
/*of the operands. Unary operators are printed before their operand    */
/*---------------------------------------------------------------------*/
void ExpTokenTree_print(TreeNode_T oTreeNode){
  LinkList_T oLinkListChild;
  unsigned int uiLength;
  assert(oTreeNode != NULL);
  oLinkListChild = ExpTokenTree_getChildren(oTreeNode);
  uiLength = LinkList_getLength(oLinkListChild);
  //if there are no children print the exptoken
  if(uiLength == 0u)
    ExpTokenTree_printValue(oTreeNode);
  //if there is one child, print this then the child
  else if(uiLength == 1u){
    ExpTokenTree_printValue(oTreeNode);
    ExpTokenTree_print(LinkList_getValue(oLinkListChild));
  }
  /*if there is more than one child print the first child, then this,  */
  /*then the rest                                                      */
  else{
    ExpTokenTree_print(LinkList_getValue(oLinkListChild));
    ExpTokenTree_printValue(oTreeNode);
    for(oLinkListChild = LinkList_getNext(oLinkListChild);\
	oLinkListChild != NULL;\
	oLinkListChild = LinkList_getNext(oLinkListChild))
      ExpTokenTree_print(LinkList_getValue(oLinkListChild));
  }
}

/*---------------------------------------------------------------------*/
/*Create a copy of oTreeNodeSource where all instances of oTreeNodeFind*/
/*have been replaced by oTreeNodeReplace. Return NULL if memory is     */
/*insufficient for the copy. The resulting tree will be identical to   */
/*oTreeNodeSource if there were no instances of oTreeNodeFind          */
/*---------------------------------------------------------------------*/
TreeNode_T ExpTokenTree_replace(TreeNode_T oTreeNodeSource,\
				TreeNode_T oTreeNodeFind,\
				TreeNode_T oTreeNodeReplace){
  TreeNode_T oTreeNodeCopy, oTreeNodeChild;
  LinkList_T oLinkListChild;
  ExpToken_T oExpTokenCopy;
  assert(oTreeNodeSource != NULL);
  assert(oTreeNodeFind != NULL);
  assert(oTreeNodeReplace != NULL);
  //make storage for the new tree
  oTreeNodeCopy = ExpTokenTree_new();
  if(oTreeNodeCopy == NULL)
    return NULL;
  //copy the value
  oExpTokenCopy = ExpToken_copy(ExpTokenTree_getValue(oTreeNodeSource));
  if(oExpTokenCopy == NULL){
    TreeNode_free(oTreeNodeCopy);
    return NULL;
  }
  ExpTokenTree_setValue(oTreeNodeCopy, oExpTokenCopy);
  //loop through the children
  for(oLinkListChild = ExpTokenTree_getChildren(oTreeNodeSource);\
      oLinkListChild != NULL;\
      oLinkListChild = LinkList_getNext(oLinkListChild)){
    //find replace the child
    oTreeNodeChild =\
      ExpTokenTree_replace((TreeNode_T)\
			   LinkList_getValue(oLinkListChild),\
			   oTreeNodeFind, oTreeNodeReplace);
    //if memory is insufficient then free the copy and return null
    if(oTreeNodeChild == NULL){
      ExpTokenTree_free(oTreeNodeCopy);
      return NULL;
    }
    //add the new child to the copy, checking if it was successful
    if(ExpTokenTree_addChild(oTreeNodeCopy, oTreeNodeChild) != 0){
      ExpTokenTree_free(oTreeNodeCopy);
      ExpTokenTree_free(oTreeNodeChild);
      return NULL;
    }
  }
  //is this tree equal to oTreeNodeFind
  if(ExpTokenTree_compareAssociative\
     (oTreeNodeSource, oTreeNodeFind) == 0){
    //then free oTreeNodeCopy and replace it with oTreeNodeReplace
    ExpTokenTree_free(oTreeNodeCopy);
    oTreeNodeCopy = ExpTokenTree_copy(oTreeNodeReplace);
    //check for memory
    if(oTreeNodeCopy == NULL)
      return NULL; //everything has already been freed at this point
  }
  return oTreeNodeCopy;
}

/*---------------------------------------------------------------------*/
/*Compare oTreeNode1 and oTreeNode2, including a consideration for     */
/*associativity. Return 0 if they are equivalent, 1 if not             */
/*---------------------------------------------------------------------*/
int ExpTokenTree_compareAssociative(TreeNode_T oTreeNode1,\
				    TreeNode_T oTreeNode2){
  LinkList_T oLinkListChild1, oLinkListChild2;
  int iChildCompare, iFoundMatch;
  TreeNode_T oTreeNodeChild1, oTreeNodeChild2;
  assert(oTreeNode1 != NULL);
  assert(oTreeNode2 != NULL);
  //compare these two
  if(ExpToken_compare(ExpTokenTree_getValue(oTreeNode1),\
		      ExpTokenTree_getValue(oTreeNode2)) == 1)
    return 1;
  //check for associativity
  if(ExpToken_isAssociative(ExpTokenTree_getValue(oTreeNode1)) == 0){
    //handle in the normal way, loop through comparing the children
    oLinkListChild1 = ExpTokenTree_getChildren(oTreeNode1);
    oLinkListChild2 = ExpTokenTree_getChildren(oTreeNode2);
    while(!(oLinkListChild1 == NULL & oLinkListChild2 == NULL)){
      //check for one being NULL
      if(oLinkListChild1 == NULL | oLinkListChild2 == NULL)
	return 1;
      //now check if the children are
      oTreeNodeChild1 = (TreeNode_T)LinkList_getValue(oLinkListChild1);
      oTreeNodeChild2 = (TreeNode_T)LinkList_getValue(oLinkListChild2);
      iChildCompare = ExpTokenTree_compareAssociative\
	(oTreeNodeChild1, oTreeNodeChild2);
      if(iChildCompare == 1)
	return 1;
      oLinkListChild1 = LinkList_getNext(oLinkListChild1);
      oLinkListChild2 = LinkList_getNext(oLinkListChild2);
    }
    return 0;
  }
  else{//the case for associativity
    //search for a match for every child
    for(oLinkListChild1 = ExpTokenTree_getChildren(oTreeNode1);
	oLinkListChild1 != NULL;
	oLinkListChild1 = LinkList_getNext(oLinkListChild1)){
      iFoundMatch = 0;
      for(oLinkListChild2 = ExpTokenTree_getChildren(oTreeNode2);
	  oLinkListChild2 != NULL;
	  oLinkListChild2 = LinkList_getNext(oLinkListChild2)){
	oTreeNodeChild1 = (TreeNode_T)LinkList_getValue(oLinkListChild1);
	oTreeNodeChild2 = (TreeNode_T)LinkList_getValue(oLinkListChild2);
	iChildCompare = ExpTokenTree_compareAssociative\
	  (oTreeNodeChild1, oTreeNodeChild2);
	if(iChildCompare == 0)
	  iFoundMatch = 1;
      }
      if(iFoundMatch == 0)
	return 1;
    }
    //repeat for the other children
    for(oLinkListChild2 = ExpTokenTree_getChildren(oTreeNode2);
	oLinkListChild2 != NULL;
	oLinkListChild2 = LinkList_getNext(oLinkListChild2)){
      iFoundMatch = 0;
      for(oLinkListChild1 = ExpTokenTree_getChildren(oTreeNode1);
	  oLinkListChild1 != NULL;
	  oLinkListChild1 = LinkList_getNext(oLinkListChild1)){
	oTreeNodeChild2 = (TreeNode_T)LinkList_getValue(oLinkListChild2);
	oTreeNodeChild1 = (TreeNode_T)LinkList_getValue(oLinkListChild1);
	iChildCompare = ExpTokenTree_compareAssociative\
	  (oTreeNodeChild1, oTreeNodeChild2);
	if(iChildCompare == 0)
	  iFoundMatch = 1;
      }
      if(iFoundMatch == 0)
	return 1;
    }
    return 0;
  }
}

/*---------------------------------------------------------------------*/
/*test exptokentree.c                                                  */
/*---------------------------------------------------------------------*/
/*
int main(void){
  ExpToken_T oExpToken1_1, oExpToken1_2, oExpToken1_3,
    oExpToken2_1, oExpToken2_2, oExpToken2_3, oExpToken4, oExpToken6_1,\
    oExpToken6_2, oExpToken6_3;
  TreeNode_T oTreeNode1_1, oTreeNode1_2, oTreeNode1_3, oTreeNode2_1,\
    oTreeNode2_2, oTreeNode2_3, oTreeNode3, oTreeNode4, oTreeNode5,\
    oTreeNode6_1, oTreeNode6_2, oTreeNode6_3;
  LinkList_T oLinkList, oLinkListNext;
  
  printf("----------------------------------------\n");
  printf("testing exptokentree.c:\n");
  printf("----------------------------------------\n");
  
  //setup our exptokens
  oExpToken1_1 = ExpToken_new();
  oExpToken1_2 = ExpToken_new();
  oExpToken1_3 = ExpToken_new();
  
  ExpToken_setType(oExpToken1_1, ADD);
  ExpToken_setType(oExpToken1_2, OPERAND);
  ExpToken_setType(oExpToken1_3, OPERAND);

  ExpToken_appendChar(oExpToken1_2, 'x');
  ExpToken_appendChar(oExpToken1_3, 'y');

  oExpToken2_1 = ExpToken_new();
  oExpToken2_2 = ExpToken_new();
  oExpToken2_3 = ExpToken_new();
  
  ExpToken_setType(oExpToken2_1, ADD);
  ExpToken_setType(oExpToken2_2, OPERAND);
  ExpToken_setType(oExpToken2_3, OPERAND);

  ExpToken_appendChar(oExpToken2_2, 'x');
  ExpToken_appendChar(oExpToken2_3, 'y');

  oExpToken4 = ExpToken_new();
  ExpToken_setType(oExpToken4, OPERAND);
  ExpToken_appendChar(oExpToken4, 'y');

  oExpToken6_1 = ExpToken_new();
  oExpToken6_2 = ExpToken_new();
  oExpToken6_3 = ExpToken_new();
  
  ExpToken_setType(oExpToken6_1, ADD);
  ExpToken_setType(oExpToken6_2, OPERAND);
  ExpToken_setType(oExpToken6_3, OPERAND);
  ExpToken_appendChar(oExpToken6_2, 'y');
  ExpToken_appendChar(oExpToken6_3, 'x');
  
  //setup the two exptoken trees
  oTreeNode1_1 = ExpTokenTree_new();
  oTreeNode1_2 = ExpTokenTree_new();
  oTreeNode1_3 = ExpTokenTree_new();
  oTreeNode2_1 = ExpTokenTree_new();
  oTreeNode2_2 = ExpTokenTree_new();
  oTreeNode2_3 = ExpTokenTree_new();

  ExpTokenTree_setValue(oTreeNode1_1, oExpToken1_1);
  ExpTokenTree_setValue(oTreeNode1_2, oExpToken1_2);
  ExpTokenTree_setValue(oTreeNode1_3, oExpToken1_3);
  ExpTokenTree_setValue(oTreeNode2_1, oExpToken2_1);
  ExpTokenTree_setValue(oTreeNode2_2, oExpToken2_2);
  ExpTokenTree_setValue(oTreeNode2_3, oExpToken2_3);

  ExpTokenTree_addChild(oTreeNode1_1, oTreeNode1_2);
  ExpTokenTree_addChild(oTreeNode1_1, oTreeNode1_3);
  ExpTokenTree_addChild(oTreeNode2_1, oTreeNode2_2);
  ExpTokenTree_addChild(oTreeNode2_1, oTreeNode2_3);

  oTreeNode4 = ExpTokenTree_new();
  ExpTokenTree_setValue(oTreeNode4, oExpToken4);

  oTreeNode6_1 =ExpTokenTree_new();
  oTreeNode6_2 =ExpTokenTree_new();
  oTreeNode6_3 =ExpTokenTree_new();
  
  ExpTokenTree_setValue(oTreeNode6_1, oExpToken6_1);
  ExpTokenTree_setValue(oTreeNode6_2, oExpToken6_2);
  ExpTokenTree_setValue(oTreeNode6_3, oExpToken6_3);
  ExpTokenTree_addChild(oTreeNode6_1, oTreeNode6_2);
  ExpTokenTree_addChild(oTreeNode6_1, oTreeNode6_3);

  if(ExpTokenTree_compare(oTreeNode1_1, oTreeNode2_1) != 0)
    printf("failed test 1\n");
  
  oLinkList = ExpTokenTree_subTree(oTreeNode1_1, oTreeNode2_1);
  if(LinkList_getValue(oLinkList) != oTreeNode1_1)
    printf("failed test 2\n");
    
  for(; oLinkList != NULL; oLinkList = oLinkListNext){
    oLinkListNext = LinkList_getNext(oLinkList);
    LinkList_free(oLinkList);
  }
  if(ExpTokenTree_compare(oTreeNode1_2, oTreeNode1_3) != 0)
    printf("failed test 3\n");
  if(ExpTokenTree_compare(oTreeNode1_2, oTreeNode2_2) != 0)
    printf("failed test 4\n");
  oLinkList = ExpTokenTree_subTree(oTreeNode1_1, oTreeNode2_2);
  if(LinkList_getValue(oLinkList) != oTreeNode1_2)
    printf("failed test 5\n");
  //if(LinkList_getValue(LinkList_getNext(oLinkList)) != oTreeNode1_3)
  //printf("failed test 6\n");
  printf("got here\n");
  for(; oLinkList != NULL; oLinkList = oLinkListNext){
    oLinkListNext = LinkList_getNext(oLinkList);
    LinkList_free(oLinkList);
  }

  oTreeNode3 = ExpTokenTree_copy(oTreeNode1_1);
  if(ExpTokenTree_compare(oTreeNode3, oTreeNode2_1) != 0)
    printf("failed test 7\n");

  printf("The next line should be: x+x\n");
  ExpTokenTree_print(oTreeNode3);
  printf("\n");

  oTreeNode5 = ExpTokenTree_replace(oTreeNode3, oTreeNode1_2, oTreeNode4);
  printf("The next line should be:y+y\n");
  ExpTokenTree_print(oTreeNode5);
  printf("\n");

  ExpTokenTree_free(oTreeNode5);
  oTreeNode5 = ExpTokenTree_replace(oTreeNode3, oTreeNode3, oTreeNode4);
  printf("The next line should be:y\n");
  ExpTokenTree_print(oTreeNode5);
  printf("\n");

  if(ExpTokenTree_compareAssociative(oTreeNode1_1, oTreeNode6_1) != 0)
    printf("Failed Test 8\n");
  
  
  ExpTokenTree_free(oTreeNode1_1);
  ExpTokenTree_free(oTreeNode2_1);
  ExpTokenTree_free(oTreeNode3);
  ExpTokenTree_free(oTreeNode4);
  ExpTokenTree_free(oTreeNode5);
  ExpTokenTree_free(oTreeNode6_1);
  
  printf("----------------------------------------\n");
  
  return 0;
}
*/
