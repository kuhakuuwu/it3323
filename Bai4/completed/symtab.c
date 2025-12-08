/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  Type* newType;
  
  if (type == NULL) return NULL;  
  
  newType = (Type*) malloc(sizeof(Type));
  newType->typeClass = type->typeClass;
  newType->arraySize = type->arraySize;
  
  if (type->typeClass == TP_ARRAY) {
    newType->elementType = duplicateType(type->elementType);
  } else {
    newType->elementType = NULL;
  }
  
  return newType;
}

int compareType(Type* type1, Type* type2) {
  
  if (type1 == NULL && type2 == NULL) return 1;
  if (type1 == NULL || type2 == NULL) return 0; 
  
  if (type1->typeClass != type2->typeClass) return 0;  
  
  if (type1->typeClass == TP_ARRAY) {
    if (type1->arraySize != type2->arraySize) return 0;
    return compareType(type1->elementType, type2->elementType);  
  }
  
  return 1;
}

void freeType(Type* type) {
  
  if (type == NULL) return;  
  
  if (type->typeClass == TP_ARRAY) {
    freeType(type->elementType);
  }

  free(type);
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {

  ConstantValue* value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = TP_INT;           
  value->intValue = i;            
  return value;
}

ConstantValue* makeCharConstant(char ch) {

  ConstantValue* value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = TP_CHAR;          
  value->charValue = ch;          
  return value;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {

  ConstantValue* newValue;
  
  if (v == NULL) return NULL;
  
  newValue = (ConstantValue*) malloc(sizeof(ConstantValue));
  newValue->type = v->type;
  
  // Copy giá trị dựa trên loại
  if (v->type == TP_INT) {
    newValue->intValue = v->intValue;
  } else if (v->type == TP_CHAR) {
    newValue->charValue = v->charValue;
  }
  
  return newValue;
}

/******************* Object utilities ******************************/

Scope* createScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* createProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* createConstantObject(char *name) {

  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);                    
  obj->kind = OBJ_CONSTANT;                   
  obj->constAttrs = (ConstantAttributes*) malloc(sizeof(ConstantAttributes));
  obj->constAttrs->value = NULL;              
  return obj;
}

Object* createTypeObject(char *name) {

  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);                    
  obj->kind = OBJ_TYPE;                       
  obj->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  obj->typeAttrs->actualType = NULL;          
  return obj;
}

Object* createVariableObject(char *name) {

  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);                    
  obj->kind = OBJ_VARIABLE;                 
  obj->varAttrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
  obj->varAttrs->type = NULL;                 
  obj->varAttrs->scope = symtab->currentScope;  
  return obj;
}

Object* createFunctionObject(char *name) {

  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);                    
  obj->kind = OBJ_FUNCTION;                   
  obj->funcAttrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
  obj->funcAttrs->paramList = NULL;           
  obj->funcAttrs->returnType = NULL;          
  obj->funcAttrs->scope = createScope(obj, symtab->currentScope);  
  return obj;
}

Object* createProcedureObject(char *name) {

  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);                    
  obj->kind = OBJ_PROCEDURE;                  
  obj->procAttrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
  obj->procAttrs->paramList = NULL;           
  obj->procAttrs->scope = createScope(obj, symtab->currentScope);  
  return obj;
}

Object* createParameterObject(char *name, enum ParamKind kind, Object* owner) {

  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);                    
  obj->kind = OBJ_PARAMETER;                  
  obj->paramAttrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
  obj->paramAttrs->kind = kind;               
  obj->paramAttrs->type = NULL;               
  obj->paramAttrs->function = owner;          
  return obj;
}

void freeObject(Object* obj) {
  
  if (obj == NULL) return;
  
  switch (obj->kind) {
  case OBJ_CONSTANT:
    
    if (obj->constAttrs) {
      if (obj->constAttrs->value) free(obj->constAttrs->value);
      free(obj->constAttrs);
    }
    break;
  case OBJ_TYPE:
    
    if (obj->typeAttrs) {
      if (obj->typeAttrs->actualType) freeType(obj->typeAttrs->actualType);
      free(obj->typeAttrs);
    }
    break;
  case OBJ_VARIABLE:
    
    if (obj->varAttrs) {
      if (obj->varAttrs->type) freeType(obj->varAttrs->type);
      free(obj->varAttrs);
    }
    break;
  case OBJ_FUNCTION:
    
    if (obj->funcAttrs) {
      if (obj->funcAttrs->paramList) freeObjectList(obj->funcAttrs->paramList);
      if (obj->funcAttrs->returnType) freeType(obj->funcAttrs->returnType);
      if (obj->funcAttrs->scope) freeScope(obj->funcAttrs->scope);
      free(obj->funcAttrs);
    }
    break;
  case OBJ_PROCEDURE:
    
    if (obj->procAttrs) {
      if (obj->procAttrs->paramList) freeObjectList(obj->procAttrs->paramList);
      if (obj->procAttrs->scope) freeScope(obj->procAttrs->scope);
      free(obj->procAttrs);
    }
    break;
  case OBJ_PROGRAM:
    
    if (obj->progAttrs) {
      if (obj->progAttrs->scope) freeScope(obj->progAttrs->scope);
      free(obj->progAttrs);
    }
    break;
  case OBJ_PARAMETER:
    
    if (obj->paramAttrs) {
      if (obj->paramAttrs->type) freeType(obj->paramAttrs->type);
      free(obj->paramAttrs);
    }
    break;
  }
  
  
  free(obj);
}

void freeScope(Scope* scope) {
  
  if (scope == NULL) return;
  
  if (scope->objList) freeObjectList(scope->objList);
  
  free(scope);
}

void freeObjectList(ObjectNode *objList) {
  
  ObjectNode *node = objList;
  ObjectNode *nextNode;
  
  while (node != NULL) {
    nextNode = node->next;         
    freeObject(node->object);      
    free(node);                    
    node = nextNode;                
  }
}

void freeReferenceList(ObjectNode *objList) {
  
  ObjectNode *node = objList;
  ObjectNode *nextNode;
  
  while (node != NULL) {
    nextNode = node->next;          

    free(node);                     
    node = nextNode;                
  }
}

void addObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  
  ObjectNode *node = objList;
  
  while (node != NULL) {
  
    if (strcasecmp(node->object->name, name) == 0) {
      return node->object;          
    }
    node = node->next;             
  }
  
  return NULL;                     
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  addObject(&(symtab->currentScope->objList), obj);
}


