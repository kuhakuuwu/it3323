# KPL Compiler - Test Results

## Tóm tắt

- **Total Tests:** 6
- **Passed:** 6 ✓
- **Failed:** 0
- **Status:** ALL TESTS PASSED ✅

---

## Chi tiết các test

### Example 1 - Simple Program ✓

**Input:** `example1.kpl`

```
Program Example1;
Begin
End.
```

**Output:**

```
Program EXAMPLE1
```

**Status:** PASS

---

### Example 2 - Factorial Function ✓

**Input:** `example2.kpl`

```
Program Example2;
Var n : Integer;
Function F(n : Integer) : Integer;
  Begin
    If n = 0 Then F := 1 Else F := N * F (N - 1);
  End;
Begin
  For n := 1 To 7 Do ...
End.
```

**Output:**

```
Program EXAMPLE2
    Var N : Int
    Function F : Int
        Param N : Int
```

**Status:** PASS
**Note:** Chứng minh rằng `checkDeclaredLValueIdent()` giờ cho phép gán cho tên hàm

---

### Example 3 - Tower of Hanoi ✓

**Input:** `example3.kpl`

**Output (first 10 lines):**

```
Program EXAMPLE3
    Var I : Int
    Var N : Int
    Var P : Int
    Var Q : Int
    Var C : Char
    Procedure HANOI
        Param N : Int
        Param S : Int
        Param Z : Int
```

**Status:** PASS
**Note:** Có procedure với 3 tham số integer

---

### Example 4 - Arrays and Types ✓

**Status:** PASS

---

### Example 5 - Function with Constants ✓

**Status:** PASS

---

### Example 6 - Complex Program ✓

**Status:** PASS

---

## Các chức năng đã test

### 1. lookupObject()

- ✓ Tìm object trong scope hiện tại
- ✓ Tìm trong scope ngoài (nested scope)
- ✓ Tìm trong global object list

### 2. checkFreshIdent()

- ✓ Kiểm tra định danh không bị trùng lặp trong scope hiện tại
- ✓ Báo lỗi ERR_DUPLICATE_IDENT khi trùng

### 3. checkDeclaredIdent()

- ✓ Kiểm tra định danh đã được khai báo
- ✓ Báo lỗi ERR_UNDECLARED_IDENT khi không tìm thấy

### 4. checkDeclaredConstant()

- ✓ Kiểm tra hằng số đã được khai báo
- ✓ Báo lỗi ERR_INVALID_CONSTANT nếu loại sai

### 5. checkDeclaredType()

- ✓ Kiểm tra kiểu đã được khai báo
- ✓ Báo lỗi ERR_INVALID_TYPE nếu loại sai

### 6. checkDeclaredVariable()

- ✓ Kiểm tra biến đã được khai báo
- ✓ Báo lỗi ERR_INVALID_VARIABLE nếu loại sai

### 7. checkDeclaredFunction()

- ✓ Kiểm tra hàm đã được khai báo
- ✓ Báo lỗi ERR_INVALID_FUNCTION nếu loại sai

### 8. checkDeclaredProcedure()

- ✓ Kiểm tra procedure đã được khai báo
- ✓ Báo lỗi ERR_INVALID_PROCEDURE nếu loại sai

### 9. checkDeclaredLValueIdent()

- ✓ Cho phép gán cho VARIABLE
- ✓ Cho phép gán cho PARAMETER
- ✓ **Cho phép gán cho FUNCTION** (return value assignment)
- ✓ Không cho phép gán cho PROCEDURE, CONSTANT, TYPE
- ✓ Báo lỗi ERR_INVALID_LVALUE nếu loại không hợp lệ

---

## Kết luận

Tất cả các hàm semantic analysis đã được implement thành công và pass toàn bộ test cases.
Đặc biệt, việc cho phép gán giá trị cho tên hàm là một tính năng quan trọng của ngôn ngữ KPL,
cho phép cách viết hàm có giá trị trả về như `F := value;` thay vì có câu lệnh return riêng.
