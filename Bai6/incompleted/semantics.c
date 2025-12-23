/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab *symtab;
extern Token *currentToken;

/**
 * Tìm kiếm object trong bảng ký hiệu từ scope hiện tại trở ra ngoài
 * @param name: tên của object cần tìm
 * @return: object nếu tìm thấy, NULL nếu không tìm thấy
 *
 * Cách hoạt động:
 * - Bắt đầu từ scope hiện tại, tìm trong danh sách object
 * - Nếu không tìm thấy, lên scope ngoài (outer) và tiếp tục tìm
 * - Cho đến khi tìm thấy hoặc hết scope (ra ngoài cùng)
 */
Object *lookupObject(char *name)
{
  Scope *scope = symtab->currentScope;

  // Duyệt từ scope hiện tại trở ra ngoài
  while (scope != NULL)
  {
    // Tìm object trong danh sách object của scope này
    Object *obj = findObject(scope->objList, name);
    if (obj != NULL)
    {
      return obj; // Tìm thấy
    }
    scope = scope->outer; // Chuyển sang scope ngoài
  }

  // Nếu không tìm thấy ở scope, tìm trong danh sách global objects
  return findObject(symtab->globalObjectList, name);
}

/**
 * Kiểm tra định danh chưa được khai báo trong scope hiện tại
 * (Dùng khi khai báo object mới để tránh trùng lặp)
 * @param name: tên của định danh
 *
 * Nếu tìm thấy object cùng tên trong scope hiện tại -> báo lỗi ERR_DUPLICATE_IDENT
 */
void checkFreshIdent(char *name)
{
  // Chỉ kiểm tra trong scope hiện tại, không tìm ngoài
  if (findObject(symtab->currentScope->objList, name) != NULL)
  {
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
  }
}

/**
 * Kiểm tra định danh đã được khai báo
 * (Dùng khi sử dụng identifier - phải được khai báo trước đó)
 * @param name: tên của định danh
 * @return: object nếu được khai báo, gọi error nếu không
 */
Object *checkDeclaredIdent(char *name)
{
  Object *obj = lookupObject(name);

  if (obj == NULL)
  {
    // Không tìm thấy -> báo lỗi
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}

/**
 * Kiểm tra hằng số đã được khai báo
 * @param name: tên của hằng số
 * @return: object hằng số nếu tìm thấy, gọi error nếu không hoặc loại sai
 */
Object *checkDeclaredConstant(char *name)
{
  Object *obj = lookupObject(name);

  if (obj == NULL)
  {
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
  }
  else if (obj->kind != OBJ_CONSTANT)
  {
    // Tìm thấy nhưng không phải hằng số
    error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}

/**
 * Kiểm tra kiểu đã được khai báo
 * @param name: tên của kiểu dữ liệu
 * @return: object kiểu nếu tìm thấy, gọi error nếu không hoặc loại sai
 */
Object *checkDeclaredType(char *name)
{
  Object *obj = lookupObject(name);

  if (obj == NULL)
  {
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
  }
  else if (obj->kind != OBJ_TYPE)
  {
    // Tìm thấy nhưng không phải kiểu dữ liệu
    error(ERR_INVALID_TYPE, currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}

/**
 * Kiểm tra biến đã được khai báo
 * @param name: tên của biến
 * @return: object biến nếu tìm thấy, gọi error nếu không hoặc loại sai
 */
Object *checkDeclaredVariable(char *name)
{
  Object *obj = lookupObject(name);

  if (obj == NULL)
  {
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  }
  else if (obj->kind != OBJ_VARIABLE)
  {
    // Tìm thấy nhưng không phải biến
    error(ERR_INVALID_VARIABLE, currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}

/**
 * Kiểm tra hàm đã được khai báo
 * @param name: tên của hàm
 * @return: object hàm nếu tìm thấy, gọi error nếu không hoặc loại sai
 */
Object *checkDeclaredFunction(char *name)
{
  Object *obj = lookupObject(name);

  if (obj == NULL)
  {
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
  }
  else if (obj->kind != OBJ_FUNCTION)
  {
    // Tìm thấy nhưng không phải hàm
    error(ERR_INVALID_FUNCTION, currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}

/**
 * Kiểm tra thủ tục (procedure) đã được khai báo
 * @param name: tên của thủ tục
 * @return: object thủ tục nếu tìm thấy, gọi error nếu không hoặc loại sai
 */
Object *checkDeclaredProcedure(char *name)
{
  Object *obj = lookupObject(name);

  if (obj == NULL)
  {
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  }
  else if (obj->kind != OBJ_PROCEDURE)
  {
    // Tìm thấy nhưng không phải thủ tục
    error(ERR_INVALID_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}

/**
 * Kiểm tra identifier ở vế trái của lệnh gán (LValue)
 * Phải là biến, tham số, hoặc tên hàm (không được là thủ tục, hằng số, kiểu)
 * @param name: tên của identifier
 * @return: object nếu hợp lệ, gọi error nếu không hoặc loại sai
 *
 * Ghi chú: Trong KPL, có thể gán giá trị cho tên hàm để coi đó là giá trị trả về
 * Ví dụ: Function F : Integer;
 *        Begin
 *          F := 5;  (* Gán giá trị trả về *)
 *        End;
 */
Object *checkDeclaredLValueIdent(char *name)
{
  Object *obj = lookupObject(name);

  if (obj == NULL)
  {
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  }
  else if ((obj->kind != OBJ_VARIABLE) && (obj->kind != OBJ_PARAMETER) && (obj->kind != OBJ_FUNCTION))
  {
    // Cho phép: biến, tham số, hoặc tên hàm
    // Không cho phép: thủ tục, hằng số, kiểu
    error(ERR_INVALID_LVALUE, currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}
