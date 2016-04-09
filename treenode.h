/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*treenode.h defines the basic functions for a generic tree structure  */
/*with variable numbers of branches from each node                     */
/*---------------------------------------------------------------------*/

#ifndef TREENODE_INCLUDED
#define TREENODE_INCLUDED

#include "linklist.h"

/*---------------------------------------------------------------------*/
/*A TreeNode is a structure for storing hierarchically ordered elements*/
/*---------------------------------------------------------------------*/
typedef struct TreeNode* TreeNode_T;

/*---------------------------------------------------------------------*/
/*Return a new empty TreeNode object, or NULL if memory is insufficient*/
/*---------------------------------------------------------------------*/
TreeNode_T TreeNode_new(void);

/*---------------------------------------------------------------------*/
/*Free all the memory associated with oTreeNode. NOTE: This free is not*/
/*recursive, nor does it touch the value of this node. Managing this   */
/*memory and making a recursive free is up to the user.                */
/*---------------------------------------------------------------------*/
void TreeNode_free(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Return a pointer to the value of oTreeNode                           */
/*---------------------------------------------------------------------*/
void* TreeNode_getValue(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Set the value of oTreeNode to pvValue                                */
/*---------------------------------------------------------------------*/
void TreeNode_setValue(TreeNode_T oTreeNode, void* pvValue);

/*---------------------------------------------------------------------*/
/*Return a list of oTreeNode's children                                */
/*---------------------------------------------------------------------*/
LinkList_T TreeNode_getChildren(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Make oTreeNodeChild a child of oTreeNodeParent. If memory is         */
/*insufficient return 1, otherwise return 0. Children are added to the */
/*end of the list                                                      */
/*---------------------------------------------------------------------*/
int TreeNode_addChild(TreeNode_T oTreeNodeParent,\
                      TreeNode_T oTreeNodeChild);

/*---------------------------------------------------------------------*/
/*Make oTreeNodeChild a child of oTreeNodeParent. If memory is         */
/*insufficient return 1, otherwise return 0. Children are added to the */
/*front of the list                                                    */
/*---------------------------------------------------------------------*/
int TreeNode_addChildFront(TreeNode_T oTreeNodeParent,\
			   TreeNode_T oTreeNodeChild);

/*---------------------------------------------------------------------*/
/*Check if oTreeNode is a leaf, meaning that it has no children. Return*/
/*1 if oTreeNode is a leaf, 0 if not                                   */
/*---------------------------------------------------------------------*/
int TreeNode_isLeaf(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Check if all of oTreeNode's children are leaves. Return 1 if this is */
/*true 0 if it is not. Return 0 if oTreeNode has no children           */
/*---------------------------------------------------------------------*/
int TreeNode_areChildrenLeaves(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Return a list of all of the twigs (TreeNodes with only leaf children)*/
/*in oTreeNode. Return NULL if memory is insufficient, or an empty list*/
/*if there are no twigs in oTreeNode                                   */
/*---------------------------------------------------------------------*/
LinkList_T TreeNode_twigList(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Return a pointer to the first twig in oTreeNode, or NULL if none exis*/
/*---------------------------------------------------------------------*/
TreeNode_T TreeNode_getTwig(TreeNode_T oTreeNode);

#endif
