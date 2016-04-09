/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*exptoken is used to store information for representing and           */
/*evaluating mathematical expressions consisting of a series of        */
/*operators and operands. Acceptable operators are described here, as  */
/*well as the maximum number of characters that can be used in an      */
/*operand's name                                                       */
/*---------------------------------------------------------------------*/

#ifndef EXPTOKEN_INCLUDED
#define EXPTOKEN_INCLUDED


/*---------------------------------------------------------------------*/
/*define types to describe the various token types:all of the valid    */
/*operators as well as an operand type.                                */
/*                                                                     */
/*Valid mathematical operators currently supported are:                */
/*positive (ex. +x): unary, does not change the sign of the operand    */
/*negative (ex. -x): unary, change the sign of the operand             */
/*add (ex. x+y): binary, add the two operands together                 */
/*subtract (ex. x-y): binary, subtract the second from the first       */
/*multiply (ex. x*y): binary, multiply the two operands                */
/*divide (ex. x/y): binary, divide the first operand by the second     */
/*power (ex. x^y): binary, exponentiate the first operand by the second*/
/*                                                                     */
/*Other token types are:                                               */
/*left_bracket (ex. ( or ] ): start of a nested statement              */
/*right_bracket (ex. ) or ]): end of a nested statement                */
/*operand (ex. x): a variable or number                                */
/*---------------------------------------------------------------------*/
enum ExpTokenType {OPERAND, POSITIVE, NEGATIVE, ADD, SUBTRACT, MULTIPLY,\
                   DIVIDE, POWER, LEFT_BRACKET, RIGHT_BRACKET};

/*---------------------------------------------------------------------*/
/*An ExpToken is a distinct element in a mathematical expression       */
/*---------------------------------------------------------------------*/
typedef struct ExpToken* ExpToken_T;

/*---------------------------------------------------------------------*/
/*Return a new ExpToken object with no name or type,  or NULL if memory*/
/*is insufficient                                                      */
/*---------------------------------------------------------------------*/
ExpToken_T ExpToken_new(void);

/*---------------------------------------------------------------------*/
/*Free all memory occupied by oExpToken                                */
/*---------------------------------------------------------------------*/
void ExpToken_free(ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Return oExpToken's type. This may be NULL if the type has not been   */
/*set since creation                                                   */
/*---------------------------------------------------------------------*/
enum ExpTokenType ExpToken_getType(ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Return a pointer to oExpToken's name. If oExpToken is then freed     */
/*this may create a dangling pointer. Return NULL if oExpToken's name  */
/*is empty. The user must take care not to corrupt oExpToken's name    */
/*---------------------------------------------------------------------*/
char* ExpToken_getName(ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Set the ExpTokenType of oExpToken                                    */
/*---------------------------------------------------------------------*/
void ExpToken_setType(ExpToken_T oExpToken, enum ExpTokenType ettType);

/*---------------------------------------------------------------------*/
/*Append char c to the end of oExpToken's name. If no room remains do  */
/*not alter oExpToken's name, but print a warning to stdErr. Return 0  */
/*if successful, 1 if not                                              */
/*---------------------------------------------------------------------*/
int ExpToken_appendChar(ExpToken_T oExpToken, char c);

/*---------------------------------------------------------------------*/
/*Compare two ExpTokens. Return 0 if they are equivalent, 1 if not.    */
/*---------------------------------------------------------------------*/
int ExpToken_compare(ExpToken_T oExpToken1, ExpToken_T oExpToken2);

/*---------------------------------------------------------------------*/
/*Compare the precedence of two ExpTokens. Return 0 if they have equal */
/*precedence, -1 if oExpToken1 has lower precedence than oExpToken2,   */
/*and 1 if oExpToken1 has higher precedence than oExpToken2. Return -2 */
/*if the precedence of the two operators cannot be compared            */
/*---------------------------------------------------------------------*/
int ExpToken_precedence(ExpToken_T oExpToken1, ExpToken_T oExpToken2);

/*---------------------------------------------------------------------*/
/*Return a copy of oExpToken, or NULL if memory is insufficient        */
/*---------------------------------------------------------------------*/
ExpToken_T ExpToken_copy(ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Return the number of operands that oExpToken applies to. For Operand */
/*ExpTokenTypes this will be 0.                                        */
/*---------------------------------------------------------------------*/
int ExpToken_numOperands(ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Return a string that represents oExpToken. This will be the operator */
/*symbol for operators, and the operand name for operands.             */
/*---------------------------------------------------------------------*/
char* ExpToken_toString(ExpToken_T oExpToken);

/*---------------------------------------------------------------------*/
/*Returns 1 if the ExpToken exhibits associativity, that is the order  */
/*of its operands does not matter. Return 0 if not                     */
/*---------------------------------------------------------------------*/
int ExpToken_isAssociative(ExpToken_T oExpToken);

#endif
