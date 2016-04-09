/*---------------------------------------------------------------------*/
/*Author: Owen Larrabee Coyle                                          */
/*exptoken.c implements exptoken.h, a subunit of a math expression     */
/*---------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "exptoken.h"

/*define the maximum number of chars in a name*/
#define MAX_CHARS 10u

/*---------------------------------------------------------------------*/
/*ExpTokens store a token's type and it's name                         */
/*---------------------------------------------------------------------*/
struct ExpToken
{
      /*type*/
      enum ExpTokenType ettType;
      /*name*/
      char pcName[MAX_CHARS+1];
};

/*---------------------------------------------------------------------*/
/*Return a new ExpToken object with pcName full of '\0' and            */ 
/*ettType = OPERAND. If memory is full, return NULL                    */
/*---------------------------------------------------------------------*/
ExpToken_T ExpToken_new(void){
   ExpToken_T oExpToken;
   unsigned int  uiIndex;
   oExpToken = (ExpToken_T)malloc(sizeof(struct ExpToken));
   if(oExpToken == NULL)
      return NULL;
   oExpToken -> ettType = OPERAND;
   for(uiIndex = 0u; uiIndex < MAX_CHARS+1; uiIndex++)
      (oExpToken -> pcName)[uiIndex] = '\0';
   return oExpToken;
}

/*---------------------------------------------------------------------*/
/*Free all memory associated with oExpToken                            */
/*---------------------------------------------------------------------*/
void ExpToken_free(ExpToken_T oExpToken){
   assert(oExpToken != NULL);
   free(oExpToken);
   oExpToken = NULL;
}

/*---------------------------------------------------------------------*/
/*Return oExpToken's type. This may be NULL if the type has not been   */
/*set since creation                                                   */
/*---------------------------------------------------------------------*/
enum ExpTokenType ExpToken_getType(ExpToken_T oExpToken) {
   assert(oExpToken != NULL);
   return oExpToken -> ettType;
}

/*---------------------------------------------------------------------*/
/*Return a point to oExpToken's name. If oExpToken is then freed       */
/*this will create a dangling pointer. The user must take care not to  */
/*corrupt oExpToken's name accidentally in using this pointer          */
/*---------------------------------------------------------------------*/
char* ExpToken_getName(ExpToken_T oExpToken){
   assert(oExpToken != NULL);
   return (oExpToken->pcName);
}

/*---------------------------------------------------------------------*/
/*Set the ExpTokenType of oExpToken                                    */
/*---------------------------------------------------------------------*/
void ExpToken_setType(ExpToken_T oExpToken, enum ExpTokenType ettType) {
   assert(oExpToken != NULL);
   oExpToken -> ettType = ettType;
}

/*---------------------------------------------------------------------*/
/*Append char c to the end of oExpToken's name. If no room remains do  */
/*not alter oExpToken's name, but print a warning to stderr. Return 0  */
/*if successful, 1 if string is full.                                  */
/*---------------------------------------------------------------------*/
int ExpToken_appendChar(ExpToken_T oExpToken, char c){
   unsigned int uiIndex;
   assert(oExpToken != NULL);
   /*loop through looking for null characters*/
   for(uiIndex = 0u; uiIndex < MAX_CHARS; uiIndex++){
      if(oExpToken->pcName[uiIndex] == '\0'){
         oExpToken->pcName[uiIndex] = c;
         return 0;
      }
   }
   fprintf(stderr, "%s", "ExpToken_appendChar: char buffer full\n");
   return 1;
}

/*---------------------------------------------------------------------*/
/*Compare two ExpTokens. Return 0 if they are equivalent, 1 if not.    */
/*---------------------------------------------------------------------*/
int ExpToken_compare(ExpToken_T oExpToken1, ExpToken_T oExpToken2){
  assert(oExpToken1 != NULL);
  assert(oExpToken2 != NULL);
  if(oExpToken1 -> ettType == oExpToken2 -> ettType)
    if(strcmp(oExpToken1 -> pcName, oExpToken2 -> pcName) == 0)
      return 0;
  return 1;
}

/*---------------------------------------------------------------------*/
/*Compare the precedence of two ExpTokens. Return 0 if they have equal */
/*precedence, -1 if oExpToken1 has lower precedence that oExpToken2,   */
/*and 1 if oExpToken1 has higher precedence than oExpToken2. Return -2 */
/*if the precedence of the two operators cannot be compared.           */
/*---------------------------------------------------------------------*/
int ExpToken_precedence(ExpToken_T oExpToken1, ExpToken_T oExpToken2){
  assert(oExpToken1 != NULL);
  assert(oExpToken2 != NULL);
  switch(oExpToken1 -> ettType)
    {
    case OPERAND:
      return -2;
    case POSITIVE:
      switch(oExpToken2 -> ettType)
	{
	case OPERAND:
	  return -2;
	case POSITIVE:
	  return 0;
	case NEGATIVE:
	  return 0;
	case ADD:
	  return 1;
	case SUBTRACT:
	  return 1;
	case MULTIPLY:
	  return -1;
	case DIVIDE:
	  return -1;
	case POWER:
	  return -1;
	case LEFT_BRACKET:
	  return -2;
	case RIGHT_BRACKET:
	  return -2;
	default:
	  assert(0);
	  break;
	}
      break;
    case NEGATIVE:
      switch(oExpToken2 -> ettType)
	{
	case OPERAND:
	  return -2;
	case POSITIVE:
	  return 0;
	case NEGATIVE:
	  return 0;
	case ADD:
	  return 1;
	case SUBTRACT:
	  return 1;
	case MULTIPLY:
	  return -1;
	case DIVIDE:
	  return -1;
	case POWER:
	  return -1;
	case LEFT_BRACKET:
	  return -2;
	case RIGHT_BRACKET:
	  return -2;
	default:
	  assert(0);
	  break;
	}
      break;
    case ADD:
      switch(oExpToken2 -> ettType)
	{
	case OPERAND:
	  return -2;
	case POSITIVE:
	  return -1;
	case NEGATIVE:
	  return -1;
	case ADD:
	  return 0;
	case SUBTRACT:
	  return 0;
	case MULTIPLY:
	  return -1;
	case DIVIDE:
	  return -1;
	case POWER:
	  return -1;
	case LEFT_BRACKET:
	  return -2;
	case RIGHT_BRACKET:
	  return -2;
	default:
	  assert(0);
	  break;
	}
      break;
    case SUBTRACT:
      switch(oExpToken2 -> ettType)
	{
	case OPERAND:
	  return -2;
	case POSITIVE:
	  return -1;
	case NEGATIVE:
	  return -1;
	case ADD:
	  return 0;
	case SUBTRACT:
	  return 0;
	case MULTIPLY:
	  return -1;
	case DIVIDE:
	  return -1;
	case POWER:
	  return -1;
	case LEFT_BRACKET:
	  return -2;
	case RIGHT_BRACKET:
	  return -2;
	default:
	  assert(0);
	  break;
	}
      break;
    case MULTIPLY:
      switch(oExpToken2 -> ettType)
	{
	case OPERAND:
	  return -2;
	case POSITIVE:
	  return 1;
	case NEGATIVE:
	  return 1;
	case ADD:
	  return 1;
	case SUBTRACT:
	  return 1;
	case MULTIPLY:
	  return 0;
	case DIVIDE:
	  return 0;
	case POWER:
	  return -1;
	case LEFT_BRACKET:
	  return -2;
	case RIGHT_BRACKET:
	  return -2;
	default:
	  assert(0);
	  break;
	}
      break;
    case DIVIDE:
      switch(oExpToken2 -> ettType)
	{
	case OPERAND:
	  return -2;
	case POSITIVE:
	  return 1;
	case NEGATIVE:
	  return 1;
	case ADD:
	  return 1;
	case SUBTRACT:
	  return 1;
	case MULTIPLY:
	  return 0;
	case DIVIDE:
	  return 0;
	case POWER:
	  return -1;
	case LEFT_BRACKET:
	  return -2;
	case RIGHT_BRACKET:
	  return -2;
	default:
	  assert(0);
	  break;
	}
      break;
    case POWER:
      switch(oExpToken2 -> ettType)
	{
	case OPERAND:
	  return -2;
	case POSITIVE:
	  return 1;
	case NEGATIVE:
	  return 1;
	case ADD:
	  return 1;
	case SUBTRACT:
	  return 1;
	case MULTIPLY:
	  return 1;
	case DIVIDE:
	  return 1;
	case POWER:
	  return 0;
	case LEFT_BRACKET:
	  return -2;
	case RIGHT_BRACKET:
	  return -2;
	default:
	  assert(0);
	  break;
	}
      break;
    case LEFT_BRACKET:
      return -2;
    case RIGHT_BRACKET:
      return -2;
    default:
      assert(0);
      break;
    }
}

/*---------------------------------------------------------------------*/
/*Return a copy of oExpToken, or NULL if memory is insufficient        */
/*---------------------------------------------------------------------*/
ExpToken_T ExpToken_copy(ExpToken_T oExpToken){
  ExpToken_T oExpTokenNew;
  assert(oExpToken != NULL);
  oExpTokenNew = ExpToken_new();
  if(oExpTokenNew == NULL)
    return NULL;
  oExpTokenNew -> ettType = oExpToken -> ettType;
  strcpy(oExpTokenNew -> pcName, oExpToken -> pcName);
  return oExpTokenNew;
}

/*---------------------------------------------------------------------*/
/*Return the number of operands that oExpToken applies to. For Operand */
/*and Brackets ExpTokenTypes this will be 0.                           */
/*---------------------------------------------------------------------*/
int ExpToken_numOperands(ExpToken_T oExpToken){
  assert(oExpToken != NULL);
  switch(oExpToken -> ettType)
    {
    case OPERAND:
      return 0;
    case POSITIVE:
      return 1;
    case NEGATIVE:
      return 1;
    case ADD:
      return 2;
    case SUBTRACT:
      return 2;
    case MULTIPLY:
      return 2;
    case DIVIDE:
      return 2;
    case POWER:
      return 2;
    case LEFT_BRACKET:
      return 0;
    case RIGHT_BRACKET:
      return 0;
    default:
      assert(0);
    }
}

/*---------------------------------------------------------------------*/
/*Return a string that represents oExpToken. This will be the operator */
/*symbol for operators, and the operand name for operands.             */
/*---------------------------------------------------------------------*/
char* ExpToken_toString(ExpToken_T oExpToken){
  assert(oExpToken != NULL);
  switch(oExpToken -> ettType)
    {
    case OPERAND:
      return oExpToken -> pcName;
    case POSITIVE:
      return "+";
    case NEGATIVE:
      return "-";
    case ADD:
      return "+";
    case SUBTRACT:
      return "-";
    case MULTIPLY:
      return "*";
    case DIVIDE:
      return "/";
    case POWER:
      return "^";
    case LEFT_BRACKET:
      return "(";
    case RIGHT_BRACKET:
      return ")";
    default:
      assert(0);
    }
}

/*---------------------------------------------------------------------*/
/*Returns 1 if the ExpToken exhibits associativity, 0 if not           */
/*---------------------------------------------------------------------*/
int ExpToken_isAssociative(ExpToken_T oExpToken){
  assert(oExpToken != NULL);
  switch(oExpToken -> ettType)
    {
    case OPERAND:
      return 0;
    case POSITIVE:
      return 0;
    case NEGATIVE:
      return 0;
    case ADD:
      return 1;
    case SUBTRACT:
      return 0;
    case MULTIPLY:
      return 1;
    case DIVIDE:
      return 0;
    case POWER:
      return 0;
    case LEFT_BRACKET:
      return 0;
    case RIGHT_BRACKET:
      return 0;
    default:
      assert(0);
      return 0;
    }
}

/*---------------------------------------------------------------------*/
/*Test exptoken.c                                                      */
/*---------------------------------------------------------------------*/
/*
int main(void){
   ExpToken_T oExpToken;
   ExpToken_T oExpToken2;
   unsigned int uiIndex;
   
   printf("-------------------------------------------\n");
   printf("Testing exptoken.c\n");
   printf("-------------------------------------------\n");

   
   oExpToken = ExpToken_new();
   if(oExpToken == NULL)
      printf("failed test 1\n");
   
   
   ExpToken_free(oExpToken);

   oExpToken = ExpToken_new();
   
   
   ExpToken_setType(oExpToken, NEGATIVE);
   if(ExpToken_getType(oExpToken) != NEGATIVE)
      printf("failed test 2\n");

   if(ExpToken_appendChar(oExpToken, 'c') != 0)
      printf("failed test 3\n");
   if(strcmp(ExpToken_getName(oExpToken), "c") != 0)
      printf("failed test 4\n");

   
   for(uiIndex = 1; uiIndex < MAX_CHARS; uiIndex++){
      if(ExpToken_appendChar(oExpToken, 'c') != 0)
         printf("failed test 5\n");
   }
   if(strcmp(ExpToken_getName(oExpToken), "cccccccccc") != 0)
      printf("failed test 6\n");

   if(ExpToken_appendChar(oExpToken, 'c') != 1)
      printf("failed test 7\n");
   
   if(strcmp(ExpToken_getName(oExpToken), "cccccccccc") != 0)
      printf("failed test 8\n");
   
   oExpToken2 = ExpToken_copy(oExpToken);
   if(ExpToken_compare(oExpToken, oExpToken2) != 0)
     printf("failed test 9\n");
   if(ExpToken_precedence(oExpToken, oExpToken2) != 0)
     printf("failed test 10\n");
   


   ExpToken_free(oExpToken);
   ExpToken_free(oExpToken2);
   oExpToken = NULL;
   oExpToken2 = NULL;
   printf("-------------------------------------------\n");
   return 0;
}
*/
