/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*exptokentree.h describes functions for managing TreeNodes holding    */
/*ExpTokens.                                                           */
/*---------------------------------------------------------------------*/

#ifndef EXPTOKENTREE_INCLUDED
#define EXPTOKENTREE_INCLUDED

#include "treenode.h"
#include "linklist.h"
#include "exptoken.h"

/*---------------------------------------------------------------------*/
/*Return a new empty TreeNode. If memory is insufficient return NULL   */
/*---------------------------------------------------------------------*/
TreeNode_T ExpTokenTree_new(void);

/*---------------------------------------------------------------------*/
/*Recursively free oTreeNode, and all of the ExpTokens in it.          */
/*---------------------------------------------------------------------*/
void ExpTokenTree_free(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Return a pointer to the value of oTreeNode                           */
/*---------------------------------------------------------------------*/
ExpToken_T ExpTokenTree_getValue(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Set the value of oTreeNode to oExpToken. If there was already a value*/
/*in oTreeNode free that ExpToken, and print a warning to stderr       */
/*---------------------------------------------------------------------*/
void ExpTokenTree_setValue(TreeNode_T oTreeNode, ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Return a list of oTreeNode's children                                */
/*---------------------------------------------------------------------*/
LinkList_T ExpTokenTree_getChildren(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Make oTreeNodeChild a child of oTreeNodeParent. If memory is         */
/*insufficient return 1, otherwise return 0. Children are added to the */
/*end of the list                                                      */
/*---------------------------------------------------------------------*/
int ExpTokenTree_addChild(TreeNode_T oTreeNodeParent,\
			  TreeNode_T oTreeNodeChild);

/*---------------------------------------------------------------------*/
/*Make oTreeNodeChild a child of oTreeNodeParent. If memory is         */
/*insufficient return 1, otherwise return 0. Children are added to the */
/*front of the list                                                    */
/*---------------------------------------------------------------------*/
int ExpTokenTree_addChildFront(TreeNode_T oTreeNodeParent,\
			       TreeNode_T oTreeNodeChild);

/*---------------------------------------------------------------------*/
/*Compare if oTreeNode1 and oTreeNode2 are equivalent in terms of their*/
/*topology and contents. Return 0 if they are equal, 1 if they are not */
/*---------------------------------------------------------------------*/
int ExpTokenTree_compare(TreeNode_T oTreeNode1, TreeNode_T oTreeNode2);

/*---------------------------------------------------------------------*/
/*Look for subtrees within oTreeNodeSearch that match oTreeNodeMatch.  */
/*Return a LinkList containing TreeNode_T objects pointing to the      */
/*matching trees. Return NULL if memory is insufficient, and an empty  */
/*LinkList if no subtrees matched oTreeNodeMatch.                      */
/*---------------------------------------------------------------------*/
LinkList_T ExpTokenTree_subTree(TreeNode_T oTreeNodeSearch,\
				TreeNode_T oTreeNodeMatch);

/*---------------------------------------------------------------------*/
/*Return a new copy of oTreeNode, or NULL if memory is insufficient    */
/*---------------------------------------------------------------------*/
TreeNode_T ExpTokenTree_copy(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Print an infix representation of oTreeNode to STDOUT                 */
/*---------------------------------------------------------------------*/
void ExpTokenTree_print(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Create a copy of oTreeNodeSource where all instances of oTreeNodeFind*/
/*have been replaced by oTreeNodeReplace. Return NULL if memory is     */
/*insufficient for the copy. The resulting tree will be identical to   */
/*oTreeNodeSource if there were no instances of oTreeNodeFind.         */
/*---------------------------------------------------------------------*/
TreeNode_T  ExpTokenTree_replace(TreeNode_T oTreeNodeSource,\
			 TreeNode_T oTreeNodeFind,\
			 TreeNode_T oTreeNodeReplace);

/*---------------------------------------------------------------------*/
/*checks whether oTreeNode is a terminal branch. A terminal branch is  */
/*a tree that has children, all of whom are leaves. If oTreeNode is    */
/*a terminal branch, return 0, else return 1                           */
/*---------------------------------------------------------------------*/
int ExpTokenTree_isTerminalBranch(TreeNode_T oTreeNode);

/*---------------------------------------------------------------------*/
/*Compares oTreeNode1 and oTreeNode2 including a consideration for     */
/*associativity. Return 0 if they are equivalent, 1 if not             */
/*---------------------------------------------------------------------*/
int ExpTokenTree_compareAssociative(TreeNode_T oTreeNode1,\
				    TreeNode_T oTreeNode2);

#endif
