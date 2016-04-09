/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*linkstack.h outlines a generic stack structure using linklist        */
/*---------------------------------------------------------------------*/

#ifndef LINKSTACK_INCLUDED
#define LINKSTACK_INCLUDED

#include "linklist.h"

/*---------------------------------------------------------------------*/
/*A linkstack is a LIFO strcuture                                      */
/*---------------------------------------------------------------------*/
typedef struct LinkStack* LinkStack_T;

/*---------------------------------------------------------------------*/
/*Return a new empty LinkStack object, or NULL if memory is            */
/*insufficient                                                         */
/*---------------------------------------------------------------------*/
LinkStack_T LinkStack_new(void);

/*---------------------------------------------------------------------*/
/*Free all memory associated with this LinkStack. Note this only frees */
/*the stack and it's pointers. To free individual elements of the stack*/
/*is the user's responsibility                                         */
/*---------------------------------------------------------------------*/
void LinkStack_free(LinkStack_T oLinkStack);

/*---------------------------------------------------------------------*/
/*Push a new element onto the stack. Return 0 if successful, 1 if      */
/*memory is insufficient.                                              */
/*---------------------------------------------------------------------*/
int LinkStack_push(LinkStack_T oLinkStack, void* pvValue);

/*---------------------------------------------------------------------*/
/*Pop the top element off the stack. This frees the memory involved in */
/*storing the element, but not the memory associated with the item     */
/*itself, which must be freed by the user. NULL is returned if the     */
/*stack is empty                                                       */
/*---------------------------------------------------------------------*/
void* LinkStack_pop(LinkStack_T oLinkStack);

/*---------------------------------------------------------------------*/
/*Return the number of items on the stack                              */
/*---------------------------------------------------------------------*/
unsigned int LinkStack_howMany(LinkStack_T oLinkStack);

/*---------------------------------------------------------------------*/
/*Return a pointer to the value of the top element of the stack without*/
/*popping it. NOTE: use with extreme caution, popping is prefferable   */
/*---------------------------------------------------------------------*/
void* LinkStack_getValue(LinkStack_T oLinkStack);

/*---------------------------------------------------------------------*/
/*Return a pointer to the list that functions as the stack. NOTE: This */
/*should only be used by client functions that will not modify the     */
/*elements in the stack, like print functions.                         */
/*---------------------------------------------------------------------*/
LinkList_T LinkStack_getList(LinkStack_T oLinkStack);

#endif
