# CHUONG TRINH QUAN LY TIN DUNG

Do an Lap trinh C - Nam 1 Dai hoc, nganh Ky thuat Phan mem.
Toan bo chuong trinh nam trong **mot file duy nhất**: `credit.c`, chay hoan toan tren terminal.

---

## 1. Cach bien dich va chay

### Cach 1: Visual Studio (may nay da co san Build Tools)
Chay `build.bat` (se bien dich ra `credit.exe`), hoac mo `credit.c` bang Visual Studio va bam F5.

### Cach 2: Dev-C++ / Code::Blocks
Tao project, them `credit.c`, bien dich (bien dich chuan C99, khong can flag gi them).

### Cach 3: Linux / macOS / WSL
```
gcc -o credit credit.c
./credit
```

---

## 2. Chuc nang ( dung yeu cau de tai )

| STT | Chuc nang | Vi tri trong code |
|-----|-----------|-------------------|
| 1 | Tao tai khoan moi (so TK, ten, PIN, loai TK, so du ban dau) | `chucNangTaoTaiKhoan()` |
| 2 | Dang nhap vao tai khoan (so TK + ma PIN) | `chucNangDangNhap()`, `dangNhap()` |
| 3 | Chuyen tien vao so tai khoan khac | `chuyenTien()` |
| 4 | Kiem tra so du | `kiemTraSoDu()` |
| 5 | Sap xep cac tai khoan theo thu tu | `sapXepTaiKhoan()` |
| (them) | Gui / rut tien, xem lich su giao dich, luu du lieu | `guiTien()`, `rutTien()`, `inLichSu()`, `luuDuLieu()` |

### Man hinh chinh
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

## 3. Kien thuc C duoc su dung

| Kien thuc | Dung o dau |
|-----------|------------|
| **Cau truc dieu khien rẽ nhánh** | `if / else if / else` trong `nhapSo()`, `nhapChuoi()` kiem tra du lieu nhap; `switch` trong `menuTaiKhoan()`, `main()`, `soSanhTK()` |
| **Cau truc dieu khien lap** | `while` trong `nhapSo()`, `nhapChuoi()`, `sapXepTaiKhoan()`; `for` trong `inDanhSachTK()`, `inLichSu()`, `luuDuLieu()`; `while (1)` la menu chinh |
| **Ham va chuong trinh con** | ~20 ham: `xoaManHinh()`, `nhapSo()`, `nhapChuoi()`, `timTaiKhoan()`, `taoTaiKhoan()`, `dangNhap()`, `guiTien()`, `rutTien()`, `chuyenTien()`, `sapXepTaiKhoan()`, `luuDuLieu()`, `taiDuLieu()`... |
| **Mang chuoi ky tu** | `char soTK[MAX_ACCOUNT]`, `char tenTK[MAX_NAME]`, `char pin[MAX_PIN]` trong `struct TaiKhoan` |
| **Cau truc (struct)** | `struct TaiKhoan`, `struct GiaoDich`, `struct NganHang` |
| **Con tro (*)** | `TaiKhoan *timTaiKhoan()` tra ve dia chi phan tu trong mang; con tro dung de ghi vao tai khoan cua nguoi dung dang dang nhap |
| **Mang (array)** | `dsTK[MAX_ACC]`, `dsGD[MAX_TX]` |

---

## 4. Cau truc du lieu

```c
typedef struct {
    char     soTK[MAX_ACCOUNT];  /* so tai khoan */
    char     tenTK[MAX_NAME];    /* ho ten chu ho */
    char     pin[MAX_PIN];       /* ma PIN */
    char     loaiTK[MAX_TYPE];   /* loai tai khoan */
    long long soDu;              /* so du tai khoan */
    int      trangThai;          /* 1 = dang hoat dong, 0 = bi khoa */
    int      soGD;               /* so giao dich da thuc hien */
} TaiKhoan;
```

---

## 5. Kien thuc dung trong code

### a) Con tro tro toi phan tu trong mang - `timTaiKhoan()`
```c
TaiKhoan *timTaiKhoan(const char *soTK)
{
    int i;
    for (i = 0; i < nganHang.soTK; i++) {
        if (strcmp(nganHang.dsTK[i].soTK, soTK) == 0) {
            return &nganHang.dsTK[i];   /* dia chi phan tu trong mang */
        }
    }
    return NULL;
}
```
`&nganHang.dsTK[i]` lay **dia chi** (con tro) cua tai khoan thu i trong mang. Tra ve con do
thi moi sua doi so du se ghi thang vao du lieu that, khong tao ban sao.

### b) Sap xep - Selection Sort voi ham so sanh tach rieng
```c
for (i = 0; i < nganHang.soTK - 1; i++) {
    viTriDacBiet = i;
    for (j = i + 1; j < nganHang.soTK; j++) {
        int ss = soSanhTK(&nganHang.dsTK[j], &nganHang.dsTK[viTriDacBiet], tieuChi);
        if ((thuTu > 0 && ss < 0) || (thuTu < 0 && ss > 0)) {
            viTriDacBiet = j;
        }
    }
    if (viTriDacBiet != i) {   /* doi cho 2 tai khoan */
        tam = nganHang.dsTK[i];
        nganHang.dsTK[i] = nganHang.dsTK[viTriDacBiet];
        nganHang.dsTK[viTriDacBiet] = tam;
    }
}
```
Mot vong lap tim phan tu "dac biet" (nho nhat khi tang dan, lon nhat khi giam dan) roi doi
cho no ve dung vi tri. `soSanhTK()` tach rieng ra de co the doi tieu chi ma khong sua
vong lap.

### c) Kiem tra du lieu nhap - vong lap `while` + `continue`
```c
while (1) {
    if (!nhapChuoi("  Nhap so tai khoan: ", soTK, MAX_ACCOUNT)) return;
    if (!toanSo(soTK) || strlen(soTK) < 6) {
        printf("  [!] So tai khoan phai gom it nhat 6 chu so.\n");
        continue;          /* nhap sai -> nhap lai */
    }
    if (timTaiKhoan(soTK) != NULL) {
        printf("  [!] So tai khoan nay da ton tai.\n");
        continue;
    }
    break;                 /* nhap dung -> thoat vong lap */
}
```

### d) Chuyen tien - cap nhat dong thoi hai tai khoan
```c
tkNguon->soDu -= soTien;   /* tru o tai khoan gui */
tkNhan->soDu  += soTien;   /* cong o tai khoan nhan */
ghiGiaoDich(tkNguon->soTK, GD_CHUYEN, soTien, tkNguon->soDu);
```

---

## 6. Kich ban thuyet trinh (demo)

File `demo_input.txt` chua san cac buoc nhap. Neu chay tu dong:

```
credit.exe < demo_input.txt
```

**Kich ban thuyet trinh de xuat (6 phut):**

1. **Mo dau** - gioi thieu ten do an, chuc nang chinh.
2. **Tao tai khoan** - tao 3 tai khoan: `1000000001 / Nguyen Van A / Tiet kiem`,
   `1000000002 / Tran Thi B / Thanh toan`, `1000000003 / Le Van C / Tin dung`.
   *Diem hay:* nhap sai PIN khong khop va nhap trung so TK de hien thi co kiem tra.
3. **Dang nhap** - dang nhap `1000000001 / 1234`, sau do thu sai PIN.
4. **Chuc nang tien** - gui 2.000.000, rut 1.000.000, chuyen 1.000.000 sang `1000000002`.
5. **Kiem tra so du** - xem so du 3 tai khoan de thay so tien "di dung cho".
6. **Sap xep** - sap xep theo so du giam dan, theo ten A-Z.
7. **Lich su giao dich** - xem lich su cua tai khoan vua giao dich.
8. **Luu/load** - luu du lieu, thoat, mo lai chuong trinh de du lieu van con.

---

## 7. Luu y

- File `bankdata.txt` luu du lieu. Chay lai chuong trinh se tu dong tai len.
- Xoa `bankdata.txt` de chay lai tu dau.
- Ma PIN trong ban demo la `1234` / `5678` / `1111`.
