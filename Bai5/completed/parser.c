/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // [ADDED] dùng strcpy lưu tên định danh

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "debug.h"

Token *currentToken;
Token *lookAhead;

extern Type *intType;
extern Type *charType;
extern SymTab *symtab;

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
    scan();
  }
  else
    missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void)
{
  // TODO: create, enter, and exit program block
  Object *program;
  Token *programToken;

  eat(KW_PROGRAM);

  // [ADDED] Lấy tên chương trình trước khi eat
  programToken = lookAhead;
  eat(TK_IDENT);

  // [ADDED] Tạo object PROGRAM và vào scope
  program = createProgramObject(programToken->string);
  enterBlock(program->progAttrs->scope);

  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);

  // [ADDED] Thoát scope chương trình
  exitBlock();
}

void compileBlock(void)
{
  // TODO: create and declare constant objects
  Object *constObj;
  Token *identToken;
  ConstantValue *constVal;
  char identName[MAX_IDENT_LEN + 1]; // [ADDED] Sao lưu tên trước khi token bị free

  if (lookAhead->tokenType == KW_CONST)
  {
    eat(KW_CONST);

    do
    {
      // [ADDED] Lấy tên hằng trước khi eat
      identToken = lookAhead;
      strcpy(identName, identToken->string); // [ADDED]
      eat(TK_IDENT);
      eat(SB_EQ);

      // [ADDED] Parse giá trị hằng
      constVal = compileConstant();
      eat(SB_SEMICOLON);

      // [ADDED] Kiểm tra khai báo trùng lặp
      if (findObject(symtab->currentScope->objList, identToken->string) != NULL)
      {
        error(ERR_DUPLICATE_IDENT, identToken->lineNo, identToken->colNo);
      }

      // [ADDED] Tạo constant object và khai báo
      constObj = createConstantObject(identName);
      constObj->constAttrs->value = constVal;
      declareObject(constObj);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock2();
  }
  else
    compileBlock2();
}

void compileBlock2(void)
{
  // TODO: create and declare type objects
  Object *typeObj;
  Token *identToken;
  Type *typeVal;
  char identName[MAX_IDENT_LEN + 1]; // [ADDED] Sao lưu tên trước khi token bị free

  if (lookAhead->tokenType == KW_TYPE)
  {
    eat(KW_TYPE);

    do
    {
      // [ADDED] Lấy tên kiểu trước khi eat
      identToken = lookAhead;
      strcpy(identName, identToken->string); // [ADDED]
      eat(TK_IDENT);
      eat(SB_EQ);

      // [ADDED] Parse định nghĩa kiểu
      typeVal = compileType();
      eat(SB_SEMICOLON);

      // [ADDED] Kiểm tra khai báo trùng lặp
      if (findObject(symtab->currentScope->objList, identToken->string) != NULL)
      {
        error(ERR_DUPLICATE_IDENT, identToken->lineNo, identToken->colNo);
      }

      // [ADDED] Tạo type object và khai báo
      typeObj = createTypeObject(identName);
      typeObj->typeAttrs->actualType = typeVal;
      declareObject(typeObj);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock3();
  }
  else
    compileBlock3();
}

void compileBlock3(void)
{
  // TODO: create and declare variable objects
  Object *varObj;
  Token *identToken;
  Type *varType;
  char identName[MAX_IDENT_LEN + 1]; // [ADDED] Sao lưu tên trước khi token bị free

  if (lookAhead->tokenType == KW_VAR)
  {
    eat(KW_VAR);

    do
    {
      // [ADDED] Lấy tên biến trước khi eat
      identToken = lookAhead;
      strcpy(identName, identToken->string); // [ADDED]
      eat(TK_IDENT);
      eat(SB_COLON);

      // [ADDED] Parse kiểu của biến
      varType = compileType();
      eat(SB_SEMICOLON);

      // [ADDED] Kiểm tra khai báo trùng lặp
      if (findObject(symtab->currentScope->objList, identToken->string) != NULL)
      {
        error(ERR_DUPLICATE_IDENT, identToken->lineNo, identToken->colNo);
      }

      // [ADDED] Tạo variable object và khai báo
      varObj = createVariableObject(identName);
      varObj->varAttrs->type = varType;
      declareObject(varObj);
    } while (lookAhead->tokenType == TK_IDENT);

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

void compileSubDecls(void)
{
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE))
  {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else
      compileProcDecl();
  }
}
// Finished
void compileFuncDecl(void)
{
  // TODO: create and declare a function object
  Object *func;
  Token *funcToken;
  char funcName[MAX_IDENT_LEN + 1]; // [ADDED]
  Type *returnType;

  eat(KW_FUNCTION);

  // [ADDED] Lấy tên hàm
  funcToken = lookAhead;
  strcpy(funcName, funcToken->string); // [ADDED]
  eat(TK_IDENT);

  // [ADDED] Tạo function object
  func = createFunctionObject(funcName);

  // [ADDED] Kiểm tra trùng lặp trong scope hiện tại
  if (findObject(symtab->currentScope->objList, funcToken->string) != NULL)
  {
    error(ERR_DUPLICATE_IDENT, funcToken->lineNo, funcToken->colNo);
  }

  // [ADDED] Khai báo function trong scope hiện tại (cho phép đệ quy)
  declareObject(func);

  // [ADDED] Vào scope của hàm để parse tham số và body
  enterBlock(func->funcAttrs->scope);

  compileParams();
  eat(SB_COLON);

  // [ADDED] Parse kiểu trả về
  returnType = compileBasicType();
  func->funcAttrs->returnType = returnType;

  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);

  // [ADDED] Thoát scope hàm
  exitBlock();
}

void compileProcDecl(void)
{
  // TODO: create and declare a procedure object
  Object *proc;
  Token *procToken;
  char procName[MAX_IDENT_LEN + 1]; // [ADDED]

  eat(KW_PROCEDURE);

  // [ADDED] Lấy tên thủ tục
  procToken = lookAhead;
  strcpy(procName, procToken->string); // [ADDED]
  eat(TK_IDENT);

  // [ADDED] Tạo procedure object
  proc = createProcedureObject(procName);

  // [ADDED] Kiểm tra trùng lặp trong scope hiện tại
  if (findObject(symtab->currentScope->objList, procToken->string) != NULL)
  {
    error(ERR_DUPLICATE_IDENT, procToken->lineNo, procToken->colNo);
  }

  // [ADDED] Khai báo procedure trong scope hiện tại (cho phép đệ quy)
  declareObject(proc);

  // [ADDED] Vào scope của thủ tục để parse tham số và body
  enterBlock(proc->procAttrs->scope);

  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);

  // [ADDED] Thoát scope thủ tục
  exitBlock();
}

ConstantValue *compileUnsignedConstant(void)
{
  // TODO: create and return an unsigned constant value
  ConstantValue *constValue = NULL;
  Token *numToken;
  Object *constObj;

  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    // [ADDED] Tạo hằng số từ giá trị số
    numToken = lookAhead;
    eat(TK_NUMBER);
    constValue = makeIntConstant(numToken->value);
    break;
  case TK_IDENT:
    // [ADDED] Tham chiếu đến hằng số được khai báo
    numToken = lookAhead;
    eat(TK_IDENT);

    // [ADDED] Tìm hằng số trong scope chain
    constObj = lookupObject(numToken->string);
    if (constObj == NULL)
    {
      error(ERR_UNDECLARED_IDENT, numToken->lineNo, numToken->colNo);
    }
    // [ADDED] Kiểm tra đó là hằng số
    if (constObj->kind != OBJ_CONSTANT)
    {
      error(ERR_INVALID_CONSTANT, numToken->lineNo, numToken->colNo);
    }
    // [ADDED] Sao chép giá trị hằng
    constValue = duplicateConstantValue(constObj->constAttrs->value);
    break;
  case TK_CHAR:
    // [ADDED] Tạo hằng số ký tự
    numToken = lookAhead;
    eat(TK_CHAR);
    constValue = makeCharConstant(numToken->string[0]);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

ConstantValue *compileConstant(void)
{
  // TODO: create and return a constant value
  ConstantValue *constValue = NULL;

  // [ADDED] Constant = [+|-] Constant2 | ConstChar

  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    // [ADDED] Dấu cộng (dương)
    eat(SB_PLUS);
    constValue = compileConstant2();
    // [ADDED] Kiểm tra kiểu là số nguyên
    if (constValue != NULL && constValue->type != TP_INT)
    {
      error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
    break;
  case SB_MINUS:
    // [ADDED] Dấu trừ (âm)
    eat(SB_MINUS);
    constValue = compileConstant2();
    // [ADDED] Kiểm tra kiểu là số nguyên và đảo dấu
    if (constValue != NULL && constValue->type != TP_INT)
    {
      error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
    if (constValue != NULL)
    {
      constValue->intValue = -constValue->intValue;
    }
    break;
  case TK_CHAR:
    // [ADDED] Hằng số ký tự
    constValue = makeCharConstant(lookAhead->string[0]);
    eat(TK_CHAR);
    break;
  default:
    // [ADDED] Mặc định: Constant2 không dấu
    constValue = compileConstant2();
    break;
  }
  return constValue;
}

ConstantValue *compileConstant2(void)
{
  // TODO: create and return a constant value
  ConstantValue *constValue = NULL;
  Token *numToken;
  Object *constObj;

  // [ADDED] Constant2 = Number | ConstIdent
  // Number: TK_NUMBER
  // ConstIdent: TK_IDENT (phải là hằng số)

  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    // [ADDED] Số nguyên
    numToken = lookAhead;
    eat(TK_NUMBER);
    constValue = makeIntConstant(numToken->value);
    break;
  case TK_IDENT:
    // [ADDED] Tham chiếu đến hằng số được khai báo
    numToken = lookAhead;
    eat(TK_IDENT);

    // [ADDED] Tìm đối tượng trong scope chain
    constObj = lookupObject(numToken->string);
    if (constObj == NULL)
    {
      error(ERR_UNDECLARED_IDENT, numToken->lineNo, numToken->colNo);
    }

    // [ADDED] Kiểm tra đó là hằng số
    if (constObj->kind != OBJ_CONSTANT)
    {
      error(ERR_INVALID_CONSTANT, numToken->lineNo, numToken->colNo);
    }

    // [ADDED] Sao chép giá trị hằng
    constValue = duplicateConstantValue(constObj->constAttrs->value);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

Type *compileType(void)
{
  // TODO: create and return a type
  Type *type = NULL;
  Type *elementType = NULL;
  int arraySize;
  Token *typeIdent;
  char typeName[MAX_IDENT_LEN + 1]; // [ADDED]
  Object *typeObj;

  // [ADDED] Type = BasicType | ARRAY[n] OF Type | TypeIdent

  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
  case KW_CHAR:
    // [ADDED] BasicType
    type = compileBasicType();
    break;
  case KW_ARRAY:
    // [ADDED] ARRAY[n] OF Type
    eat(KW_ARRAY);
    eat(SB_LSEL);

    // [ADDED] Lấy kích thước mảng
    if (lookAhead->tokenType != TK_NUMBER)
    {
      error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
      arraySize = 1;
    }
    else
    {
      arraySize = lookAhead->value;
      eat(TK_NUMBER);
    }

    eat(SB_RSEL);
    eat(KW_OF);

    // [ADDED] Loại phần tử (đệ quy)
    elementType = compileType();

    // [ADDED] Tạo kiểu mảng
    if (elementType != NULL)
    {
      type = makeArrayType(arraySize, elementType);
    }
    break;
  case TK_IDENT:
    // [ADDED] Tham chiếu đến kiểu được khai báo
    typeIdent = lookAhead;
    strcpy(typeName, typeIdent->string); // [ADDED]
    eat(TK_IDENT);

    // [ADDED] Tìm kiểu trong scope chain
    typeObj = lookupObject(typeName);
    if (typeObj == NULL)
    {
      error(ERR_UNDECLARED_IDENT, typeIdent->lineNo, typeIdent->colNo);
    }

    // [ADDED] Kiểm tra đó là kiểu
    if (typeObj->kind != OBJ_TYPE)
    {
      error(ERR_INVALID_TYPE, typeIdent->lineNo, typeIdent->colNo);
    }

    // [ADDED] Sao chép kiểu được tham chiếu
    if (typeObj != NULL)
    {
      type = duplicateType(typeObj->typeAttrs->actualType);
    }
    break;
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

Type *compileBasicType(void)
{
  // TODO: create and return a basic type
  Type *type = NULL;

  // [ADDED] BasicType = INTEGER | CHAR

  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
    // [ADDED] Kiểu INTEGER
    eat(KW_INTEGER);
    type = duplicateType(intType);
    break;
  case KW_CHAR:
    // [ADDED] Kiểu CHAR
    eat(KW_CHAR);
    type = duplicateType(charType);
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

void compileParams(void)
{
  if (lookAhead->tokenType == SB_LPAR)
  {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON)
    {
      eat(SB_SEMICOLON);
      compileParam();
    }
    eat(SB_RPAR);
  }
}

void compileParam(void)
{
  // TODO: create and declare a parameter
  Token *paramToken;
  char paramName[MAX_IDENT_LEN + 1]; // [ADDED]
  Type *paramType = NULL;
  Object *paramObj;
  int paramKind = PARAM_VALUE;

  // [ADDED] Param = [VAR] <ident> : BasicType

  if (lookAhead->tokenType == KW_VAR)
  {
    // [ADDED] Tham số qua tham chiếu
    eat(KW_VAR);
    paramKind = PARAM_REFERENCE;
  }

  // [ADDED] Lấy tên tham số
  if (lookAhead->tokenType != TK_IDENT)
  {
    error(ERR_INVALID_PARAMETER, lookAhead->lineNo, lookAhead->colNo);
    return;
  }

  paramToken = lookAhead;
  strcpy(paramName, paramToken->string); // [ADDED]
  eat(TK_IDENT);
  eat(SB_COLON);

  // [ADDED] Parse kiểu cơ bản
  paramType = compileBasicType();

  // [ADDED] Kiểm tra trùng tên tham số
  if (findObject(symtab->currentScope->objList, paramToken->string) != NULL)
  {
    error(ERR_DUPLICATE_IDENT, paramToken->lineNo, paramToken->colNo);
  }

  // [ADDED] Tạo đối tượng tham số, owner là hàm/thủ tục hiện tại
  paramObj = createParameterObject(paramName, paramKind, symtab->currentScope->owner);
  paramObj->paramAttrs->type = paramType;

  // [ADDED] Khai báo tham số trong scope hiện tại
  declareObject(paramObj);
}

void compileStatements(void)
{
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON)
  {
    eat(SB_SEMICOLON);
    compileStatement();
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
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileLValue(void)
{
  eat(TK_IDENT);
  compileIndexes();
}

void compileAssignSt(void)
{
  compileLValue();
  eat(SB_ASSIGN);
  compileExpression();
}

void compileCallSt(void)
{
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
}

void compileGroupSt(void)
{
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileIfSt(void)
{
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE)
    compileElseSt();
}

void compileElseSt(void)
{
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void)
{
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
}

void compileForSt(void)
{
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
}

void compileArgument(void)
{
  compileExpression();
}

void compileArguments(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_LPAR:
    eat(SB_LPAR);
    compileArgument();

    while (lookAhead->tokenType == SB_COMMA)
    {
      eat(SB_COMMA);
      compileArgument();
    }

    eat(SB_RPAR);
    break;
    // Check FOLLOW set
  case SB_TIMES:
  case SB_SLASH:
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void)
{
  compileExpression();
  switch (lookAhead->tokenType)
  {
  case SB_EQ:
    eat(SB_EQ);
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    break;
  case SB_LE:
    eat(SB_LE);
    break;
  case SB_LT:
    eat(SB_LT);
    break;
  case SB_GE:
    eat(SB_GE);
    break;
  case SB_GT:
    eat(SB_GT);
    break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }

  compileExpression();
}

void compileExpression(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    compileExpression2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileExpression2();
    break;
  default:
    compileExpression2();
  }
}

void compileExpression2(void)
{
  compileTerm();
  compileExpression3();
}

void compileExpression3(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
    break;
    // check the FOLLOW set
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileTerm(void)
{
  compileFactor();
  compileTerm2();
}

void compileTerm2(void)
{
  switch (lookAhead->tokenType)
  {
  case SB_TIMES:
    eat(SB_TIMES);
    compileFactor();
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    compileFactor();
    compileTerm2();
    break;
    // check the FOLLOW set
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileFactor(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    switch (lookAhead->tokenType)
    {
    case SB_LPAR:
      compileArguments();
      break;
    case SB_LSEL:
      compileIndexes();
      break;
    default:
      break;
    }
    break;
  default:
    error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileIndexes(void)
{
  while (lookAhead->tokenType == SB_LSEL)
  {
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
  }
}

int compile(char *fileName)
{
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  initSymTab();

  compileProgram();

  printObject(symtab->program, 0);

  cleanSymTab();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;
}
