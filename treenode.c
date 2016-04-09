/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*treenode.c implements treenode.h, a generic tree structure with      */
/*variable numbers of branches from each node                          */
/*---------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include "treenode.h"
#include <stdio.h>

/*---------------------------------------------------------------------*/
/*A TreeNode is a structure for storing hierarchically ordered elements*/
/*---------------------------------------------------------------------*/
struct TreeNode
{
      /*value*/
      void* pvValue;
      /*children*/
      LinkList_T pllChildren;
};

/*---------------------------------------------------------------------*/
/*Return a new empty TreeNode object, or NULL if memory is insufficient*/
/*---------------------------------------------------------------------*/
TreeNode_T TreeNode_new(void){
   TreeNode_T oTreeNode = (TreeNode_T)malloc(sizeof(struct TreeNode));
   if(oTreeNode == NULL)
      return NULL;
   oTreeNode -> pvValue = NULL;
   oTreeNode -> pllChildren = LinkList_new();
   if(oTreeNode -> pllChildren == NULL){
      free(oTreeNode);
      return NULL;
   }
   return oTreeNode;
}

/*---------------------------------------------------------------------*/
/*Free all the memory associated with oTreeNode. NOTE: This free is not*/
/*recursive, nor does it touch the value of this node. Managing this   */
/*memory and making a recursive free is up to the user.                */
/*---------------------------------------------------------------------*/
void TreeNode_free(TreeNode_T oTreeNode){
  assert(oTreeNode != NULL);
  LinkList_free(oTreeNode -> pllChildren);
  oTreeNode -> pvValue = NULL;
  oTreeNode -> pllChildren = NULL;
  free(oTreeNode);
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the value of oTreeNode                           */
/*---------------------------------------------------------------------*/
void* TreeNode_getValue(TreeNode_T oTreeNode){
  assert(oTreeNode != NULL);
  return oTreeNode -> pvValue;
}

/*---------------------------------------------------------------------*/
/*Set the value of oTreeNode to pvValue                                */
/*---------------------------------------------------------------------*/
void TreeNode_setValue(TreeNode_T oTreeNode, void* pvValue){
  assert(oTreeNode != NULL);
  oTreeNode -> pvValue = pvValue;
}

/*---------------------------------------------------------------------*/
/*Return a list of oTreeNode's children                                */
/*---------------------------------------------------------------------*/
LinkList_T TreeNode_getChildren(TreeNode_T oTreeNode){
  assert(oTreeNode != NULL);
  return LinkList_getNext(oTreeNode -> pllChildren);
}

/*---------------------------------------------------------------------*/
/*Make oTreeNodeChild a child of oTreeNodeParent. If memory is         */
/*insufficient return 1, otherwise return 0. Children are added to the */
/*end of the list                                                      */
/*---------------------------------------------------------------------*/
int TreeNode_addChild(TreeNode_T oTreeNodeParent,\
		      TreeNode_T oTreeNodeChild){
  LinkList_T oLinkList;
  assert(oTreeNodeParent != NULL);
  assert(oTreeNodeChild != NULL);
  oLinkList = LinkList_new();
  if(oLinkList == NULL)
    return 1;
  LinkList_setValue(oLinkList, oTreeNodeChild);
  LinkList_setNext(LinkList_getLast(oTreeNodeParent->pllChildren),\
		   oLinkList);
  return 0;
}

/*---------------------------------------------------------------------*/
/*Make oTreeNodeChild a child of oTreeNodeParent. If memory is         */
/*insufficient return 1, otherwise return 0. Children are added to the */
/*front of the list. The first null null list remains in first position*/
/*---------------------------------------------------------------------*/
int TreeNode_addChildFront(TreeNode_T oTreeNodeParent,\
			   TreeNode_T oTreeNodeChild){
  LinkList_T oLinkList;
  assert(oTreeNodeParent != NULL);
  assert(oTreeNodeChild != NULL);
  oLinkList = LinkList_new();
  if(oLinkList == NULL)
    return 1;
  LinkList_setValue(oLinkList, oTreeNodeChild);
  LinkList_setNext(oLinkList,\
		   LinkList_getNext(oTreeNodeParent -> pllChildren));
  LinkList_setNext(oTreeNodeParent -> pllChildren, oLinkList);
  return 0;
}

/*---------------------------------------------------------------------*/
/*Check if oTreeNode is a leaf, meaning that it has no children. Return*/
/*1 if oTreeNode is a leaf, 0 if not                                   */
/*---------------------------------------------------------------------*/
int TreeNode_isLeaf(TreeNode_T oTreeNode){
  assert(oTreeNode != NULL);
  if(TreeNode_getChildren(oTreeNode) == NULL)
    return 1;
  return 0;
}

/*---------------------------------------------------------------------*/
/*Check if all of oTreeNode's children are leaves. Return 1 if this is */
/*true, 0 if it is not. Return 0 if oTreeNode has no children          */
/*---------------------------------------------------------------------*/
int TreeNode_areChildrenLeaves(TreeNode_T oTreeNode){
  LinkList_T oLinkListChild;
  assert(oTreeNode != NULL);
  if(TreeNode_isLeaf(oTreeNode))
    return 0;
  for(oLinkListChild = TreeNode_getChildren(oTreeNode);
      oLinkListChild != NULL;
      oLinkListChild = LinkList_getNext(oLinkListChild)){
    if(TreeNode_isLeaf\
       ((TreeNode_T)LinkList_getValue(oLinkListChild)) == 0)
      return 0;
  }
  return 1;
}

/*---------------------------------------------------------------------*/
/*Return a list of all of the twigs (TreeNodes with only leaf children)*/
/*in oTreeNode. Return NULL if memory is insufficient, or an empty list*/
/*if there are no twigs in oTreeNode                                   */
/*---------------------------------------------------------------------*/
LinkList_T TreeNode_twigList(TreeNode_T oTreeNode){
  LinkList_T oLinkListStorage, oLinkListNew, oLinkListChild;
  TreeNode_T oTreeNodeChild;
  assert(oTreeNode != NULL);
  //allocate storage
  oLinkListStorage = LinkList_new();
  if(oLinkListStorage == NULL)
    return NULL;
  //deal with this tree, is it a twig?
  if(TreeNode_areChildrenLeaves(oTreeNode) == 1){
    //it is, ok put it in storage
    LinkList_setValue(oLinkListStorage, oTreeNode);
  }
  //loop through the children
  for(oLinkListChild = TreeNode_getChildren(oTreeNode);
      oLinkListChild != NULL;
      oLinkListChild = LinkList_getNext(oLinkListChild)){
    oTreeNodeChild = (TreeNode_T)LinkList_getValue(oLinkListChild);
    //get the list of twigs from this child
    oLinkListNew = TreeNode_twigList(oTreeNodeChild);
    //if memory was insufficient
    if(oLinkListNew == NULL){
      LinkList_freeRecursive(oLinkListStorage);
      return NULL;
    }
    //if there were no twigs
    else if(LinkList_isEmpty(oLinkListNew) == 0){
      //free the empty list
      LinkList_free(oLinkListNew);
      oLinkListNew = NULL;
    }
    //if there were matches
    else{
      //if storage is still empty
      if(LinkList_isEmpty(oLinkListStorage) == 0){
	//free oLinkListStorage, replace with oLinkListNext
	LinkList_free(oLinkListStorage);
	oLinkListStorage = oLinkListNew;
      }
      //if there were twigs already in storage
      else{
	//add oLinkListNew to the end of oLinkListStorage
	LinkList_setNext(LinkList_getLast(oLinkListStorage),\
			 oLinkListNew);
      }
    }
  }
  return oLinkListStorage;
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the first twig in oTreeNode, or NULL if none exis*/
/*---------------------------------------------------------------------*/
TreeNode_T TreeNode_getTwig(TreeNode_T oTreeNode){
  LinkList_T oLinkListChild;
  TreeNode_T oTreeNodeChild, oTreeNodeTwig;
  assert(oTreeNode != NULL);
  //check if I am a twig
  if(TreeNode_areChildrenLeaves(oTreeNode) == 1)
    return oTreeNode;
  //check my children
  for(oLinkListChild = TreeNode_getChildren(oTreeNode);
      oLinkListChild != NULL;
      oLinkListChild = LinkList_getNext(oLinkListChild)){
    oTreeNodeChild = (TreeNode_T)LinkList_getValue(oLinkListChild);
    oTreeNodeTwig = TreeNode_getTwig(oTreeNodeChild);
    if(oTreeNodeTwig != NULL)
      return oTreeNodeTwig;
  }
  return NULL;
}
/*---------------------------------------------------------------------*/
/*test treenode.c                                                      */
/*---------------------------------------------------------------------*/
/*
int main(void){
  LinkList_T oLinkListTwigs;
  TreeNode_T oTreeNode1, oTreeNode2;
  int i1, i2;
  int* pi1;
  int* pi2;
  printf("-----------------------------------\n");
  printf("Testing treenode.c\n");
  printf("-----------------------------------\n");
  oTreeNode1 = TreeNode_new();
  oTreeNode2 = TreeNode_new();
  if(oTreeNode1 == NULL | oTreeNode2 == NULL)
    printf("Failed Test 1\n");
  i1 = 1;
  i2 = 2;
  pi1 = &i1;
  pi2 = &i2;
  TreeNode_setValue(oTreeNode1, pi1);
  TreeNode_setValue(oTreeNode2, pi2);
  if((int*)TreeNode_getValue(oTreeNode1) != pi1)
    printf("Failed Test 2\n");
  if((int*)TreeNode_getValue(oTreeNode2) != pi2)
    printf("Failed Test 3\n");
  if(TreeNode_getChildren(oTreeNode1) != NULL)
    printf("Failed Test 4\n");
  if(TreeNode_addChild(oTreeNode1, oTreeNode2) != 0)
    printf("Failed Test 5\n");
  if((TreeNode_T)LinkList_getValue(TreeNode_getChildren(oTreeNode1)) != oTreeNode2)
    printf("Failed Test 6\n");
  if(TreeNode_isLeaf(oTreeNode2) == 0)
    printf("Failed Test 7\n");
  if(TreeNode_areChildrenLeaves(oTreeNode1) == 0)
    printf("Failed Test 8\n");
  oLinkListTwigs = TreeNode_twigList(oTreeNode1);
  if(LinkList_isEmpty(oLinkListTwigs) == 0)
    printf("Failed Test 9\n");
  if(LinkList_getLength(oLinkListTwigs) != 1u)
    printf("Failed Test 10\n");
  if((TreeNode_T)LinkList_getValue(oLinkListTwigs) != oTreeNode1)
    printf("Failed Test 11\n");
  LinkList_freeRecursive(oLinkListTwigs);
  if(TreeNode_getTwig(oTreeNode1) != oTreeNode1)
    printf("Failed Test 12\n");
  if(TreeNode_getTwig(oTreeNode2) != NULL)
    printf("Failed Test 13\n");
  LinkList_free(TreeNode_getChildren(oTreeNode1));
  TreeNode_free(oTreeNode1);
  TreeNode_free(oTreeNode2);
  printf("-----------------------------------\n");
  return 0;
}
*/
