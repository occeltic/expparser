/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*exptokenstack.h describes a stack for storing ExpTokens              */
/*---------------------------------------------------------------------*/

#ifndef EXPTOKENSTACK_INCLUDED
#define EXPTOKENSTACK_INCLUDED

#include "exptoken.h"

/*---------------------------------------------------------------------*/
/*an ExpTokenStack is a LIFO structure for storing ExpTokens           */
/*---------------------------------------------------------------------*/
typedef struct ExpTokenStack* ExpTokenStack_T;

/*---------------------------------------------------------------------*/
/*Return a new empty ExpTokenStack, or NULL if memory is insufficient  */
/*---------------------------------------------------------------------*/
ExpTokenStack_T ExpTokenStack_new(void);

/*---------------------------------------------------------------------*/
/*Free all the memory associated with oExpTokenStack. This includes the*/
/*memory associated with all of the ExpTokens in the stack. Print a    */
/*warning to stderr if there were still items left in the stack when it*/
/*was freed                                                            */
/*---------------------------------------------------------------------*/
void ExpTokenStack_free(ExpTokenStack_T oExpTokenStack);

/*---------------------------------------------------------------------*/
/*Push a new ExpToken onto the stack. Return 0 if successful, 1 if     */
/*memory is insufficient                                               */
/*---------------------------------------------------------------------*/
int ExpTokenStack_push(ExpTokenStack_T oExpTokenStack,\
		       ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Pop the top ExpToken off the stack. This frees the memory involved in*/
/*storing the ExpToken, but not the memory of the ExpToken itself,     */
/*which the user must free. NULL is returned if the stack is empty.    */
/*---------------------------------------------------------------------*/
ExpToken_T ExpTokenStack_pop(ExpTokenStack_T oExpTokenStack);

/*---------------------------------------------------------------------*/
/*Return the number of items on the stack                              */
/*---------------------------------------------------------------------*/
unsigned int ExpTokenStack_howMany(ExpTokenStack_T oExpTokenStack);

/*---------------------------------------------------------------------*/
/*Compare the precedence of oExpToken with the ExpToken on top of      */
/*oExpTokenStack                                                       */
/*---------------------------------------------------------------------*/
int ExpTokenStack_precedence(ExpTokenStack_T oExpTokenStack,\
			     ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Return the type of the ExpToken on top of the stack                  */
/*---------------------------------------------------------------------*/
enum ExpTokenType ExpTokenStack_getType(ExpTokenStack_T oExpTokenStack);

/*---------------------------------------------------------------------*/
/*Print a representation of oExpTokenStack to StdOut                   */
/*---------------------------------------------------------------------*/
void ExpTokenStack_print(ExpTokenStack_T oExpTokenStack);

#endif
