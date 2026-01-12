/**
 * GIẢI THÍCH CHI TIẾT: compileAssignSt()
 *
 * Hàm này xử lý cú pháp gán (assignment statement) trong KPL:
 *   - Gán đơn: a := 10
 *   - Gán nhiều: a, b, c := 1, 2, 3
 *
 * ============================================================================
 * PHẦN 1: ĐỌC VÀ LƯU TẤT CẢ TÊN BIẾN BÊN TRÁI
 * ============================================================================
 */

// Token *savedTokens[50];      // Mảng lưu tên của tất cả biến bên trái
// int savedCount = 0;          // Số lượng biến bên trái

// Ví dụ: a, b, c := 1, 2, 3
//
// Bước 1: Lấy biến đầu tiên (a)
savedTokens[0] = makeToken(...);     // Tạo token mới
strcpy(savedTokens[0]->string, "a"); // Lưu tên "a"
savedCount = 1;

// Bước 2: Kiểm tra nếu có dấu phẩy, đọc biến tiếp theo
while (lookAhead->tokenType == SB_COMMA) // Nếu thấy dấu phẩy
{
    eat(SB_COMMA);                   // Bỏ qua dấu phẩy
    savedTokens[1] = makeToken(...); // Tạo token cho "b"
    strcpy(savedTokens[1]->string, "b");
    savedCount = 2;
    eat(TK_IDENT); // Bỏ qua tên biến
}

// Sau đó:
//   savedTokens[0]->string = "a"
//   savedTokens[1]->string = "b"
//   savedTokens[2]->string = "c"
//   savedCount = 3

// ============================================================================
// PHẦN 2: XÁC ĐỊNH LÀ GÁN ĐƠN HAY GÁN NHIỀU
// ============================================================================

// Nếu chỉ có 1 biến và không phải mảng → GÁN ĐƠN (đơn giản)
if (savedCount == 1 && !hasIndex)
{
    // Đây là gán đơn: a := 10

    // Compile bình thường như code cũ:
    var = checkDeclaredLValueIdent(savedTokens[0]->string); // a được khai báo?
    genVariableAddress(var);                                // Stack: [addr(a)]
    varType = var->varAttrs->type;                          // Lưu kiểu của a

    expType = compileExpression(); // Compile biểu thức: 10
                                   // Stack: [addr(a), 10]

    checkTypeEquality(varType, expType); // Kiểm tra kiểu (Integer == Integer?)
    genST();                             // Lệnh Store: *addr(a) = 10
                                         // Stack: [] (trống)
    return;                              // KẾT THÚC
}

// Nếu có > 1 biến → GÁN NHIỀU (phức tạp hơn)
// ============================================================================
// PHẦN 3: GÁN NHIỀU BIẾN - COMPILE XEN KẼ
// ============================================================================

// Ví dụ: a, b, c := 1, 2, 3
//
// Mục tiêu: Stack phải có cấu trúc
//   [addr(a), 1, addr(b), 2, addr(c), 3]
//
// Để khi genST() được gọi ngược (từ cuối về đầu):
//   genST() lần 1: pop(3), pop(addr(c)) → c = 3
//   genST() lần 2: pop(2), pop(addr(b)) → b = 2
//   genST() lần 3: pop(1), pop(addr(a)) → a = 1

// BƯỚC 1: Compile biến và biểu thức đầu tiên XEN KẼ
var = checkDeclaredLValueIdent(savedTokens[0]->string); // Kiểm tra "a"
genVariableAddress(var);                                // Stack: [addr(a)]
lvalueTypes[0] = var->varAttrs->type;

expTypes[0] = compileExpression(); // Stack: [addr(a), 1]
count = 1;

// BƯỚC 2: Compile các cặp (biến, biểu thức) còn lại XEN KẼ
for (i = 1; i < savedCount; i++)
{
    eat(SB_COMMA); // Bỏ qua dấu phẩy

    // Compile biến thứ i
    var = checkDeclaredLValueIdent(savedTokens[i]->string); // Kiểm tra "b"
    genVariableAddress(var);                                // Stack: [addr(a), 1, addr(b)]
    lvalueTypes[i] = var->varAttrs->type;

    // Compile biểu thức thứ i
    expTypes[i] = compileExpression(); // Stack: [addr(a), 1, addr(b), 2]
    count++;
}

// Kết thúc vòng lặp:
// Stack hiện tại: [addr(a), 1, addr(b), 2, addr(c), 3]
// count = 3

// BƯỚC 3: Kiểm tra kiểu dữ liệu
for (i = 0; i < count; i++)
{
    checkTypeEquality(lvalueTypes[i], expTypes[i]);
}
// Kiểm tra:
//   Integer (kiểu a) == Integer (kiểu 1)? ✓
//   Integer (kiểu b) == Integer (kiểu 2)? ✓
//   Integer (kiểu c) == Integer (kiểu 3)? ✓

// BƯỚC 4: SINH MÃ LỆNH ST THEO THỨ TỰ NGƯỢC
for (i = count - 1; i >= 0; i--)
{
    genST(); // Gọi genST() theo thứ tự: i=2, i=1, i=0
}

// Vòng lặp chạy:
//
// i = 2 (lần gọi thứ 1):
//   genST() lấy 2 phần tử trên cùng stack
//   Pop: 3 (value), Pop: addr(c)
//   Thực hiện: *addr(c) = 3
//   Stack: [addr(a), 1, addr(b), 2] → [addr(a), 1, addr(b)]
//
// i = 1 (lần gọi thứ 2):
//   genST() lấy 2 phần tử trên cùng stack
//   Pop: 2 (value), Pop: addr(b)
//   Thực hiện: *addr(b) = 2
//   Stack: [addr(a), 1] → [addr(a)]
//
// i = 0 (lần gọi thứ 3):
//   genST() lấy 2 phần tử trên cùng stack
//   Pop: 1 (value), Pop: addr(a)
//   Thực hiện: *addr(a) = 1
//   Stack: [] (trống)

// KẾT QUẢ CUỐI CÙNG: a = 1, b = 2, c = 3 ✓✓✓

// ============================================================================
// PHẦN 4: GIẢI PHÓNG BỘ NHỚ
// ============================================================================

for (i = 0; i < savedCount; i++)
    free(savedTokens[i]); // Giải phóng tất cả token đã lưu

// ============================================================================
// TÓM TẮT QUY TRÌNH
// ============================================================================

/*
 * GÁN ĐƠN: a := 10
 *   1. Lưu tên "a"
 *   2. Kiểm tra savedCount == 1 → GÁN ĐƠN
 *   3. Compile: addr(a), 10
 *   4. genST() 1 lần → a = 10
 *
 * GÁN NHIỀU: a, b, c := 1, 2, 3
 *   1. Lưu tên "a", "b", "c"
 *   2. Kiểm tra savedCount == 3 → GÁN NHIỀU
 *   3. Compile XEN KẼ:
 *      - addr(a), 1, addr(b), 2, addr(c), 3
 *   4. genST() 3 lần NGƯỢC:
 *      - c = 3, b = 2, a = 1
 */
