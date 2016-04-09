/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*expparser.h describes a struct for parsing infix representations of  */
/*mathematical expressions into expression trees.                      */
/*---------------------------------------------------------------------*/

#ifndef EXPPARSER_INCLUDED
#define EXPPARSER_INCLUDED


/*---------------------------------------------------------------------*/
/*an ExpParser is a structure used for parsing and manipulating infix  */
/*mathematical expressions                                             */
/*---------------------------------------------------------------------*/
typedef struct ExpParser* ExpParser_T;

/*---------------------------------------------------------------------*/
/*Return a new, empty ExpParser object, or NULL if memory is           */
/*insufficient                                                         */
/*---------------------------------------------------------------------*/
ExpParser_T ExpParser_new(void);

/*---------------------------------------------------------------------*/
/*Free all memory occupied by oExpParser                               */
/*---------------------------------------------------------------------*/
void ExpParser_free(ExpParser_T oExpParser);

/*---------------------------------------------------------------------*/
/*Update oExpParser based on c. If ExpParser_update is iteratively     */
/*applied to the elements o a string representing an infix expression, */
/*oExpParser will contain an expression tree representation of the     */
/*expression at the end of the iteration. Return 0 if successful, 1 if */
/*unsuccessful due memory limitations, and -1 if unsuccessful because  */
/*based on the current state of oExpParser, updating with c indicates  */
/*a malformatted expression                                            */
/*---------------------------------------------------------------------*/
//int ExpParser_update(ExpParser_T oExpParser, char c);

/*---------------------------------------------------------------------*/
/*Simplify the expressions stored in oExpParser into a series of small */
/*substitutions that is close to the minimum number of operations      */
/*needed to represent those expressions. Print this series to stdout.  */
/*If oExpParser doesn't contain properly formatted expressions then    */
/*print a warning to stderr instead                                    */
/*---------------------------------------------------------------------*/
//void ExpParser_simplify(ExpParser_T oExpParser);


#endif
