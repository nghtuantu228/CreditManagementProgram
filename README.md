# CHƯƠNG TRÌNH QUẢN LÝ TÍN DỤNG

Đồ án Lập trình C — Năm 1 Đại học, ngành Kỹ thuật Phần mềm.
Toàn bộ chương trình nằm trong **một file duy nhất**: `credit.c`, chạy hoàn toàn trên terminal.

---

## 1. Cách biên dịch và chạy

### Visual Studio (máy này đã có sẵn Build Tools)
- Chạy `build.bat` → tạo ra `credit.exe`
- Hoặc mở `credit.c` bằng Visual Studio và bấm **F5**

### Dev-C++ / Code::Blocks
Tạo project, thêm `credit.c`, biên dịch (chuẩn C99, không cần thêm flag).

### Linux / macOS / WSL
```bash
gcc -o credit credit.c
./credit
```

---

## 2. Các chức năng

| STT | Chức năng | Hàm xử lý |
|-----|-----------|-----------|
| 1 | Tạo tài khoản mới (số TK, họ tên, PIN, loại TK, số dư ban đầu) | `chucNangTaoTaiKhoan()` |
| 2 | Đăng nhập vào tài khoản (số TK + mã PIN) | `chucNangDangNhap()`, `dangNhap()` |
| 3 | Chuyển tiền sang tài khoản khác | `chuyenTien()` |
| 4 | Kiểm tra số dư | `kiemTraSoDu()` |
| 5 | Sắp xếp các tài khoản theo thứ tự | `sapXepTaiKhoan()` |
| 6 | *(thêm)* Gửi / rút tiền | `guiTien()`, `rutTien()` |
| 7 | *(thêm)* Xem lịch sử giao dịch | `inLichSu()` |
| 8 | *(thêm)* Lưu / tải dữ liệu | `luuDuLieu()`, `taiDuLieu()` |

### Màn hình chính
```
==============================================================
  QUAN LY TIN DUNG
==============================================================
  Ngan hang so
==============================================================

  1. Tao tai khoan
  2. Dang nhap
  3. Xem danh sach tai khoan
  4. Sap xep tai khoan
  5. Luu du lieu
  6. Thoat
--------------------------------------------------------------
  Lua chon (1-6):
```

---

## 3. Kiến thức C được vận dụng

| Kiến thức | Dùng ở đâu |
|-----------|------------|
| **Rẽ nhánh** `if / else / switch` | Kiểm tra dữ liệu nhập trong `nhapSo()`, `nhapChuoi()`; `switch` cho menu và so sánh tiêu chí |
| **Lặp** `while / for` | `while` lặp lại khi nhập sai; `for` duyệt mảng; `while (1)` là menu chính |
| **Hàm & chương trình con** | Khoảng 20 hàm: `nhapSo()`, `nhapChuoi()`, `timTaiKhoan()`, `taoTaiKhoan()`, `guiTien()`, `chuyenTien()`, `sapXepTaiKhoan()`, `luuDuLieu()`… |
| **Mảng chuỗi ký tự** | `char soTK[20]`, `char tenTK[50]`, `char pin[6]` trong `struct TaiKhoan` |
| **Cấu trúc (struct)** | `struct TaiKhoan`, `struct GiaoDich`, `struct NganHang` |
| **Con trỏ** | `TaiKhoan *timTaiKhoan()` trả về **địa chỉ** phần tử trong mảng |
| **Mảng (array)** | `dsTK[50]`, `dsGD[100]` |

---

## 4. Cấu trúc dữ liệu

```c
typedef struct {
    char     soTK[MAX_ACCOUNT];  /* số tài khoản */
    char     tenTK[MAX_NAME];    /* họ tên chủ hộ */
    char     pin[MAX_PIN];       /* mã PIN */
    char     loaiTK[MAX_TYPE];   /* loại tài khoản */
    long long soDu;              /* số dư */
    int      trangThai;          /* 1 = hoạt động, 0 = khóa */
    int      soGD;               /* số giao dịch đã thực hiện */
} TaiKhoan;
```

---

## 5. Giải thích các đoạn code quan trọng

### a) Con trỏ trỏ tới phần tử trong mảng — `timTaiKhoan()`
```c
TaiKhoan *timTaiKhoan(const char *soTK)
{
    int i;
    for (i = 0; i < nganHang.soTK; i++) {
        if (strcmp(nganHang.dsTK[i].soTK, soTK) == 0) {
            return &nganHang.dsTK[i];   /* địa chỉ phần tử trong mảng */
        }
    }
    return NULL;
}
```
`&nganHang.dsTK[i]` lấy **địa chỉ** (con trỏ) của tài khoản thứ `i` trong mảng.
Trả về con trỏ này thì khi sửa số dư, dữ liệu ghi thẳng vào tài khoản thật — không tạo bản sao.

### b) Sắp xếp — Selection Sort, tách hàm so sánh riêng
```c
for (i = 0; i < nganHang.soTK - 1; i++) {
    viTriDacBiet = i;
    for (j = i + 1; j < nganHang.soTK; j++) {
        int ss = soSanhTK(&nganHang.dsTK[j], &nganHang.dsTK[viTriDacBiet], tieuChi);
        if ((thuTu > 0 && ss < 0) || (thuTu < 0 && ss > 0)) {
            viTriDacBiet = j;
        }
    }
    if (viTriDacBiet != i) {   /* đổi chỗ hai tài khoản */
        tam = nganHang.dsTK[i];
        nganHang.dsTK[i] = nganHang.dsTK[viTriDacBiet];
        nganHang.dsTK[viTriDacBiet] = tam;
    }
}
```
Mỗi vòng lặp tìm phần tử "đặc biệt" (nhỏ nhất khi tăng dần, lớn nhất khi giảm dần)
rồi đổi chỗ nó về đúng vị trí. Hàm `soSanhTK()` tách riêng nên muốn đổi tiêu chí
sắp xếp cũng không cần sửa vòng lặp.

### c) Kiểm tra dữ liệu nhập — vòng lặp `while` + `continue`
```c
while (1) {
    if (!nhapChuoi("  So TK: ", soTK, MAX_ACCOUNT)) return;
    if (!toanSo(soTK) || strlen(soTK) < 6) {
        printf("  [!] So TK phai co it nhat 6 chu so.\n");
        continue;          /* nhập sai → nhập lại */
    }
    if (timTaiKhoan(soTK) != NULL) {
        printf("  [!] So TK da ton tai. Chon so khac.\n");
        continue;
    }
    break;                 /* nhập đúng → thoát vòng lặp */
}
```

### d) Chuyển tiền — cập nhật đồng thời hai tài khoản
```c
tkNguon->soDu -= soTien;   /* trừ ở tài khoản gửi */
tkNhan->soDu  += soTien;   /* cộng ở tài khoản nhận */
ghiGiaoDich(tkNguon->soTK, GD_CHUYEN, soTien, tkNguon->soDu);
```

---

## 6. Kịch bản thuyết trình (khoảng 6 phút)

File `demo_input.txt` đã chứa sẵn các bước nhập. Chạy tự động:
```
credit.exe < demo_input.txt
```

**Các bước trình bày:**

1. **Mở đầu** — giới thiệu tên đồ án, các chức năng chính.
2. **Tạo tài khoản** — tạo 3 tài khoản:
   - `1000000001 / Nguyễn Văn A / Tiết kiệm`
   - `1000000002 / Trần Thị B / Thanh toán`
   - `1000000003 / Lê Văn C / Tín dụng`
   - *Điểm hay:* nhập sai PIN không khớp và nhập trùng số TK để thể hiện có kiểm tra.
3. **Đăng nhập** — đăng nhập `1000000001 / 1234`, sau đó thử sai PIN.
4. **Chức năng tiền** — gửi 2.000.000, rút 1.000.000, chuyển 1.000.000 sang `1000000002`.
5. **Kiểm tra số dư** — xem số dư cả 3 tài khoản để thấy tiền "đi đúng cho".
6. **Sắp xếp** — sắp xếp theo số dư giảm dần, rồi theo tên A–Z.
7. **Lịch sử giao dịch** — xem lịch sử của tài khoản vừa giao dịch.
8. **Lưu / tải** — lưu dữ liệu, thoát, mở lại chương trình để dữ liệu vẫn còn.

---

## 7. Lưu ý

- File `bankdata.txt` lưu dữ liệu. Chạy lại chương trình sẽ tự động tải lên.
- Xoá `bankdata.txt` để chạy lại từ đầu.
- Mã PIN trong bản demo: `1234` / `5678` / `1111`.
