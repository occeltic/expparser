/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*expparser.c implements expparser.h                                   */
/*---------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expparser.h"
#include "linklist.h"
#include "exptokentreestack.h"
#include "exptokenstack.h"
#include "exptokentree.h"
#include "exptoken.h"


/*---------------------------------------------------------------------*/
/*Define a character to signify that a variable is a new variable      */
/*---------------------------------------------------------------------*/
#define VAR_CHAR '#'

/*---------------------------------------------------------------------*/
/*Define an enum type for states of the parsing DFA                    */
/*---------------------------------------------------------------------*/
enum DFAState {START, AFTER_OPERATOR, AFTER_OPERAND, ERROR, COMPLETE};

/*---------------------------------------------------------------------*/
/*ExpParsers have en exptoken for assembling multichar tokens, an      */
/*an exptokenstack for storing operators during parsing, an            */
/*exptokentreestack for storing operands during parsing, and a linklist*/
/*for storing completed exptokentrees should the input include multiple*/
/*expressions                                                          */
/*---------------------------------------------------------------------*/
struct ExpParser
{
  /*exptoken under assembly*/
  ExpToken_T petAssemble;
  /*operator stack*/
  ExpTokenStack_T petsOperatorStack;
  /*operand stack*/
  ExpTokenTreeStack_T pettsOperandStack;
  /*storage for complete expressions*/
  LinkList_T pllExpressions;
};

/*---------------------------------------------------------------------*/
/*Declare static functions used by functions that occur earlier than   */
/*the static functions                                                 */
/*---------------------------------------------------------------------*/
static int ExpParser_popOperator(ExpParser_T oExpParser);
static void ExpParser_print(ExpParser_T oExpParser);

/*---------------------------------------------------------------------*/
/*Return a new, empty ExpParser object, or NULL if memory is           */
/*insufficient                                                         */
/*---------------------------------------------------------------------*/
ExpParser_T ExpParser_new(void){
  ExpParser_T oExpParser = (ExpParser_T)malloc(sizeof(struct ExpParser));
  if(oExpParser == NULL)
    return NULL;
  oExpParser -> petAssemble = ExpToken_new();
  if(oExpParser -> petAssemble == NULL){
    free(oExpParser);
    return NULL;
  }
  oExpParser -> petsOperatorStack = ExpTokenStack_new();
  if(oExpParser -> petsOperatorStack == NULL){
    ExpToken_free(oExpParser -> petAssemble);
    oExpParser -> petAssemble = NULL;
    free(oExpParser);
    return NULL;
  }
  oExpParser -> pettsOperandStack = ExpTokenTreeStack_new();
  if(oExpParser -> pettsOperandStack == NULL){
    ExpTokenStack_free(oExpParser -> petsOperatorStack);
    oExpParser -> petsOperatorStack = NULL;
    ExpToken_free(oExpParser -> petAssemble);
    oExpParser -> petAssemble = NULL;
    free(oExpParser);
    return NULL;
  }
  //The LinkList isn't initialized until a value needs to be stored
  oExpParser -> pllExpressions = NULL;
  /*oExpParser -> pllExpressions = LinkList_new();
  if(oExpParser -> pllExpressions == NULL){
    ExpTokenTreeStack_free(oExpParser -> pettsOperandStack);
    oExpParser -> pettsOperandStack = NULL;
    ExpTokenStack_free(oExpParser -> petsOperatorStack);
    oExpParser -> petsOperatorStack = NULL;
    ExpToken_free(oExpParser -> petAssemble);
    oExpParser -> petAssemble = NULL;
    free(oExpParser);
    return NULL;
  }
  */
  return oExpParser;
}

/*---------------------------------------------------------------------*/
/*Free all memory occupied by oExpParser                               */
/*---------------------------------------------------------------------*/
void ExpParser_free(ExpParser_T oExpParser){
  LinkList_T oLinkList, oLinkListNext;
  oLinkListNext = NULL;
  assert(oExpParser != NULL);
  ExpToken_free(oExpParser -> petAssemble);
  oExpParser -> petAssemble = NULL;
  ExpTokenStack_free(oExpParser -> petsOperatorStack);
  oExpParser -> petsOperatorStack = NULL;
  ExpTokenTreeStack_free(oExpParser -> pettsOperandStack);
  oExpParser -> pettsOperandStack = NULL;
  //traverse the list freeing the links and values
  for(oLinkList = oExpParser -> pllExpressions;
      oLinkList != NULL;
      oLinkList = oLinkListNext){
    oLinkListNext = LinkList_getNext(oLinkList);
    //free the token tree
    ExpTokenTree_free((TreeNode_T)LinkList_getValue(oLinkList));
    //free the linkage
    LinkList_free(oLinkList);
  }
  oExpParser -> pllExpressions = NULL;
  free(oExpParser);
}

/*---------------------------------------------------------------------*/
/*Handle the case where petAssemble is being pushed onto the stack and */
/*it is an OPERAND type. Return 0 if successful, 1 if memory is        */
/*insufficient, and -1 if the expression is malformed                  */      
/*---------------------------------------------------------------------*/
static int ExpParser_handlePush_OPERAND(ExpParser_T oExpParser){
  ExpToken_T oExpTokenNew;
  TreeNode_T oTreeNodeNew;
  assert(oExpParser != NULL);
  assert(ExpToken_getType(oExpParser -> petAssemble) == OPERAND);
  //allocate storage for the new token
  oExpTokenNew = ExpToken_new();
  if(oExpTokenNew == NULL)
    return 1;
  //allocate storage for the new tree node
  oTreeNodeNew = ExpTokenTree_new();
  if(oTreeNodeNew == NULL){
    ExpToken_free(oExpTokenNew);
    return 1;
  }
  //store petAssemble in the TreeNode
  ExpTokenTree_setValue(oTreeNodeNew, oExpParser -> petAssemble);
  //ExpParser_print(oExpParser);
  //push it onto the operand stack
  if(ExpTokenTreeStack_push(oExpParser -> pettsOperandStack,\
			    oTreeNodeNew) == 1){
    TreeNode_free(oTreeNodeNew);//so as not to free petAssemble
    ExpToken_free(oExpTokenNew);
    return 1;
  }
  //ExpParser_print(oExpParser);
  //point petAssemble to the new token
  oExpParser -> petAssemble = oExpTokenNew;
  return 0;
}

/*---------------------------------------------------------------------*/
/*Handle the case where petAssemble is being pushed and is of type     */
/*LEFT_BRACKET. Return 0 if successful, 1 if memory was insufficient   */
/*and -1 if malformed                                                  */
/*---------------------------------------------------------------------*/
static int ExpParser_handlePush_LEFT_BRACKET(ExpParser_T oExpParser){
  ExpToken_T oExpTokenNew;
  assert(oExpParser != NULL);
  assert(ExpToken_getType(oExpParser -> petAssemble) == LEFT_BRACKET);
  //make storage for the next exptoken
  oExpTokenNew = ExpToken_new();
  if(oExpTokenNew == NULL)
    return 1;
  //push petAssemble onto the operator stack
  if(ExpTokenStack_push(oExpParser -> petsOperatorStack,\
			oExpParser -> petAssemble) == 1){
    ExpToken_free(oExpTokenNew);
    return 1;
  }
  //point petAssemble at the new token
  oExpParser -> petAssemble = oExpTokenNew;
  return 0;
}

/*---------------------------------------------------------------------*/
/*Handle the case where petAssemble is being pushed onto a stack, and  */
/*is type RIGHT_BRACKET. Return 0 if successful, 1 if memory was       */
/*insufficient, and -1 if malformed                                    */
/*---------------------------------------------------------------------*/
static int ExpParser_handlePush_RIGHT_BRACKET(ExpParser_T oExpParser){
  ExpToken_T oExpTokenNew;
  int iPopReturn;
  assert(oExpParser != NULL);
  assert(ExpToken_getType(oExpParser -> petAssemble) == RIGHT_BRACKET);
  //make space for the new token
  oExpTokenNew = ExpToken_new();
  if(oExpTokenNew == NULL)
    return 1;
  //if the operator stack is empty the expression is malformed
  //pop operators from the operator stack until you find a left bracket
  while(ExpTokenStack_howMany(oExpParser -> petsOperatorStack) > 0u){
    if(ExpTokenStack_getType(oExpParser -> petsOperatorStack) !=\
       LEFT_BRACKET){
      iPopReturn = ExpParser_popOperator(oExpParser);
      if(iPopReturn != 0){
	ExpToken_free(oExpTokenNew);
	return iPopReturn;
      }
    }
    else{//found a left bracket
      //free the left bracket
      ExpToken_free(ExpTokenStack_pop(oExpParser -> petsOperatorStack));
      //free the right bracket
      ExpToken_free(oExpParser -> petAssemble);
      //assign the new token
      oExpParser -> petAssemble = oExpTokenNew;
      return 0;
    }
  }
  //if control reaches here then the expression is malformed
  ExpToken_free(oExpTokenNew);
  fprintf								\
    (stderr,"%s",							\
     "ExpParser_handlePush_RIGHT_BRACKET: Missing Left Bracket\n");
  return -1;
}

/*---------------------------------------------------------------------*/
/*Handle pushing petAssemble onto the stack when it is of type POSITIVE*/
/*NEGATIVE, ADD, SUBTRACT, MULTIPLY, DIVIDE, or POWER. Return 0 if     */
/*successful, 1 if memory is insufficient, -1 if malformed             */
/*---------------------------------------------------------------------*/
static int ExpParser_handlePush_OPERATOR(ExpParser_T oExpParser){
  ExpToken_T oExpTokenNew;
  enum ExpTokenType ettType;
  int iPrecedence, iPopReturn;
  assert(oExpParser != NULL);
  ettType = ExpToken_getType(oExpParser -> petAssemble);
  assert(ettType == POSITIVE|			\
	 ettType == NEGATIVE|			\
	 ettType == ADD|			\
	 ettType == SUBTRACT|			\
	 ettType == MULTIPLY|			\
	 ettType == DIVIDE|			\
	 ettType == POWER);
  //allocate space for the new token
  oExpTokenNew = ExpToken_new();
  if(oExpTokenNew == NULL)
    return 1;
  //is the stack empty?
  while(ExpTokenStack_howMany(oExpParser -> petsOperatorStack) > 0u){
    //find the precedence of petAssemble compared to the top operator
    iPrecedence =						\
      ExpTokenStack_precedence(oExpParser -> petsOperatorStack,	\
			       oExpParser -> petAssemble);
    //if petAssemble is higher precedence just push it
    if(iPrecedence == 1|
       ExpTokenStack_getType(oExpParser -> petsOperatorStack)\
       == LEFT_BRACKET){
      if(ExpTokenStack_push(oExpParser -> petsOperatorStack,\
			    oExpParser -> petAssemble) == 1){
	ExpToken_free(oExpTokenNew);
	return 1;
      }
      //assign petAssemble
      oExpParser -> petAssemble = oExpTokenNew;
      return 0;
    }
    //if lower precedence then pop the operator on top
    else if(iPrecedence == 0 | iPrecedence == -1){
      iPopReturn = ExpParser_popOperator(oExpParser);
      if(iPopReturn != 0){
	ExpToken_free(oExpTokenNew);
	return iPopReturn;
      }
    }
    //if precedence is undefined something has gone wrong
    else{
      ExpToken_free(oExpTokenNew);
      fprintf\
	(stderr,\
	 "%s", "ExpParser_handlePush_OPERATOR: precedence undefined\n");
      return -1;
    }
  }
  //if the stack is empty just push petAssemble
  if(ExpTokenStack_push(oExpParser -> petsOperatorStack,	\
			oExpParser -> petAssemble) == 1){
    ExpToken_free(oExpTokenNew);
    return 1;
  }
  //reassign petAssemble
  oExpParser -> petAssemble = oExpTokenNew;
  return 0;
}
    
/*---------------------------------------------------------------------*/
/*Push petAssemble onto the appropriate stack, and replace it with a   */
/*new ExpToken. Return 0 if successful, 1 if memory was insufficient,  */
/*-1 if the expression is malformed                                    */
/*---------------------------------------------------------------------*/
static int ExpParser_pushToken(ExpParser_T oExpParser){
  enum ExpTokenType ettType;
  assert(oExpParser != NULL);
  //handle the token differently depending on its type
  ettType = ExpToken_getType(oExpParser -> petAssemble);
  if(ettType == OPERAND)
    return ExpParser_handlePush_OPERAND(oExpParser);
  else if(ettType == LEFT_BRACKET)
    return ExpParser_handlePush_LEFT_BRACKET(oExpParser);
  else if(ettType == RIGHT_BRACKET)
    return ExpParser_handlePush_RIGHT_BRACKET(oExpParser);
  //handle all of the supported operations
  else if(ettType == POSITIVE|			\
	  ettType == NEGATIVE|			\
	  ettType == ADD|			\
	  ettType == SUBTRACT|			\
	  ettType == MULTIPLY|			\
	  ettType == DIVIDE|			\
	  ettType == POWER)
    return ExpParser_handlePush_OPERATOR(oExpParser);
  else{//undefined type
    assert(0);
    return -1;
  }
}

/*---------------------------------------------------------------------*/
/*push petAssemble onto the operator stack and replace it with a new   */
/*ExpToken. Return 0 if successful, 1 if memory was insufficient       */
/*---------------------------------------------------------------------*/
/*static int ExpParser_pushOperator(ExpParser_T oExpParser){
  ExpToken_T oExpTokenNew = ExpToken_new();
  if(oExpTokenNew == NULL)
    return 1;
  assert(oExpParser != NULL);
  if(ExpTokenStack_push(oExpParser -> petsOperatorStack,\
			oExpParser -> petAssemble) == 1){
    ExpToken_free(oExpTokenNew);
    return 1;
  }
  oExpParser -> petAssemble = oExpTokenNew;
  return 0;
} 
*/
/*---------------------------------------------------------------------*/
/*Handle operators popped from the operator stack. Create a new        */
/*ExpTokenTree to store them, and assign the appropriate number of     */
/*children to that node, then repush it onto the ExpTokenTreeStack.    */
/*Return 0 if successful, 1 if failure due to memory limitations, and  */
/*-1 if malformed. If the operator stack is empty return -1.           */
/*---------------------------------------------------------------------*/
static int ExpParser_popOperator(ExpParser_T oExpParser){
  TreeNode_T oExpTokenTreeNew, oExpTokenTreeChild;
  ExpToken_T oExpToken;
  int iIndex;
  assert(oExpParser != NULL);
  //ExpParser_print(oExpParser);
  oExpToken = ExpTokenStack_pop(oExpParser -> petsOperatorStack);
  //ExpParser_print(oExpParser);
  if(oExpToken == NULL){
    //print an informative warning
    fprintf(stderr, "%s","ExpParser_popOperator: no operator to pop\n");
    return -1;
  }
  //make a new ExpTokenTree for storage
  oExpTokenTreeNew = ExpTokenTree_new();
  //ExpTokenTree_print(oExpTokenTreeNew);
  if(oExpTokenTreeNew == NULL){
    //attempt to repush the oExpToken, if fails then free it
    if(ExpTokenStack_push(oExpParser -> petsOperatorStack, oExpToken)\
       == 1)
      ExpToken_free(oExpToken);
    return 1;
  }
  //set the value of the new tree to be the popped token
  ExpTokenTree_setValue(oExpTokenTreeNew, oExpToken);
  //ExpTokenTree_print(oExpTokenTreeNew);
  //pop the number of operands appropriate for this operator
  for(iIndex = ExpToken_numOperands(oExpToken);
      iIndex > 0;
      iIndex --){
    oExpTokenTreeChild =\
      ExpTokenTreeStack_pop(oExpParser -> pettsOperandStack);
    //ExpTokenTree_print(oExpTokenTreeChild);
    //ExpParser_print(oExpParser);
    if(oExpTokenTreeChild == NULL){
      fprintf(stderr, "ExpParser_popOperator: %i operands missing\n",\
	      iIndex);
      ExpTokenTree_free(oExpTokenTreeNew);
      return -1;
    }
    if(ExpTokenTree_addChildFront(oExpTokenTreeNew, oExpTokenTreeChild)\
       == 1){
      ExpTokenTree_free(oExpTokenTreeNew);
      ExpTokenTree_free(oExpTokenTreeChild);
      return 1;
    }
    //ExpTokenTree_print(oExpTokenTreeNew);
  }
  //repush the new treenode
  //ExpParser_print(oExpParser);
  //ExpTokenTree_print(oExpTokenTreeNew);
  if(ExpTokenTreeStack_push(oExpParser -> pettsOperandStack,\
			    oExpTokenTreeNew) == 1){
    ExpTokenTree_free(oExpTokenTreeNew);
    return 1;
  }
  return 0;
}

/*---------------------------------------------------------------------*/
/*Print a representation of oExpParser to StdOut                       */
/*---------------------------------------------------------------------*/
static void ExpParser_print(ExpParser_T oExpParser){
  LinkList_T oLinkListPrint;
  unsigned int uiIndex = 0u;
  assert(oExpParser != NULL);
  printf("---Assembly Token---\n");
  fflush(stdout);
  fprintf(stdout, "%s\n", ExpToken_toString(oExpParser -> petAssemble));
  printf("---Operator Stack---\n");
  fflush(stdout);
  ExpTokenStack_print(oExpParser -> petsOperatorStack);
  printf("---Operand Stack---\n");
  fflush(stdout);
  ExpTokenTreeStack_print(oExpParser -> pettsOperandStack);
  printf("---Expression List---\n");
  fflush(stdout);
  for(oLinkListPrint = oExpParser -> pllExpressions;
      oLinkListPrint != NULL;
      oLinkListPrint = LinkList_getNext(oLinkListPrint)){
    fprintf(stdout, "---Expression %u---\n", uiIndex);
    fflush(stdout);
    uiIndex++;
    ExpTokenTree_print((TreeNode_T)LinkList_getValue(oLinkListPrint));
    printf("\n");
  }
}

/*---------------------------------------------------------------------*/
/*Push the operand tree to the pllExpressions and reset the stacks.    */
/*Return 0 if successful, 1 if memory is insufficient, and -1 if       */
/*inappropriate. Reasons for -1 include having more than 1 item on the */
/*operand stack, or having any items remaining on the operator stack   */
/*---------------------------------------------------------------------*/
static int ExpParser_newTree(ExpParser_T oExpParser){
  LinkList_T oLinkListNew;
  ExpToken_T oExpTokenNew;
  assert(oExpParser != NULL);
  //allocate memory for the new link
  //ExpParser_print(oExpParser);
  oLinkListNew = LinkList_new();
  if(oLinkListNew == NULL)
    return 1;
  oExpTokenNew = ExpToken_new();
  if(oExpTokenNew == NULL){
    LinkList_free(oLinkListNew);
    return 1;
  }
  //check if this oExpParser is formatted correctly
  if(ExpTokenStack_howMany(oExpParser -> petsOperatorStack) != 0u){
    fprintf(stderr, "%s",						\
	    "ExpParser_newTree: tokens remain on operator stack\n");
    LinkList_free(oLinkListNew);
    ExpToken_free(oExpTokenNew);
    return -1;
  }
  if(ExpTokenTreeStack_howMany(oExpParser -> pettsOperandStack) == 0u){
    fprintf(stderr, "%s",						\
	    "ExpParser_newTree: no expression trees on stack\n");
    LinkList_free(oLinkListNew);
    ExpToken_free(oExpTokenNew);
    return -1;
  }
  if(ExpTokenTreeStack_howMany(oExpParser -> pettsOperandStack) > 1u){
    fprintf(stderr, "%s",						\
	    "ExpParser_newTree: unconsolidated expression trees\n");
    LinkList_free(oLinkListNew);
    ExpToken_free(oExpTokenNew);
    return -1;
  }
  //store the tree and add it to the list
  LinkList_setValue(oLinkListNew, ExpTokenTreeStack_pop\
		    (oExpParser -> pettsOperandStack));
  //if the list is empty then just swap
  if(oExpParser -> pllExpressions == NULL)
    oExpParser -> pllExpressions = oLinkListNew;
  else
    LinkList_setNext(LinkList_getLast(oExpParser -> pllExpressions),\
		     oLinkListNew);
  //reset petAssemble
  ExpToken_free(oExpParser -> petAssemble);
  oExpParser -> petAssemble = oExpTokenNew;
  return 0;
}
							
/*---------------------------------------------------------------------*/
/*Handle the parsing DFA when the state is START. Return the next DFA  */
/*state                                                                */
/*---------------------------------------------------------------------*/
static enum DFAState ExpParser_handleDFA_START(ExpParser_T oExpParser){
  int iChar, iStatus;
  assert(oExpParser != NULL);
  //get the next char from stdin
  iChar = getchar();
  if(iChar == ' ')
    return START;
  else if(iChar == '+'){
    //set the next token to be positive
    ExpToken_setType(oExpParser -> petAssemble, POSITIVE);
    return AFTER_OPERATOR;
  }
  else if(iChar == '-'){
    //set the next token to be negative
    ExpToken_setType(oExpParser -> petAssemble, NEGATIVE);
    return AFTER_OPERATOR;
  }
  else if(iChar == '*'){
    //this signifies a malformed expression
    fprintf(stderr,"%s","ExpParser: invalid operator sequence\n");
    return ERROR;
  }
  else if(iChar == '/'){
    //this signifies a malformed expression
    fprintf(stderr,"%s","ExpParser: invalid operator sequence\n");
    return ERROR;
  }
  else if(iChar == '^'){
    //this signifies a malformed expression
    fprintf(stderr,"%s","ExpParser: invalid operator sequence\n");
    return ERROR;
  }
  else if(iChar == EOF | iChar == '\n'){
    //there is not input
    fprintf(stderr, "%s", "ExpParser: no input in new expression\n");
    return ERROR;
  }
  else if(iChar == '(' | iChar == '[' | iChar == '{'){
    //set petAssemble to be LEFT_BRACKET
    ExpToken_setType(oExpParser -> petAssemble, LEFT_BRACKET);
    return AFTER_OPERATOR;
  }
  else if(iChar == ')' | iChar == ']' | iChar == '}'){
    //set petAssemble to be RIGHT_BRACKET
    ExpToken_setType(oExpParser -> petAssemble, RIGHT_BRACKET);
    return AFTER_OPERATOR;
  }
  else if(iChar == ','){
    //ignore and read the next character
    return START;
  }
  //if iChar is some other character assume it is a variable name
  else{
    //append iChar to petAssemble, and check exit status
    iStatus = ExpToken_appendChar(oExpParser -> petAssemble,	\
				  (char)iChar);
    if(iStatus == 0)
      return AFTER_OPERAND;
    else if(iStatus == 1)
      return ERROR;
    else{
      assert(0);
      return ERROR;
    }
  }
}
   

/*---------------------------------------------------------------------*/
/*Handle the parsing DFA when the state is AFTER_OPERATOR. Return the  */
/*next DFA state                                                       */
/*---------------------------------------------------------------------*/
static enum DFAState ExpParser_handleDFA_AFTER_OPERATOR	\
(ExpParser_T oExpParser){
  int iChar, iStatus;
  assert(oExpParser != NULL);
  //get the next character from stdin
  iChar = getchar();
  //behavior is different depending on what iChar is
  //ignore whitespace
  if(iChar == ' ')
    return AFTER_OPERATOR;
  else if(iChar == '+'){
    //push petAssemble but check its output
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be positive
      ExpToken_setType(oExpParser -> petAssemble, POSITIVE);
      return AFTER_OPERATOR;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == '-'){
    //push petAssemble but check its output
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be negative
      ExpToken_setType(oExpParser -> petAssemble, NEGATIVE);
      return AFTER_OPERATOR;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == '*'){
    //this signifies a malformed expression
    fprintf(stderr,"%s","ExpParser: invalid operator sequence\n");
    return ERROR;
  }
  else if(iChar == '/'){
    //this signifies a malformed expression
    fprintf(stderr,"%s","ExpParser: invalid operator sequence\n");
    return ERROR;
  }
  else if(iChar == '^'){
    //this signifies a malformed expression
    fprintf(stderr,"%s","ExpParser: invalid operator sequence\n");
    return ERROR;
  }
  else if(iChar == EOF | iChar == '\n'){
    //there is an operator that wasn't matched
    fprintf(stderr, "%s", "ExpParser: extra operator\n");
    return ERROR;
  }
  else if(iChar == '(' | iChar == '[' | iChar == '{'){
    //push petAssemble but check its return status
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be a left bracket
      ExpToken_setType(oExpParser -> petAssemble, LEFT_BRACKET);
      return AFTER_OPERATOR;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == ')' | iChar == ']' | iChar == '}'){
    //push petAssemble but check its return status
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be a right bracket
      ExpToken_setType(oExpParser -> petAssemble, RIGHT_BRACKET);
      return AFTER_OPERAND;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == ','){//signifies a new expression
    //push petAssemble and monitor its status
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //store the current tree and make reset the parser for the next one
      iStatus = ExpParser_newTree(oExpParser);
      if(iStatus == 0){
	return START;
      }
      else if(iStatus == 1){
	fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
	return ERROR;
      }
      else if(iStatus == -1)
	return ERROR;
      else{//should not happen
	assert(0);
	return ERROR;
      }
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should not happen
      assert(0);
      return ERROR;
    }
  }
  //if iChar is some other character assume it is a variable name
  else{
    //push petAssemble and check the exit status
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the type to be operand
      ExpToken_setType(oExpParser -> petAssemble, OPERAND);
      //append iChar to the new petAssemble, and check the status
      iStatus = ExpToken_appendChar(oExpParser -> petAssemble,	\
				    (char)iChar);
      if(iStatus == 0)
	return AFTER_OPERAND;
      else if(iStatus == 1)
	return ERROR;
      else{//should not occur
	assert(0);
	return ERROR;
      }
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should not occur
      assert(0);
      return ERROR;
    }
  }
}
/*---------------------------------------------------------------------*/
/*Handle the parsing DFA when the state is AFTER_OPERAND. Return the   */
/*next DFA state                                                       */
/*---------------------------------------------------------------------*/
static enum DFAState ExpParser_handleDFA_AFTER_OPERAND	\
(ExpParser_T oExpParser){
  int iChar, iStatus;
  assert(oExpParser != NULL);
  //get the next character from stdin
  iChar = getchar();
  //behavior is different depending on what iChar is
  //ignore whitespace
  if(iChar == ' ')
    return AFTER_OPERAND;
  else if(iChar == '+'){
    //push petAssemble but check its output
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be add
      ExpToken_setType(oExpParser -> petAssemble, ADD);
      return AFTER_OPERATOR;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == '-'){
    //push petAssemble but check its output
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be subtract
      ExpToken_setType(oExpParser -> petAssemble, SUBTRACT);
      return AFTER_OPERATOR;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == '*'){
    //push petAssemble but check its output
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be multiply
      ExpToken_setType(oExpParser -> petAssemble, MULTIPLY);
      return AFTER_OPERATOR;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == '/'){
     //push petAssemble but check its output
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be divide
      ExpToken_setType(oExpParser -> petAssemble, DIVIDE);
      return AFTER_OPERATOR;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == '^'){
    //push petAssemble but check its output
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be power
      ExpToken_setType(oExpParser -> petAssemble, POWER);
      return AFTER_OPERATOR;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == EOF| iChar == '\n'){
    //push petAssemble and check the exit status
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //put the expression tree on the list
      iStatus = ExpParser_newTree(oExpParser);
      if(iStatus == 0)
	return COMPLETE;
      else if(iStatus == 1){
	fprintf(stderr, "%s", "%ExpParser: insufficient memory\n");
	return ERROR;
      }
      else if(iStatus == -1)
	return ERROR;
      else{
	assert(0);
	return ERROR;
      }
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never happen
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == '(' | iChar == '[' | iChar == '{'){
    //signifies that there is a missing operator
    fprintf(stderr, "%s", "ExpParser: missing operator after bracket\n");
    return ERROR;
  }
  else if(iChar == ')' | iChar == ']' | iChar == '}'){
    //push petAssemble but check its return status
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //set the next token to be a right bracket
      ExpToken_setType(oExpParser -> petAssemble, RIGHT_BRACKET);
      return AFTER_OPERAND;
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
  else if(iChar == ','){//signifies a new expression
    //push petAssemble and monitor its status
    iStatus = ExpParser_pushToken(oExpParser);
    if(iStatus == 0){
      //store the current tree and make reset the parser for the next one
      iStatus = ExpParser_newTree(oExpParser);
      if(iStatus == 0){
	return START;
      }
      else if(iStatus == 1){
	fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
	return ERROR;
      }
      else if(iStatus == -1)
	return ERROR;
      else{//should not happen
	assert(0);
	return ERROR;
      }
    }
    else if(iStatus == 1){
      fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
      return ERROR;
    }
    else if(iStatus == -1)
      return ERROR;
    else{//should not happen
      assert(0);
      return ERROR;
    }
  }
  //if iChar is some other character assume it is a variable name
  else{
    //append iChar to petAssemble
    iStatus = ExpToken_appendChar(oExpParser -> petAssemble,	\
				  (char)iChar);
    if(iStatus == 0)
      return AFTER_OPERAND;
    else if(iStatus == 1)
      return ERROR;
    else{//should never occur
      assert(0);
      return ERROR;
    }
  }
}

/*---------------------------------------------------------------------*/
/*Return the number of instances of subtree oTreeNodeSearch in the     */
/*trees stored in pllExpressions. Return -1 if memory was insufficient */
/*---------------------------------------------------------------------*/
static int ExpParser_howManySubTrees(ExpParser_T oExpParser,\
				     TreeNode_T oTreeNodeSearch){
  LinkList_T oLinkListMatches, oLinkListTree;
  int iNumMatches = 0;
  assert(oExpParser != NULL);
  assert(oTreeNodeSearch != NULL);
  //loop through every stored tree
  for(oLinkListTree = oExpParser -> pllExpressions;
      oLinkListTree != NULL;
      oLinkListTree = LinkList_getNext(oLinkListTree)){
    //get a list of the subtrees matching oTreeNodeSearch in this tree
    oLinkListMatches =\
      ExpTokenTree_subTree((TreeNode_T)				\
			   LinkList_getValue(oLinkListTree),	\
			   oTreeNodeSearch);
    //did memory run out
    if(oLinkListMatches == NULL)
      return -1;
    //were there no matches, if so do nothing
    else if(LinkList_isEmpty(oLinkListMatches) == 0);
    //if there were matches how many were there
    else{
      iNumMatches =\
	iNumMatches + (int)LinkList_getLength(oLinkListMatches);
    }
    //free the linkages that were returned
    LinkList_freeRecursive(oLinkListMatches);
  }
  return iNumMatches;
}

/*---------------------------------------------------------------------*/
/*Return a list of all of the twigs in the trees in the expression list*/
/*Return NULL if memory is insufficient, an empty list if there are no */
/*twigs                                                                */
/*---------------------------------------------------------------------*/
static LinkList_T ExpParser_expressionTwigs(ExpParser_T oExpParser){
  LinkList_T oLinkListStorage, oLinkListTree, oLinkListNew;
  assert(oExpParser != NULL);
  //allocate storage
  oLinkListStorage = LinkList_new();
  if(oLinkListStorage == NULL)
    return NULL;
  //get the twig list from each tree
  for(oLinkListTree = oExpParser -> pllExpressions;
      oLinkListTree != NULL;
      oLinkListTree = LinkList_getNext(oLinkListTree)){
    oLinkListNew =\
      TreeNode_twigList((TreeNode_T)LinkList_getValue(oLinkListTree));
    //if memory was insufficient
    if(oLinkListNew == NULL){
      LinkList_freeRecursive(oLinkListStorage);
      return NULL;
    }
    //if there were no twigs in that tree
    else if(LinkList_isEmpty(oLinkListNew) == 0){
      LinkList_free(oLinkListNew);
      oLinkListNew = NULL;
    }
    //if there were twigs
    else{
      //if there is nothing in storage yet
      if(LinkList_isEmpty(oLinkListStorage) == 0){
	LinkList_free(oLinkListStorage);
	oLinkListStorage = oLinkListNew;
      }
      //if there is something in storage already
      else{
	LinkList_setNext\
	  (LinkList_getLast(oLinkListStorage), oLinkListNew);
      }
    }
  }
  return oLinkListStorage;
}

/*---------------------------------------------------------------------*/
/*Return a pointer to the next twig in the expression list of oExpParse*/
/*Return NULL if there are no twigs                                    */
/*---------------------------------------------------------------------*/
static TreeNode_T ExpParser_nextTwig(ExpParser_T oExpParser){
  TreeNode_T oTreeNodeCurrent, oTreeNodeTwig;
  LinkList_T oLinkList, oLinkListChild;
  assert(oExpParser != NULL);
  //loop through the expression list until it is empty
  for(oLinkList = oExpParser -> pllExpressions;
      oLinkList != NULL;
      oLinkList = LinkList_getNext(oLinkList)){
    oTreeNodeCurrent = (TreeNode_T)LinkList_getValue(oLinkList);
    //get the first twig
    oTreeNodeTwig = TreeNode_getTwig(oTreeNodeCurrent);
    //if there was one return
    if(oTreeNodeTwig != NULL)
      return oTreeNodeTwig;
  }
  return NULL;
}


/*---------------------------------------------------------------------*/
/*Return a pointer to a representative example of the most common      */
/*subtree in the expression list of oExpParser. Only look at leaves and*/
/*their immediate parents. Return NULL if there are no trees in the    */
/*or you run out of memory                                             */
/*---------------------------------------------------------------------*/
static TreeNode_T ExpParser_mostCommonSubTree(ExpParser_T oExpParser){
  TreeNode_T oTreeNodeBest, oTreeNodeCurrent;
  LinkList_T oLinkListTwigs, oLinkListCurrent;
  int iBest = 0;
  int iCurrent;
  assert(oExpParser != NULL);
  //intialize oTreeNodeBest to NULL
  oTreeNodeBest = NULL;
  //get a list of all of the twigs in pllExpressions
  oLinkListTwigs = ExpParser_expressionTwigs(oExpParser);
  //check for insufficient memory
  if(oLinkListTwigs == NULL){
    fprintf\
      (stderr,\
       "%s", "ExpParser_mostCommonSubTree: insufficient memory\n");
    return NULL;
  }
  else if(LinkList_isEmpty(oLinkListTwigs) == 0){
    LinkList_free(oLinkListTwigs);
    return NULL;
  }
  //loop through the twigs checking how much each one is represented
  for(oLinkListCurrent = oLinkListTwigs;
      oLinkListCurrent != NULL;
      oLinkListCurrent = LinkList_getNext(oLinkListCurrent)){
    //how many times does this subtree occur
    oTreeNodeCurrent = (TreeNode_T)LinkList_getValue(oLinkListCurrent);
    iCurrent = ExpParser_howManySubTrees(oExpParser, oTreeNodeCurrent);
    //check for memory insufficiency
    if(iCurrent == -1){
      LinkList_freeRecursive(oLinkListTwigs);
      fprintf\
	(stderr,\
	"%s", "ExpParser_mostCommonSubTree: insufficient memory\n");
      return NULL;
    }
    //is it better than the current best
    if(iCurrent > iBest){
      iBest = iCurrent;
      oTreeNodeBest = oTreeNodeCurrent;
    }
  }
  //free the twig list
  LinkList_freeRecursive(oLinkListTwigs);
  //return the most common subtree
  return oTreeNodeBest;
}

/*---------------------------------------------------------------------*/
/*Create a new TreeNode storing an operand type exptoken with a name   */
/*that is the var char followed by the characters representing uiVarNum*/
/*Return NULL if memory is insufficient                                */
/*---------------------------------------------------------------------*/
static TreeNode_T ExpParser_makeNewVar(unsigned int uiVarNum){
  ExpToken_T oExpToken;
  TreeNode_T oTreeNode;
  unsigned int uiIndex;
  unsigned int uiNumDigits = 9u;//can accomodate uiVarNum up to 10^9
  char pcVarNum[uiNumDigits+1u];
  oExpToken = ExpToken_new();
  if(oExpToken == NULL)
    return NULL;
  //setup oExpToken
  ExpToken_setType(oExpToken, OPERAND);
  //translate uiVarNum to a string
  sprintf(pcVarNum, "%u", uiVarNum);
  //read the var char into the new token
  if(ExpToken_appendChar(oExpToken, VAR_CHAR) != 0){
    ExpToken_free(oExpToken);
    return NULL;
  }
  //read the character representation of uiVarNum int the token
  for(uiIndex = 0u; pcVarNum[uiIndex] != '\0'; uiIndex++){
    if(ExpToken_appendChar(oExpToken, pcVarNum[uiIndex]) != 0){
      ExpToken_free(oExpToken);
      return NULL;
    }
  }
  //allocate space for the TreeNode
  oTreeNode = ExpTokenTree_new();
  if(oTreeNode == NULL){
    ExpToken_free(oExpToken);
    return NULL;
  }
  //setup the TreeNode
  ExpTokenTree_setValue(oTreeNode, oExpToken);
  return oTreeNode;
} 

/*---------------------------------------------------------------------*/
/*Iteratively replace instances of oTreeNodeFind with oTreeNodeReplace */
/*in all of the trees in pllExpressions. return 0 if successful, 1 if  */
/*memory was insufficient                                              */
/*---------------------------------------------------------------------*/
static int ExpParser_replace(ExpParser_T oExpParser,\
			     TreeNode_T oTreeNodeFind,\
			     TreeNode_T oTreeNodeReplace){
  LinkList_T oLinkListTree;
  TreeNode_T oTreeNodeNew;
  assert(oExpParser != NULL);
  assert(oTreeNodeFind != NULL);
  assert(oTreeNodeReplace != NULL);
  //loop through every tree in pllExpressions
  for(oLinkListTree = oExpParser -> pllExpressions;
      oLinkListTree != NULL;
      oLinkListTree = LinkList_getNext(oLinkListTree)){
    //ExpParser_print(oExpParser);
    //ExpTokenTree_print(oTreeNodeFind);
    //ExpTokenTree_print(oTreeNodeReplace);
    //find replace
    oTreeNodeNew =\
      ExpTokenTree_replace((TreeNode_T)LinkList_getValue(oLinkListTree)\
			   , oTreeNodeFind, oTreeNodeReplace);
    //ExpTokenTree_print(oTreeNodeNew);
    //check for memory insufficiency
    if(oTreeNodeNew == NULL){
      fprintf(stderr, "%s", "ExpParser_replace: insufficient memory\n");
      return 0;
    }
    //Free the old tree and replace it with the new one
    ExpTokenTree_free((TreeNode_T)LinkList_getValue(oLinkListTree));
    LinkList_setValue(oLinkListTree, oTreeNodeNew);
  }
  return 0;
}

/*---------------------------------------------------------------------*/
/*Iteratively simplify the expression trees in pllExpressions by       */
/*substituting new variables for the twigs of the trees until no twigs */
/*remain. Print an infix representation of each substitution to stdout */
/*If an error occurs, print an informative message to stderr           */
/*---------------------------------------------------------------------*/
static void ExpParser_simplify(ExpParser_T oExpParser){
  TreeNode_T oTreeNodeFind, oTreeNodeReplace;
  unsigned int uiVarNum = 0u;
  assert(oExpParser != NULL);
  //loop until no more twigs or memory insufficient
  for(oTreeNodeFind =ExpParser_nextTwig(oExpParser);
      oTreeNodeFind != NULL;
      oTreeNodeFind =\
	ExpParser_nextTwig(oExpParser)){
    //copy the value of oTreeNodeFind
    oTreeNodeFind = ExpTokenTree_copy(oTreeNodeFind);
    //ExpParser_print(oExpParser);
    //ExpTokenTree_print(oTreeNodeFind);
    //generate the variable to substitute
    oTreeNodeReplace = ExpParser_makeNewVar(uiVarNum);
    //ExpTokenTree_print(oTreeNodeReplace);
    //check for memory insufficiency
    if(oTreeNodeReplace == NULL){
      fprintf(stderr, "%s", "ExpParser_simplify: insufficient memory\n");
      break;
    }
    //replace all subtrees
    if(ExpParser_replace\
       (oExpParser, oTreeNodeFind, oTreeNodeReplace) != 0){
      fprintf(stderr, "%s", "ExpParser_simplify: insufficient memory\n");
      ExpTokenTree_free(oTreeNodeReplace);
      break;
    }
    //print an infix representation of the substitution
    ExpTokenTree_print(oTreeNodeReplace);
    printf(" = ");
    ExpTokenTree_print(oTreeNodeFind);
    printf("\n");
    //free oTreeNodeReplace
    ExpTokenTree_free(oTreeNodeReplace);
    //free oTreeNodeFind
    ExpTokenTree_free(oTreeNodeFind);
    //update the variable number
    uiVarNum++;
  }
  //print a representation of the completed ExpPaser
  ExpParser_print(oExpParser);
}
    
      

/*---------------------------------------------------------------------*/
/*Read infix expressions in from stdin, ignoring whitespace and convert*/
/*them to expression trees. Print the interpreted expressions back to  */
/*stdout                                                               */
/*---------------------------------------------------------------------*/
int main(void){
  enum DFAState state = START;
  ExpParser_T oExpParser = ExpParser_new();
  
  //TreeNode_T oTreeNodeMostCommon;
  
  /*
  LinkList_T oLinkListTwigs, oLinkListScan;
  unsigned int uiTwig = 0u;
  */
  
  /*
  ExpToken_T oExpTokenSearch;
  TreeNode_T oTreeNodeSearch;
  */
  if(oExpParser == NULL){
    fprintf(stderr, "%s", "ExpParser: insufficient memory\n");
    return EXIT_FAILURE;
  }

  //intialize things for testing
  /*
  oExpTokenSearch = ExpToken_new();
  ExpToken_setType(oExpTokenSearch, OPERAND);
  ExpToken_appendChar(oExpTokenSearch, 'p');
  oTreeNodeSearch = ExpTokenTree_new();
  ExpTokenTree_setValue(oTreeNodeSearch, oExpTokenSearch);
  */

  //loop until you reach error or completion
  while(0==0){
    //print the state of the parser
    //ExpParser_print(oExpParser);
    //fprintf(stdout, "%u\n", ExpTokenTreeStack_howMany(oExpParser -> pettsOperandStack));
    //fflush(stdout);
    switch(state)
      {
      case START:
	state = ExpParser_handleDFA_START(oExpParser);
	break;
      case AFTER_OPERATOR:
	state = ExpParser_handleDFA_AFTER_OPERATOR(oExpParser);
	break;
      case AFTER_OPERAND:
	state = ExpParser_handleDFA_AFTER_OPERAND(oExpParser);
	break;
      case ERROR:
	//free the memory allocated
	ExpParser_free(oExpParser);
	//return an error signal
	return EXIT_FAILURE;
      case COMPLETE:
	//Simplify and print the ExpParser
	ExpParser_simplify(oExpParser);
	
	/*
	//testing code
	oTreeNodeMostCommon = ExpParser_mostCommonSubTree(oExpParser);
	printf("Most Common Twig: ");
	ExpTokenTree_print(oTreeNodeMostCommon);
	printf("\n");
	*/


	/*
	//testing code
	oLinkListTwigs = ExpParser_expressionTwigs(oExpParser);
	for(oLinkListScan = oLinkListTwigs;
	    oLinkListScan != NULL;
	    oLinkListScan = LinkList_getNext(oLinkListScan)){
	  if(LinkList_isEmpty(oLinkListScan) == 0)
	    break;
	  fprintf(stdout, "---Twig %u---\n", uiTwig);
	  ExpTokenTree_print\
	    ((TreeNode_T)LinkList_getValue(oLinkListScan));
	  printf("\n");
	  uiTwig++;
	}
	LinkList_freeRecursive(oLinkListTwigs);
	*/
	
	
	//testing code
	/*
	  fprintf(stdout, "%i instances of '%s'\n",			\
		ExpParser_howManySubTrees(oExpParser, oTreeNodeSearch),\
		ExpToken_toString(oExpTokenSearch));
	ExpTokenTree_free(oTreeNodeSearch);
	*/
	
	//free the memory
	ExpParser_free(oExpParser);
	//return a successful exit status
	return EXIT_SUCCESS;
      }
  }
}
