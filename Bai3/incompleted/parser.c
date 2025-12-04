/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void)
{
  Token *tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType)
{
  if (lookAhead->tokenType == tokenType)
  {
    printToken(lookAhead);
    scan();
  }
  else
    missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void)
{
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void)
{
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST)
  {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  }
  else
    compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void)
{
  if (lookAhead->tokenType == KW_TYPE)
  {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  }
  else
    compileBlock3();
}

void compileBlock3(void)
{
  if (lookAhead->tokenType == KW_VAR)
  {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  }
  else
    compileBlock4();
}

void compileBlock4(void)
{
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void)
{
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == TK_IDENT)
  {
    compileConstDecl();
    compileConstDecls();
  }
}

void compileConstDecl(void)
{
  // TODO DONE
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void)
{
  // TODO DONE
  if (lookAhead->tokenType == TK_IDENT)
  {
    compileTypeDecl();
    compileTypeDecls();
  }
}

void compileTypeDecl(void)
{
  // TODO DONE
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void)
{
  // TODO DONE
  if (lookAhead->tokenType == TK_IDENT)
  {
    compileVarDecl();
    compileVarDecls();
  }
}

void compileVarDecl(void)
{
  // TODO DONE
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void)
{
  assert("Parsing subtoutines ....");
  // TODO DONE
  if (lookAhead->tokenType == KW_FUNCTION)
  {
    compileFuncDecl();
    compileSubDecls();
  }
  else if (lookAhead->tokenType == KW_PROCEDURE)
  {
    compileProcDecl();
    compileSubDecls();
  }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void)
{
  assert("Parsing a function ....");
  // TODO DONE?
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

void compileProcDecl(void)
{
  assert("Parsing a procedure ....");
  // TODO DONE?
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

void compileUnsignedConstant(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == TK_NUMBER)
  {
    eat(TK_NUMBER);
  }
  else if (lookAhead->tokenType == TK_CHAR)
  {
    eat(TK_CHAR);
  }
  else if (lookAhead->tokenType == TK_IDENT)
  {
    eat(TK_IDENT);
  }
  else
  {
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileConstant(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == SB_PLUS)
  {
    eat(SB_PLUS);
    compileConstant2();
  }
  else if (lookAhead->tokenType == SB_MINUS)
  {
    eat(SB_MINUS);
    compileConstant2();
  }
  else if (lookAhead->tokenType == TK_CHAR)
  {
    eat(TK_CHAR);
  }
  else
  {
    compileConstant2();
  }
}

void compileConstant2(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == TK_IDENT)
  {
    eat(TK_IDENT);
  }
  else if (lookAhead->tokenType == TK_NUMBER)
  {
    eat(TK_NUMBER);
  }
  else
  {
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileType(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == KW_INTEGER || lookAhead->tokenType == KW_CHAR)
  {
    compileBasicType();
  }
  else if (lookAhead->tokenType == KW_ARRAY)
  {
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    eat(SB_RSEL);
    eat(KW_OF);
    compileType();
  }
  else if (lookAhead->tokenType == TK_IDENT)
  {
    eat(TK_IDENT);
  }
  else
  {
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileBasicType(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == KW_INTEGER)
  {
    eat(KW_INTEGER);
  }
  else if (lookAhead->tokenType == KW_CHAR)
  {
    eat(KW_CHAR);
  }
  else
  {
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileParams(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == SB_LPAR)
  {
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  }
}

void compileParams2(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == SB_SEMICOLON)
  {
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }
  else
  {
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileParam(void)
{
  // TODO DONE?
  if (lookAhead->tokenType == TK_IDENT)
  {
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }
  else if (lookAhead->tokenType == KW_VAR)
  {
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }
}

void compileStatements(void)
{
  // TODO
  compileStatement();
  compileStatements2();
}

void compileStatements2(void)
{
  // TODO
  if (lookAhead->tokenType == SB_SEMICOLON)
  {
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  }
  else
  {
    // epsilon: nothing to do
  }
}

void compileStatement(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void)
{
  assert("Parsing an assign statement ....");
  // TODO
  if (lookAhead->tokenType==KW_VAR) {
    eat(KW_VAR);
    eat(SB_ASSIGN);
    compileExpression();
  } else if (lookAhead->tokenType==TK_IDENT) {
    eat(TK_IDENT);
    eat(SB_ASSIGN);
    compileExpression();
  }
  assert("Assign statement parsed ....");
}

void compileCallSt(void)
{
  assert("Parsing a call statement ....");
  // TODO
    eat(KW_CALL);
    eat(TK_IDENT);
    compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void)
{
  assert("Parsing a group statement ....");
  // TODO
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void)
{
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE)
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void)
{
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void)
{
  assert("Parsing a while statement ....");
  // TODO
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void)
{
  assert("Parsing a for statement ....");
  // TODO
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

void compileArguments(void)
{
  // TODO
  if (lookAhead->tokenType==SB_LPAR) {
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
  }
}

void compileArguments2(void)
{
  // TODO
  if (lookAhead->tokenType==SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
  } else {
    err(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void)
{
  // TODO
}

void compileCondition2(void)
{
  // TODO
}

void compileExpression(void)
{
  assert("Parsing an expression");
  // TODO
  assert("Expression parsed");
}

void compileExpression2(void)
{
  // TODO
}

void compileExpression3(void)
{
  // TODO
}

void compileTerm(void)
{
  // TODO
}

void compileTerm2(void)
{
  // TODO
}

void compileFactor(void)
{
  // TODO
}

void compileIndexes(void)
{
  // TODO
}

int compile(char *fileName)
{
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;
}
