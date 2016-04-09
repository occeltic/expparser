/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*linklist.h defines the basic functions for a generic linked list     */
/*---------------------------------------------------------------------*/

#ifndef LINKLIST_INCLUDED
#define LINKLIST_INCLUDED

/*---------------------------------------------------------------------*/
/*A linklist is a structure for storing ordered elements               */
/*---------------------------------------------------------------------*/
typedef struct LinkList* LinkList_T;

/*---------------------------------------------------------------------*/
/*Return a new empty LinkList object, or NULL if memory is insufficient*/
/*---------------------------------------------------------------------*/
LinkList_T LinkList_new(void);

/*---------------------------------------------------------------------*/
/*Free all the memory associated with this LinkList element. NOTE: This*/
/*free should not be recursive and free all memory in the list, just   */
/*the memory of this element. As it is not known whether the elements  */
/*of oLinkList have any memory that needs to be freed independently the*/
/*user should free all memory associated with the value of this element*/
/*before freeing the link                                              */
/*---------------------------------------------------------------------*/
void LinkList_free(LinkList_T oLinkList);

/*---------------------------------------------------------------------*/
/*Return a pointer to the value of this element                        */
/*---------------------------------------------------------------------*/
void* LinkList_getValue(LinkList_T oLinkList);

/*---------------------------------------------------------------------*/
/*Set the value of this element                                        */
/*---------------------------------------------------------------------*/
void LinkList_setValue(LinkList_T oLinkList, void* pvValue);

/*---------------------------------------------------------------------*/
/*Return a pointer to the next element in the list. If this is the last*/
/*element in the list it will return NULL                              */
/*---------------------------------------------------------------------*/
LinkList_T LinkList_getNext(LinkList_T oLinkList);

/*---------------------------------------------------------------------*/
/*Set the next element of oLinkListThis to oLinkListNext. Provides no  */
/*protection against losing data previously pointed to in oLinkListThis*/
/*---------------------------------------------------------------------*/
void LinkList_setNext(LinkList_T oLinkListThis,\
                      LinkList_T oLinkListNext);

/*---------------------------------------------------------------------*/
/*Return a pointer to the last element in the list                     */
/*---------------------------------------------------------------------*/
LinkList_T LinkList_getLast(LinkList_T oLinkList);

/*---------------------------------------------------------------------*/
/*Check whether a list is empty, as in it has not value and no next    */
/*Return 0 if true, 1 if false                                         */
/*---------------------------------------------------------------------*/
int LinkList_isEmpty(LinkList_T oLinkList);

/*---------------------------------------------------------------------*/
/*Return the number of linkages in the list                            */
/*---------------------------------------------------------------------*/
unsigned int LinkList_getLength(LinkList_T oLinkList);

/*---------------------------------------------------------------------*/
/*Recursively free linkages in oLinkList. Does not free the values     */
/*---------------------------------------------------------------------*/
void LinkList_freeRecursive(LinkList_T oLinkList);





#endif
