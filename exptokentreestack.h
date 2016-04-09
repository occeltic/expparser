/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*exptokentreestack.h describes a struct for storing exptokentrees in  */
/*a LIFO stack.                                                        */
/*---------------------------------------------------------------------*/

#ifndef EXPTOKENTREESTACK_INCLUDED
#define EXPTOKENTREESTACK_INCLUDED

#include "exptokentree.h"

/*---------------------------------------------------------------------*/
/*an ExpTokenTreeStack is a LIFO structure for storing ExpTokenTrees   */
/*---------------------------------------------------------------------*/
typedef struct ExpTokenTreeStack* ExpTokenTreeStack_T;

/*---------------------------------------------------------------------*/
/*Return a new empty ExpTokenTreeStack, or NULL if memory is           */
/*insufficient                                                         */
/*---------------------------------------------------------------------*/
ExpTokenTreeStack_T ExpTokenTreeStack_new(void);

/*---------------------------------------------------------------------*/
/*Free all the memory associated with oExpTokenTreeStack. This includes*/
/*the memory associated with all of the ExpTokenTrees in the stack.    */
/*Print a warning to stderr if there were still items in the stack when*/
/*it was freed                                                         */
/*---------------------------------------------------------------------*/
void ExpTokenTreeStack_free(ExpTokenTreeStack_T oExpTokenTreeStack);

/*---------------------------------------------------------------------*/
/*Push a new ExpTokenTree onto the stack. Return 0 if successful, 1 if */
/*memory is insufficient                                               */
/*---------------------------------------------------------------------*/
int ExpTokenTreeStack_push(ExpTokenTreeStack_T oExpTokenTreeStack,\
			   TreeNode_T oExpTokenTree);

/*---------------------------------------------------------------------*/
/*Pop the top ExpTokenTree off the stack. This frees the memory        */
/*involved in storing the ExpTokenTree, but not the memory of the      */
/*ExpTokenTree itself, which the user must free. NULL is returned if   */
/*stack is empty.                                                      */
/*---------------------------------------------------------------------*/
TreeNode_T ExpTokenTreeStack_pop\
(ExpTokenTreeStack_T oExpTokenTreeStack);

/*---------------------------------------------------------------------*/
/*Return the number of items currently on the stack                    */
/*---------------------------------------------------------------------*/
unsigned int ExpTokenTreeStack_howMany\
(ExpTokenTreeStack_T oExpTokenTreeStack);

/*---------------------------------------------------------------------*/
/*Print a representation of oExpTokenTreeStack to StdOut               */
/*---------------------------------------------------------------------*/
void ExpTokenTreeStack_print(ExpTokenTreeStack_T oExpTokenTreeStack);


#endif
