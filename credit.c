/*
 * =====================================================================
 * CHUONG TRINH QUAN LY TIN DUNG
 * Lap trinh C - Nam 1 Dai hoc, nganh Ky thuat Phan mem
 * Chay hoan toan tren terminal (console).
 *
 * YEU CAO:
 *   + Tao tai khoan (ten TK, so TK, ma PIN, ...)
 *   + Dang nhap vao tai khoan
 *   + Chuyen tien vao so tai khoan khac
 *   + Kiem tra so du
 *   + Sap xep cac tai khoan theo thu tu
 *
 * Kien thuc dung:
 *   - Cau truc dieu khien re nhanh (if/else, switch) va lap (while, for)
 *   - Ham va chuong trinh con
 *   - Mang chuoi ky tu (char[])
 *   - Cau truc (struct)
 *   - Con tro (*)
 * =====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* ======================== 1. HANG SO VA CAU TRUC ======================== */

/* Do dai toi da cho cac chuoi (mang chuoi ky tu) */
#define MAX_NAME    50   /* do dai ho ten chu ho */
#define MAX_ACCOUNT 20   /* do dai so tai khoan */
#define MAX_PIN      6   /* do dai ma PIN */
#define MAX_TYPE    20   /* do dai loai tai khoan */
#define MAX_AMOUNT  30   /* do dai chuoi hien thi so tien */

/* So tai khoan toi da cho phep */
#define MAX_ACC  50

/* So giao dich toi da cho phep */
#define MAX_TX  100

/* Loai tai khoan */
#define TK_TIETKIEM  "Tiet kiem"
#define TK_THANHTOAN "Thanh toan"
#define TK_TINDUNG   "Tin dung"

/* Kieu giao dich */
#define GD_GUI        "GUI TIEN"
#define GD_RUT        "RUT TIEN"
#define GD_CHUYEN     "CHUYEN TIEN"

/*
 * CAU TRUC (struct) cua mot tai khoan ngan hang.
 * Dung mang ky tu char[] de luu chuoi -> khong can cap phat bo dem.
 */
typedef struct {
    char     soTK[MAX_ACCOUNT];  /* so tai khoan */
    char     tenTK[MAX_NAME];    /* ho ten chu ho */
    char     pin[MAX_PIN];       /* ma PIN */
    char     loaiTK[MAX_TYPE];   /* loai tai khoan */
    long long soDu;              /* so du tai khoan */
    int      trangThai;          /* 1 = dang hoat dong, 0 = bi khoa */
    int      soGD;               /* so giao dich da thuc hien */
} TaiKhoan;

/*
 * CAU TRUC cua mot giao dich (de in lich su).
 */
typedef struct {
    char     soTK[MAX_ACCOUNT];  /* tai khoan thuc hien giao dich */
    char     loai[20];           /* loai giao dich */
    long long soTien;            /* so tien giao dich */
    long long sauGD;             /* so du sau khi giao dich */
    char     thoiGian[20];       /* thoi diem giao dich */
} GiaoDich;

/*
 * CAU TRUC chua toan bo du lieu ngan hang.
 */
typedef struct {
    TaiKhoan  dsTK[MAX_ACC];    /* mang cac tai khoan */
    int       soTK;              /* so tai khoan hien co */
    GiaoDich  dsGD[MAX_TX];      /* mang cac giao dich */
    int       soGD;              /* so giao dich hien co */
} NganHang;

/* Bien toan cuc dung de luu du lieu ngan hang */
NganHang nganHang;

/* ======================== 2. HAM TIEN ICH ======================== */

/*
 * Ham xoa man hinh.
 * KHONG dung system("cls") vi lenh nay khoi dong cmd.exe va lam "an" mat
 * du lieu dang nhap tu ban phim. Dung API cua Windows de xoa truc tiep.
 */
void xoaManHinh(void)
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &mode)) {
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        printf("\033[2J\033[H");     /* xoa man hinh + dua con tro ve dau */
        fflush(stdout);
        return;
    }
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

/* Ham in mot duong ke gach ngang */
void inGach(char c, int n)
{
    int i;
    for (i = 0; i < n; i++) putchar(c);
    putchar('\n');
}

/* Ham in tieu de lon */
void inTieuDe(const char *tieuDe)
{
    inGach('=', 62);
    printf("  %s\n", tieuDe);
    inGach('=', 62);
}

/* Ham in tieu de nho */
void inTieuDeNho(const char *tieuDe)
{
    inGach('-', 62);
    printf("  %s\n", tieuDe);
    inGach('-', 62);
}

/* Ham tam dung cho nguoi dung bam ENTER */
void tamDung(void)
{
    char tmp[10];
    printf("\nNhan ENTER de tiep tuc...");    fflush(stdout);
    if (fgets(tmp, sizeof(tmp), stdin) == NULL) exit(0);
}

/* Ham bat UTF-8 tren Windows de hien thi tieng Viet co dau */
void batTiengViet(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
}

/*
 * Ham dinh dang tien: 1234567 -> "1,234,567".
 * Vong lap them dau phay cach nhom 3 chu so.
 */
void dinhDangTien(long long soTien, char *ketQua, int doDai)
{
    char tmp[30];      /* chuoi tam chua so nguyen */
    char ketQuaTam[40];
    int doDaiSo, i, dem = 0, viTri = 0;

    if (ketQua == NULL || doDai <= 0) return;

    sprintf(tmp, "%lld", (soTien < 0) ? -soTien : soTien);
    doDaiSo = (int)strlen(tmp);

    for (i = 0; i < doDaiSo; i++) {
        ketQuaTam[viTri++] = tmp[i];
        dem++;
        /* Con it nhat 1 chu so phia sau va het nhom 3 -> them dau ',' */
        if (dem < doDaiSo && (doDaiSo - i - 1) % 3 == 0) {
            ketQuaTam[viTri++] = ',';
        }
    }
    ketQuaTam[viTri] = '\0';

    if (soTien < 0) {
        sprintf(ketQua, "-%s", ketQuaTam);
    } else {
        sprintf(ketQua, "%s", ketQuaTam);
    }
}

/*
 * Ham sao chep chuoi co gioi han do dai (tranh tran bo dem).
 * Dung con tro de "di" qua tung ky tu.
 */
void saoChepChuoi(char *dich, int doDaiDich, const char *nguon)
{
    int i = 0;
    if (dich == NULL || doDaiDich <= 0) return;
    if (nguon == NULL) { dich[0] = '\0'; return; }

    while (nguon[i] != '\0' && i < doDaiDich - 1) {
        dich[i] = nguon[i];
        i++;
    }
    dich[i] = '\0';
}

/*
 * Ham doc 1 dong tu ban phim va bo ky tu xuong dong.
 * Tra ve 1 neu doc thanh cong, 0 neu ket thuc tep (EOF).
 */
int docDong(char *chuoi, int doDai)
{
    int c;
    if (fgets(chuoi, doDai, stdin) == NULL) return 0;

    /* Bo ky tu xuong dong cuoi chuoi */
    int len = (int)strlen(chuoi);
    if (len > 0 && chuoi[len - 1] == '\n') {
        chuoi[len - 1] = '\0';
    } else {
        /* Dong qua dai -> nap het phan con lai trong bo dem */
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    return 1;
}

/* Ham xoa khoang trang o dau va cuoi chuoi */
void catKhoangTrang(char *chuoi)
{
    int i = 0, len;
    if (chuoi == NULL) return;

    /* Bua dau: tim vi tri dau tien khong phai khoang trang */
    while (chuoi[i] != '\0' && isspace((unsigned char)chuoi[i])) i++;
    if (i > 0) memmove(chuoi, chuoi + i, strlen(chuoi) - i + 1);

    /* Bua cuoi */
    len = (int)strlen(chuoi);
    while (len > 0 && isspace((unsigned char)chuoi[len - 1])) {
        chuoi[--len] = '\0';
    }
}

/* Ham kiem tra chuoi co rong hay khong */
int rong(const char *chuoi)
{
    int i;
    if (chuoi == NULL) return 1;
    for (i = 0; chuoi[i] != '\0'; i++) {
        if (!isspace((unsigned char)chuoi[i])) return 0;
    }
    return 1;
}

/* Ham kiem tra chuoi co toan chu so hay khong */
int toanSo(const char *chuoi)
{
    int i;
    if (chuoi == NULL || chuoi[0] == '\0') return 0;
    for (i = 0; chuoi[i] != '\0'; i++) {
        if (!isdigit((unsigned char)chuoi[i])) return 0;
    }
    return 1;
}

/*
 * Ham nhap mot chuoi (khong cho nhap rong).
 * Tra ve 1 neu thanh cong, 0 neu EOF.
 */
int nhapChuoi(const char *cauHoi, char *ketQua, int doDai)
{
    char tmp[200];

    while (1) {
        printf("%s", cauHoi);
        fflush(stdout);
        if (!docDong(tmp, sizeof(tmp))) return 0;
        catKhoangTrang(tmp);
        if (tmp[0] == '\0') {
            printf("  [!] Khong duoc rong.\n");
            continue;
        }
        saoChepChuoi(ketQua, doDai, tmp);
        return 1;
    }
}

/*
 * Ham nhap so nguyen co kiem tra khoang gia tri.
 * strtol chuyen chuoi thanh so, con tro endPtr tro toi ky tu thua.
 */
int nhapSo(const char *cauHoi, int min, int max)
{
    char tmp[60];
    char *endPtr;
    long giaTri;

    while (1) {
        printf("%s", cauHoi);
        fflush(stdout);
        if (!docDong(tmp, sizeof(tmp))) return min;

        catKhoangTrang(tmp);
        if (tmp[0] == '\0') {
            printf("  [!] Phai la so.\n");
            continue;
        }
        giaTri = strtol(tmp, &endPtr, 10);
        if (*endPtr != '\0') {
            printf("  [!] Chi nhap so.\n");
            continue;
        }
        if (giaTri < min || giaTri > max) {
            printf("  [!] Nhap trong khoang [%d, %d].\n", min, max);
            continue;
        }
        return (int)giaTri;
    }
}

/* Ham nhap so tien (long long) co kiem tra khoang gia tri */
long long nhapTien(const char *cauHoi, long long min, long long max)
{
    char tmp[60];
    char *endPtr;
    long long giaTri;

    while (1) {
        printf("%s", cauHoi);
        fflush(stdout);
        if (!docDong(tmp, sizeof(tmp))) return min;

        catKhoangTrang(tmp);
        if (tmp[0] == '\0') {
            printf("  [!] Phai la so tien.\n");
            continue;
        }
        giaTri = strtoll(tmp, &endPtr, 10);
        if (*endPtr != '\0') {
            printf("  [!] Chi nhap so.\n");
            continue;
        }
        if (giaTri < min || giaTri > max) {
            printf("  [!] Nhap trong khoang [%lld, %lld].\n", min, max);
            continue;
        }
        return giaTri;
    }
}

/* ======================== 3. HAM XU LY TAI KHOAN ======================== */

/*
 * Ham khoi tao he thong rong.
 * Con tro nhanh toi bien toan cuc nganHang.
 */
void khoiTaoHeThong(void)
{
    memset(&nganHang, 0, sizeof(NganHang));
    nganHang.soTK = 0;
    nganHang.soGD = 0;
}

/*
 * Ham tim tai khoan theo so tai khoan.
 * Tra ve CON TRO tro toi tai khoan trong mang, NULL neu khong tim thay.
 * Con po giu nguyen "dia chi" cua tai khoan -> moi thay doi no ghi vao du lieu that.
 */
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

/*
 * Ham tao tai khoan moi.
 * Tra ve 1 neu thanh cong, 0 neu that bai.
 */
int taoTaiKhoan(const char *soTK, const char *tenTK, const char *pin,
                const char *loaiTK, long long soDuBanDau)
{
    TaiKhoan *tk;   /* con tro tro toi tai khoan moi them vao cuoi mang */

    /* Kiem tra so tai khoan da co chua */
    if (timTaiKhoan(soTK) != NULL) {
        printf("  [!] So TK %s da ton tai.\n", soTK);
        return 0;
    }

    /* Kiem tra mang da day chua */
    if (nganHang.soTK >= MAX_ACC) {
        printf("  [!] Chi chua toi da %d tai khoan.\n", MAX_ACC);
        return 0;
    }

    /* Con tro tro toi phan tu cuoi cung cua mang */
    tk = &nganHang.dsTK[nganHang.soTK];

    /* Ghi du lieu vao tai khoan moi */
    saoChepChuoi(tk->soTK, MAX_ACCOUNT, soTK);
    saoChepChuoi(tk->tenTK, MAX_NAME, tenTK);
    saoChepChuoi(tk->pin, MAX_PIN, pin);
    saoChepChuoi(tk->loaiTK, MAX_TYPE, loaiTK);
    tk->soDu = soDuBanDau;
    tk->trangThai = 1;
    tk->soGD = 0;

    /* Tang so luong tai khoan len 1 */
    nganHang.soTK++;

    printf("  [+] Tao tai khoan thanh cong!\n");
    printf("      So TK  : %s\n", tk->soTK);
    printf("      Chu ho : %s\n", tk->tenTK);
    printf("      Loai  : %s\n", tk->loaiTK);
    return 1;
}

/*
 * Ham dang nhap vao tai khoan.
 * Kiem tra so TK va ma PIN. Neu dung tra ve con tro toi tai khoan.
 */
TaiKhoan *dangNhap(const char *soTK, const char *pin)
{
    TaiKhoan *tk = timTaiKhoan(soTK);

    if (tk == NULL) {
        printf("  [!] Khong co tai khoan %s.\n", soTK);
        return NULL;
    }
    if (tk->trangThai == 0) {
        printf("  [!] Tai khoan %s dang bi khoa.\n", soTK);
        return NULL;
    }
    if (strcmp(tk->pin, pin) != 0) {
        printf("  [!] Sai ma PIN.\n");
        return NULL;
    }
    return tk;
}

/*
 * Ham ghi 1 giao dich vao lich su.
 * Con tro tro toi phan tu cuoi cung cua mang giao dich.
 */
void ghiGiaoDich(const char *soTK, const char *loai, long long soTien, long long sauGD)
{
    GiaoDich *gd = &nganHang.dsGD[nganHang.soGD];
    time_t bayGio = time(NULL);
    struct tm *gt = localtime(&bayGio);

    saoChepChuoi(gd->soTK, MAX_ACCOUNT, soTK);
    saoChepChuoi(gd->loai, 20, loai);
    gd->soTien = soTien;
    gd->sauGD = sauGD;

    if (gt != NULL) {
        strftime(gd->thoiGian, sizeof(gd->thoiGian), "%d/%m/%Y %H:%M:%S", gt);
    } else {
        saoChepChuoi(gd->thoiGian, 20, "--/--/---- --:--:--");
    }

    nganHang.soGD++;
}

/* Ham kiem tra so du cua tai khoan */
void kiemTraSoDu(const TaiKhoan *tk)
{
    char tien[MAX_AMOUNT];

    if (tk == NULL) return;

    dinhDangTien(tk->soDu, tien, MAX_AMOUNT);
    printf("\n  === SO DU ===\n");
    printf("  So TK   : %s\n", tk->soTK);
    printf("  Chu ho  : %s\n", tk->tenTK);
    printf("  Loai    : %s\n", tk->loaiTK);
    printf("  So du   : %s VND\n", tien);
}

/* Ham gui tien vao tai khoan */
int guiTien(TaiKhoan *tk, long long soTien)
{
    char tien[MAX_AMOUNT];

    if (tk == NULL) return 0;
    if (soTien <= 0) {
        printf("  [!] So tien phai lon hon 0.\n");
        return 0;
    }
    if (tk->soDu + soTien > 999000000000LL) {
        printf("  [!] Vuot gioi han so du.\n");
        return 0;
    }

    /* Cong them so tien vao so du */
    tk->soDu += soTien;
    tk->soGD++;
    ghiGiaoDich(tk->soTK, GD_GUI, soTien, tk->soDu);

    dinhDangTien(soTien, tien, MAX_AMOUNT);
    printf("  [+] GUI TIEN THANH CONG: %s VND\n", tien);
    dinhDangTien(tk->soDu, tien, MAX_AMOUNT);
    printf("      So du moi: %s VND\n", tien);
    return 1;
}

/* Ham rut tien tu tai khoan */
int rutTien(TaiKhoan *tk, long long soTien)
{
    char tien[MAX_AMOUNT];

    if (tk == NULL) return 0;
    if (soTien <= 0) {
        printf("  [!] So tien phai lon hon 0.\n");
        return 0;
    }
    if (soTien > tk->soDu) {
        dinhDangTien(tk->soDu, tien, MAX_AMOUNT);
        printf("  [!] So du khong du. Hien co: %s VND\n", tien);
        return 0;
    }

    /* Tru so tien khoi so du */
    tk->soDu -= soTien;
    tk->soGD++;
    ghiGiaoDich(tk->soTK, GD_RUT, soTien, tk->soDu);

    dinhDangTien(soTien, tien, MAX_AMOUNT);
    printf("  [+] RUT TIEN THANH CONG: %s VND\n", tien);
    dinhDangTien(tk->soDu, tien, MAX_AMOUNT);
    printf("      So du moi: %s VND\n", tien);
    return 1;
}

/*
 * Ham chuyen tien sang tai khoan khac.
 * Con tro tkNguon tro tai khoan gui, con tro tkNhan tro tai khoan nhan.
 */
int chuyenTien(TaiKhoan *tkNguon, const char *soTKNhan, long long soTien)
{
    TaiKhoan *tkNhan;
    char tien[MAX_AMOUNT];

    if (tkNguon == NULL) return 0;

    /* Khong chuyen cho chinh minh */
    if (strcmp(tkNguon->soTK, soTKNhan) == 0) {
        printf("  [!] Khong chuyen cho chinh minh.\n");
        return 0;
    }

    /* Tim tai khoan nhan */
    tkNhan = timTaiKhoan(soTKNhan);
    if (tkNhan == NULL) {
        printf("  [!] Tai khoan nhan khong co: %s\n", soTKNhan);
        return 0;
    }
    if (tkNhan->trangThai == 0) {
        printf("  [!] Tai khoan nhan dang bi khoa.\n");
        return 0;
    }
    if (soTien <= 0) {
        printf("  [!] So tien phai lon hon 0.\n");
        return 0;
    }
    if (soTien > tkNguon->soDu) {
        dinhDangTien(tkNguon->soDu, tien, MAX_AMOUNT);
        printf("  [!] So du khong du. Hien co: %s VND\n", tien);
        return 0;
    }

    /* Cap nhat hai tai khoan */
    tkNguon->soDu -= soTien;
    tkNhan->soDu += soTien;
    tkNguon->soGD++;
    tkNhan->soGD++;

    ghiGiaoDich(tkNguon->soTK, GD_CHUYEN, soTien, tkNguon->soDu);

    dinhDangTien(soTien, tien, MAX_AMOUNT);
    printf("  [+] CHUYEN TIEN THANH CONG: %s VND\n", tien);
    printf("      Tu  : %s (%s)\n", tkNguon->soTK, tkNguon->tenTK);
    printf("      Den : %s (%s)\n", tkNhan->soTK, tkNhan->tenTK);    return 1;
}

/* Ham in lich su giao dich cua mot tai khoan */
void inLichSu(const char *soTK)
{
    int i, timThay = 0;
    char tien[MAX_AMOUNT];

    inTieuDeNho("LICH SU GIAO DICH");
    printf("  %-4s %-20s %-16s %14s %16s\n",
           "#", "THOI GIAN", "LOAI", "SO TIEN", "SO DU SAU");    inGach('-', 76);

    for (i = 0; i < nganHang.soGD; i++) {
        if (strcmp(nganHang.dsGD[i].soTK, soTK) != 0) continue;

        timThay = 1;
        dinhDangTien(nganHang.dsGD[i].soTien, tien, MAX_AMOUNT);
        printf("  %-4d %-20s %-16s %14s",
               i + 1, nganHang.dsGD[i].thoiGian, nganHang.dsGD[i].loai, tien);
        dinhDangTien(nganHang.dsGD[i].sauGD, tien, MAX_AMOUNT);
        printf(" %16s\n", tien);
    }

    if (timThay == 0) printf("  (Chua co giao dich.)\n");
    inGach('-', 76);
}

/* Ham in danh sach tat ca tai khoan */
void inDanhSachTK(void)
{
    int i;
    char tien[MAX_AMOUNT];

    inTieuDeNho("DANH SACH TAI KHOAN");
    if (nganHang.soTK == 0) {
        printf("  (Chua co tai khoan nao.)\n");
        return;
    }

    printf("  %-4s %-14s %-22s %-14s %14s  %s\n",
           "#", "SO TK", "CHU HO", "LOAI", "SO DU", "TRANG THAI");
    inGach('-', 88);

    for (i = 0; i < nganHang.soTK; i++) {
        dinhDangTien(nganHang.dsTK[i].soDu, tien, MAX_AMOUNT);
        printf("  %-4d %-14s %-22s %-14s %14s  %s\n",               i + 1,
               nganHang.dsTK[i].soTK,
               nganHang.dsTK[i].tenTK,
               nganHang.dsTK[i].loaiTK,
               tien,
               nganHang.dsTK[i].trangThai ? "Hoat dong" : "Da khoa");
    }
    inGach('-', 88);
    printf("  Tong: %d tai khoan\n", nganHang.soTK);
}

/* ======================== 4. HAM SAP XEP ======================== */

/* Cac tieu chi sap xep */
#define SAP_XEP_SO_TK   1   /* theo so tai khoan */
#define SAP_XEP_TEN     2   /* theo ten chu ho */
#define SAP_XEP_SO_DU   3   /* theo so du */
#define SAP_XEP_LOAI    4   /* theo loai tai khoan */

/*
 * Ham so sanh 2 tai khoan theo tieu chi.
 * Tra ve: <0 neu a < b, 0 neu bang nhau, >0 neu a > b.
 * Tach rieng de "Selection Sort" su dung lai.
 */
int soSanhTK(const TaiKhoan *a, const TaiKhoan *b, int tieuChi)
{
    switch (tieuChi) {
        case SAP_XEP_TEN:                     /* so sanh theo ten (chuoi) */
            return strcmp(a->tenTK, b->tenTK);

        case SAP_XEP_SO_DU:                   /* so sanh theo so du (so) */
            if (a->soDu < b->soDu) return -1;
            if (a->soDu > b->soDu) return 1;
            return 0;

        case SAP_XEP_LOAI:                    /* theo loai, neu trung thi theo so TK */
            if (strcmp(a->loaiTK, b->loaiTK) != 0)
                return strcmp(a->loaiTK, b->loaiTK);
            return strcmp(a->soTK, b->soTK);

        default:                              /* mac dinh: theo so tai khoan */
            return strcmp(a->soTK, b->soTK);
    }
}

/*
 * Ham sap xep danh sach tai khoan - thuat toan Selection Sort.
 * Tang dan: thuTu = 1, Giam dan: thuTu = -1.
 *
 * Y tuong: moi vong lap tim mot phan tu "dac biet" (nho nhat neu tang dan)
 * roi doi cho phan tu do ve dung vi tri. Lap lai cho toan bo mang.
 */
void sapXepTaiKhoan(int tieuChi, int thuTu)
{
    int i, j, viTriDacBiet;
    TaiKhoan tam;

    if (nganHang.soTK <= 1) return;

    for (i = 0; i < nganHang.soTK - 1; i++) {
        viTriDacBiet = i;   /* ban dau gia su phan tu dac biet la phan tu i */

        for (j = i + 1; j < nganHang.soTK; j++) {
            int ss = soSanhTK(&nganHang.dsTK[j], &nganHang.dsTK[viTriDacBiet], tieuChi);
            /* Tang dan: tim phan tu nho hon; Giam dan: tim phan tu lon hon */
            if ((thuTu > 0 && ss < 0) || (thuTu < 0 && ss > 0)) {
                viTriDacBiet = j;
            }
        }

        /* Doi cho 2 tai khoan neu vi tri dac biet khac vi tri i */
        if (viTriDacBiet != i) {
            tam = nganHang.dsTK[i];
            nganHang.dsTK[i] = nganHang.dsTK[viTriDacBiet];
            nganHang.dsTK[viTriDacBiet] = tam;
        }
    }
}

/* ======================== 5. HAM LUU / TAI DU LIEU ======================== */

/*
 * Ham luu du lieu he thong ra file.
 * Moi dong ghi 1 tai khoan, cac truong tach nhau bang dau '|'.
 */
int luuDuLieu(const char *tenFile)
{
    FILE *f;
    int i;

    f = fopen(tenFile, "w");
    if (f == NULL) {
        printf("  [!] Khong mo duoc file %s.\n", tenFile);
        return 0;
    }

    fprintf(f, "%d\n", nganHang.soTK);
    for (i = 0; i < nganHang.soTK; i++) {
        fprintf(f, "%s|%s|%s|%s|%lld|%d\n",
                nganHang.dsTK[i].soTK,
                nganHang.dsTK[i].tenTK,
                nganHang.dsTK[i].pin,
                nganHang.dsTK[i].loaiTK,
                nganHang.dsTK[i].soDu,
                nganHang.dsTK[i].trangThai);
    }

    fprintf(f, "%d\n", nganHang.soGD);
    for (i = 0; i < nganHang.soGD; i++) {
        fprintf(f, "%s|%s|%lld|%lld|%s\n",
                nganHang.dsGD[i].soTK,
                nganHang.dsGD[i].loai,
                nganHang.dsGD[i].soTien,
                nganHang.dsGD[i].sauGD,
                nganHang.dsGD[i].thoiGian);
    }

    fclose(f);
    printf("  [+] Da luu vao file %s.\n", tenFile);
    return 1;
}

/*
 * Ham tai du lieu he thong tu file.
 * Dung sscanf de doc cac truong theo dau '|'.
 */
int taiDuLieu(const char *tenFile)
{
    FILE *f;
    char dong[500];
    int soLuong, i;

    f = fopen(tenFile, "r");
    if (f == NULL) return 0;   /* chua co file -> khong co gi de tai */

    khoiTaoHeThong();

    /* --- Doc danh sach tai khoan --- */
    if (fgets(dong, sizeof(dong), f) == NULL) { fclose(f); return 0; }
    soLuong = atoi(dong);
    if (soLuong < 0 || soLuong > MAX_ACC) soLuong = 0;

    for (i = 0; i < soLuong; i++) {
        if (fgets(dong, sizeof(dong), f) == NULL) break;
        if (sscanf(dong, "%19[^|]|%49[^|]|%5[^|]|%19[^|]|%lld|%d",
                   nganHang.dsTK[i].soTK, nganHang.dsTK[i].tenTK,
                   nganHang.dsTK[i].pin, nganHang.dsTK[i].loaiTK,
                   &nganHang.dsTK[i].soDu, &nganHang.dsTK[i].trangThai) != 6) {
            break;
        }
        nganHang.dsTK[i].soGD = 0;
    }
    nganHang.soTK = soLuong;

    /* --- Doc lich su giao dich --- */
    if (fgets(dong, sizeof(dong), f) != NULL) {
        soLuong = atoi(dong);
        if (soLuong < 0 || soLuong > MAX_TX) soLuong = 0;

        for (i = 0; i < soLuong; i++) {
            if (fgets(dong, sizeof(dong), f) == NULL) break;
            if (sscanf(dong, "%19[^|]|%19[^|]|%lld|%lld|%19[^\n]",
                       nganHang.dsGD[i].soTK, nganHang.dsGD[i].loai,
                       &nganHang.dsGD[i].soTien, &nganHang.dsGD[i].sauGD,
                       nganHang.dsGD[i].thoiGian) != 5) {
                break;
            }
        }
        nganHang.soGD = soLuong;
    }

    fclose(f);
    return 1;
}

/* ======================== 6. CAC CHUC NANG TRONG MENU ======================== */

/* Chuc nang 1: Tao tai khoan moi */
void chucNangTaoTaiKhoan(void)
{
    char soTK[MAX_ACCOUNT], tenTK[MAX_NAME], pin[MAX_PIN], xacNhan[MAX_PIN];
    char loaiTK[MAX_TYPE];
    long long soDuBanDau;

    xoaManHinh();
    inTieuDeNho("TAO TAI KHOAN");

    /* --- Nhap so tai khoan (6-19 chu so, va chua ton tai) --- */
    while (1) {
        if (!nhapChuoi("  So TK: ", soTK, MAX_ACCOUNT)) return;
        if (!toanSo(soTK) || strlen(soTK) < 6) {
            printf("  [!] So TK phai co it nhat 6 chu so.\n");
            continue;
        }
        if (timTaiKhoan(soTK) != NULL) {
            printf("  [!] So TK da ton tai. Chon so khac.\n");
            continue;
        }
        break;   /* nhap dung -> thoat vong lap */
    }

    /* --- Nhap ho ten chu ho --- */
    if (!nhapChuoi("  Ho ten: ", tenTK, MAX_NAME)) return;

    /* --- Nhap ma PIN (4 chu so, xac nhan 2 lan) --- */
    while (1) {
        if (!nhapChuoi("  PIN (4 chu so): ", pin, MAX_PIN)) return;
        if (!toanSo(pin) || strlen(pin) != 4) {
            printf("  [!] PIN phai co 4 chu so.\n");
            continue;
        }
        if (!nhapChuoi("  Nhap lai PIN: ", xacNhan, MAX_PIN)) return;
        if (strcmp(pin, xacNhan) != 0) {
            printf("  [!] 2 ma PIN khong khop. Nhap lai.\n");
            continue;
        }
        break;
    }

    /* --- Chon loai tai khoan (switch rẽ nhánh) --- */
    printf("\n  Loai tai khoan:\n");
    printf("    1. %s\n", TK_TIETKIEM);
    printf("    2. %s\n", TK_THANHTOAN);
    printf("    3. %s\n", TK_TINDUNG);
    switch (nhapSo("  Lua chon (1-3): ", 1, 3)) {
        case 1: saoChepChuoi(loaiTK, MAX_TYPE, TK_TIETKIEM);  break;
        case 2: saoChepChuoi(loaiTK, MAX_TYPE, TK_THANHTOAN); break;
        default: saoChepChuoi(loaiTK, MAX_TYPE, TK_TINDUNG);   break;
    }

    /* --- Nhap so du ban dau --- */
    soDuBanDau = nhapTien("  So du ban dau: ", 0, 999000000000LL);
    printf("\n");
    if (taoTaiKhoan(soTK, tenTK, pin, loaiTK, soDuBanDau)) {
        /* Ghi lai 1 giao dich mo ta tai khoan */
        ghiGiaoDich(soTK, "TAO TAI KHOAN", soDuBanDau, soDuBanDau);
    }
}

/*
 * Chuc nang 2: Dang nhap vao tai khoan.
 * Tra ve con tro toi tai khoan sau khi dang nhap, NULL neu that bai.
 */
TaiKhoan *chucNangDangNhap(void)
{
    char soTK[MAX_ACCOUNT], pin[MAX_PIN];
    TaiKhoan *tk;

    xoaManHinh();
    inTieuDeNho("DANG NHAP");

    if (!nhapChuoi("  So TK: ", soTK, MAX_ACCOUNT)) return NULL;
    if (!nhapChuoi("  PIN: ", pin, MAX_PIN)) return NULL;

    tk = dangNhap(soTK, pin);
    if (tk == NULL) {
        printf("  [!] Dang nhap that bai.\n");
        return NULL;
    }

    printf("  [+] Dang nhap thanh cong. Chao %s!\n", tk->tenTK);
    return tk;
}

/* Chuc nang 4: Sap xep danh sach tai khoan */
void chucNangSapXep(void)
{
    int tieuChi, thuTu;

    xoaManHinh();
    inTieuDeNho("SAP XEP TAI KHOAN");

    printf("\n  Tieu chi:\n");
    printf("    1. So tai khoan\n");
    printf("    2. Ten chu ho\n");
    printf("    3. So du\n");
    printf("    4. Loai tai khoan\n");
    tieuChi = nhapSo("  Lua chon (1-4): ", 1, 4);

    printf("\n  Thu tu:\n");
    printf("    1. Tang dan\n");
    printf("    2. Giam dan\n");
    thuTu = nhapSo("  Lua chon (1-2): ", 1, 2);

    /* thuTu = 1 tang dan, thuTu = -1 giam dan */
    sapXepTaiKhoan(tieuChi, (thuTu == 1) ? 1 : -1);

    printf("\n");
    inDanhSachTK();
    tamDung();
}

/*
 * Menu con sau khi dang nhap thanh cong.
 * Tra ve 0 de quay lai menu chinh.
 */
int menuTaiKhoan(TaiKhoan *tk)
{
    int luaChon;

    while (1) {
        xoaManHinh();
        inTieuDe("TAI KHOAN CUA BAN");
        printf("  Chu ho : %s\n", tk->tenTK);
        printf("  So TK  : %s\n", tk->soTK);
        printf("  Loai   : %s\n", tk->loaiTK);
        inGach('=', 62);
        printf("\n");
        printf("  1. Kiem tra so du\n");
        printf("  2. Gui tien\n");
        printf("  3. Rut tien\n");
        printf("  4. Chuyen tien\n");
        printf("  5. Lich su giao dich\n");
        printf("  6. Thong tin tai khoan\n");
        printf("  0. Quay lai\n");
        inGach('-', 62);

        luaChon = nhapSo("  Lua chon (0-6): ", 0, 6);

        switch (luaChon) {
            case 1:     /* Kiem tra so du */
                xoaManHinh();
                inTieuDeNho("KIEM TRA SO DU");
                kiemTraSoDu(tk);
                tamDung();
                break;

            case 2: {   /* Gui tien vao tai khoan */
                long long soTien;
                xoaManHinh();
                inTieuDeNho("GUI TIEN");
                soTien = nhapTien("  So tien gui: ", 1, 999000000000LL);
                guiTien(tk, soTien);
                tamDung();
                break;
            }

            case 3: {   /* Rut tien tu tai khoan */
                long long soTien;
                xoaManHinh();
                inTieuDeNho("RUT TIEN");
                soTien = nhapTien("  So tien rut: ", 1, 999000000000LL);
                rutTien(tk, soTien);
                tamDung();
                break;
            }

            case 4: {   /* Chuyen tien sang tai khoan khac */
                char soTKNhan[MAX_ACCOUNT];
                long long soTien;
                xoaManHinh();
                inTieuDeNho("CHUYEN TIEN");
                if (!nhapChuoi("  So TK nhan: ", soTKNhan, MAX_ACCOUNT)) break;
                soTien = nhapTien("  So tien chuyen: ", 1, 999000000000LL);
                chuyenTien(tk, soTKNhan, soTien);
                tamDung();
                break;
            }

            case 5:     /* Lich su giao dich */
                xoaManHinh();
                inLichSu(tk->soTK);
                tamDung();
                break;

            case 6: {   /* Thong tin tai khoan */
                char tien[MAX_AMOUNT];
                xoaManHinh();
                inTieuDeNho("THONG TIN TAI KHOAN");
                printf("  So TK  : %s\n", tk->soTK);
                printf("  Chu ho : %s\n", tk->tenTK);
                printf("  Loai  : %s\n", tk->loaiTK);
                printf("  Trang thai: %s\n", tk->trangThai ? "Hoat dong" : "Da khoa");
                printf("  So giao dich: %d\n", tk->soGD);
                dinhDangTien(tk->soDu, tien, MAX_AMOUNT);
                printf("  So du : %s VND\n", tien);
                tamDung();
                break;
            }

            case 0:     /* Quay lai menu chinh */
            default:
                return 0;
        }
    }
}

/* ======================== 7. CHUONG TRINH CHINH ======================== */

int main(void)
{
    int luaChon;

    batTiengViet();
    khoiTaoHeThong();

    /* Tu dong tai du lieu da luu (neu co file) */
    if (taiDuLieu("bankdata.txt")) {
        printf("  [+] Da tai %d tai khoan tu file bankdata.txt.\n", nganHang.soTK);
    }

    /* Vong lap vo han: chay menu cho den khi nguoi dung chon thoat */
    while (1) {
        xoaManHinh();
        inTieuDe("QUAN LY TIN DUNG");
        printf("  Ngan hang so\n");
        inGach('=', 62);
        printf("\n");
        printf("  1. Tao tai khoan\n");
        printf("  2. Dang nhap\n");
        printf("  3. Xem danh sach tai khoan\n");
        printf("  4. Sap xep tai khoan\n");
        printf("  5. Luu du lieu\n");
        printf("  6. Thoat\n");
        inGach('-', 62);

        luaChon = nhapSo("  Lua chon (1-6): ", 1, 6);

        switch (luaChon) {
            case 1:     /* Tao tai khoan moi */
                chucNangTaoTaiKhoan();
                tamDung();
                break;

            case 2: {   /* Dang nhap vao tai khoan */
                TaiKhoan *tk = chucNangDangNhap();
                if (tk != NULL) {
                    menuTaiKhoan(tk);
                    printf("\n  Da dang xuat tai khoan.\n");
                }
                tamDung();
                break;
            }

            case 3:     /* Xem danh sach tai khoan */
                xoaManHinh();
                inDanhSachTK();
                tamDung();
                break;

            case 4:     /* Sap xep tai khoan */
                chucNangSapXep();
                break;

            case 5:     /* Luu du lieu */
                xoaManHinh();
                inTieuDeNho("LUU DU LIEU");
                luuDuLieu("bankdata.txt");
                tamDung();
                break;

            case 6:     /* Thoat chuong trinh */
                xoaManHinh();
                inTieuDe("CAM ON BAN DA SU DUNG CHUONG TRINH");
                printf("\n  Chuong trinh ket thuc. Hen gap lai!\n\n");                return 0;

            default:
                break;
        }
    }
}
