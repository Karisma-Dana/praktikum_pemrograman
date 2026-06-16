#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>

#define MAX_BARANG 100
#define STR_LEN 50

typedef struct {
    char kode[STR_LEN];
    char nama[STR_LEN];
    char jenis[STR_LEN];
    int harga;
    int stok;
} Barang;

void backup_file(const char *filename) {
    char backup_name[100];
    snprintf(backup_name, sizeof(backup_name), "backup_%s", filename);
    
    FILE *src = fopen(filename, "rb");
    if (!src) return; 
    
    FILE *dst = fopen(backup_name, "wb");
    if (!dst) { fclose(src); return; }
    
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dst);
    }
    fclose(src); fclose(dst);
}


int cari_barang(Barang *list, int jumlah, const char *kode) {
    for (int i = 0; i < jumlah; i++) {
        if (strcmp(list[i].kode, kode) == 0) return i;
    }
    return -1; 
}


void print_header() {
    printf("\n%-15s %-25s %-15s %-12s %-10s\n", "Kode", "Nama", "Jenis", "Harga", "Stok");
    printf("--------------------------------------------------------------------------------\n");
}

void print_row(Barang b) {
    printf("%-15s %-25s %-15s Rp %-9d %-10d\n", b.kode, b.nama, b.jenis, b.harga, b.stok);
}




void simpan_barang(Barang *list, int jumlah) {
    backup_file("barang.csv");
    FILE *file = fopen("barang.csv", "w");
    if (!file) return;
    
    fprintf(file, "Kode Barang,Jumlah Stok,Nama Barang,Jenis Barang,Harga Barang\n");
    for (int i = 0; i < jumlah; i++) {
        fprintf(file, "%s,%d,%s,%s,%d\n", list[i].kode, list[i].stok, list[i].nama, list[i].jenis, list[i].harga);
    }
    fclose(file);
}

int baca_barang(Barang *list) {
    FILE *file = fopen("barang.csv", "r");
    if (!file) return 0;
    
    char line[256];
    int jumlah = 0;
    fgets(line, sizeof(line), file); 
    
    // Parsing cepat dengan sscanf
    while (fgets(line, sizeof(line), file) && jumlah < MAX_BARANG) {
        sscanf(line, "%[^,],%d,%[^,],%[^,],%d", 
               list[jumlah].kode, &list[jumlah].stok, 
               list[jumlah].nama, list[jumlah].jenis, &list[jumlah].harga);
        jumlah++;
    }
    fclose(file);
    return jumlah;
}

int tambah_barang(Barang *list, int jumlah) {
    if (jumlah >= MAX_BARANG) {
        printf("Kapasitas maksimum data tercapai!\n");
        return jumlah;
    }
    
    Barang b;
    printf("\n=== TAMBAH BARANG ===\nKode  : "); scanf("%s", b.kode);
    
    if (cari_barang(list, jumlah, b.kode) != -1) {
        printf("Error: Kode barang sudah ada!\n");
        return jumlah;
    }
    
    printf("Nama  : "); scanf(" %[^\n]", b.nama);
    printf("Jenis : "); scanf(" %[^\n]", b.jenis);
    printf("Harga : Rp "); scanf("%d", &b.harga);
    printf("Stok  : "); scanf("%d", &b.stok);
    
    list[jumlah++] = b;
    simpan_barang(list, jumlah);
    printf("✅ Barang berhasil ditambahkan!\n");
    return jumlah;
}

void ubah_barang(Barang *list, int jumlah) {
    char kode[STR_LEN];
    printf("\n=== UBAH BARANG ===\nMasukkan Kode: "); scanf("%s", kode);
    
    int idx = cari_barang(list, jumlah, kode);
    if (idx == -1) { printf("Barang tidak ditemukan.\n"); return; }
    
    printf("\n[Data Saat Ini]\n");
    print_header(); print_row(list[idx]);
    
    printf("\nNama Baru  : "); scanf(" %[^\n]", list[idx].nama);
    printf("Jenis Baru : "); scanf(" %[^\n]", list[idx].jenis);
    printf("Harga Baru : Rp "); scanf("%d", &list[idx].harga);
    printf("Stok Baru  : "); scanf("%d", &list[idx].stok);
    
    simpan_barang(list, jumlah);
    printf("✅ Data berhasil diperbarui!\n");
}

int hapus_barang(Barang *list, int jumlah) {
    char kode[STR_LEN];
    printf("\n=== HAPUS BARANG ===\nMasukkan Kode: "); scanf("%s", kode);
    
    int idx = cari_barang(list, jumlah, kode);
    if (idx == -1) { printf("Barang tidak ditemukan.\n"); return jumlah; }
    
    printf("Yakin menghapus '%s'? (y/n): ", list[idx].nama);
    char acc; scanf(" %c", &acc);
    
    if (acc == 'y' || acc == 'Y') {
    
        memmove(&list[idx], &list[idx + 1], (jumlah - idx - 1) * sizeof(Barang));
        jumlah--;
        simpan_barang(list, jumlah);
        printf("✅ Barang berhasil dihapus!\n");
    }
    return jumlah;
}


void tampilkan_barang(Barang *list, int jumlah, int mode) {
    char filter[STR_LEN] = "";
    if (mode == 1) {
        printf("\nMasukkan Jenis: "); scanf(" %[^\n]", filter);
    } else if (mode == 2) {
        printf("\n=== BARANG STOK HABIS ===\n");
    } else {
        printf("\n=== SEMUA BARANG ===\n");
    }
    
    print_header();
    int hitung = 0;
    for (int i = 0; i < jumlah; i++) {
        if (mode == 1 && strcmp(list[i].jenis, filter) != 0) continue;
        if (mode == 2 && list[i].stok > 0) continue;
        
        print_row(list[i]);
        hitung++;
    }
    if (hitung == 0) printf("Data tidak ditemukan atau kosong.\n");
}




void log_transaksi(const char* trx, const char* wkt, const char* kd_b, const char* nm_b, int jml, int hrg, int tot) {
    static int sesi_baru = 1;
    if (sesi_baru) { backup_file("transaksi.csv"); sesi_baru = 0; }
    
    // Cek apakah file sudah ada atau belum
    FILE *cek = fopen("transaksi.csv", "r");
    int butuh_header = (cek == NULL);
    if (cek) fclose(cek); // Kalau ada, tutup lagi
    
    FILE *f = fopen("transaksi.csv", "a");
    if (!f) return;
    
    // Jika file baru, tulis header-nya terlebih dahulu
    if (butuh_header) {
        fprintf(f, "Kode Transaksi,Waktu,Kode Barang,Nama Barang,Jumlah,Harga Satuan,Total Harga\n");
    }
    
    // Tulis data transaksinya
    fprintf(f, "%s,%s,%s,%s,%d,%d,%d\n", trx, wkt, kd_b, nm_b, jml, hrg, tot);
    fclose(f);
}

void transaksi_kasir(Barang *list, int jumlah) {
    char kode[STR_LEN];
    printf("\n=== TRANSAKSI PENJUALAN ===\nKode Barang: "); scanf("%s", kode);
    
    int idx = cari_barang(list, jumlah, kode);
    if (idx == -1) { printf("Barang tidak ditemukan.\n"); return; }
    if (list[idx].stok <= 0) { printf("Stok habis!\n"); return; }
    
    printf("Barang : %s\nHarga  : Rp %d\nSisa   : %d\n", list[idx].nama, list[idx].harga, list[idx].stok);
    printf("Jumlah Beli: "); 
    int qty; scanf("%d", &qty);
    
    if (qty <= 0 || qty > list[idx].stok) { printf("Input jumlah tidak valid!\n"); return; }
    
    int total = qty * list[idx].harga;
    printf("Total Harga: Rp %d. Lanjutkan? (y/n): ", total);
    char acc; scanf(" %c", &acc);
    
    if (acc == 'y' || acc == 'Y') {
        list[idx].stok -= qty;
        
        
        time_t now = time(NULL);
        char trx_kode[30], waktu[30];
        snprintf(trx_kode, sizeof(trx_kode), "TRX%ld", now);
        strftime(waktu, sizeof(waktu), "%Y-%m-%d %H:%M:%S", localtime(&now));
        
        log_transaksi(trx_kode, waktu, list[idx].kode, list[idx].nama, qty, list[idx].harga, total);
        simpan_barang(list, jumlah);
        
        printf("\n✅ Transaksi Berhasil!\nKode Struk: %s\n", trx_kode);
    }
}


void riwayat_transaksi(int mode) {
    char target[STR_LEN] = "";
    if (mode == 1) {
        printf("\nKode Transaksi (Cth: TRX...): "); scanf("%s", target);
    } else {
        printf("\n=== RIWAYAT SEMUA TRANSAKSI ===\n");
    }
    
    FILE *f = fopen("transaksi.csv", "r");
    if (!f) { printf("Belum ada data transaksi tersimpan.\n"); return; }
    
    char line[256];
    fgets(line, sizeof(line), f); 
    
    printf("\n%-15s %-20s %-15s %-20s %-5s %-15s %-15s\n", "Kode", "Waktu", "ID Barang", "Nama", "Qty", "Harga Satuan", "Total Harga");
    printf("------------------------------------------------------------------------------------------------------------\n");
    
    int count = 0; long total_uang = 0;
    char c_trx[30], wkt[30], c_brg[30], n_brg[50];
    int qty, prc, tot;
    
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d,%d,%d", c_trx, wkt, c_brg, n_brg, &qty, &prc, &tot);
        
        if (mode == 1 && strcmp(c_trx, target) != 0) continue;
        
        printf("%-15s %-20s %-15s %-20s %-5d Rp %-12d Rp %-12d\n", c_trx, wkt, c_brg, n_brg, qty, prc, tot);
        count++; total_uang += tot;
    }
    fclose(f);
    
    if (count == 0) {
        printf("Transaksi tidak ditemukan.\n");
    } else if (mode == 0) {
        printf("------------------------------------------------------------------------------------------------------------\n");
        printf("Total Jumlah Transaksi : %d\nTotal Uang Masuk       : Rp %ld\n", count, total_uang);
    }
}



int main() {
    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("\n[LOKASI FILE CSV ANDA]: %s\n\n", cwd);
    }
    Barang list[MAX_BARANG];
    int jumlah = baca_barang(list);
    int pilihan;
    
    backup_file("barang.csv");
    backup_file("transaksi.csv");
    
    do {
        printf("\n===================================\n");
        printf("       SISTEM MANAJEMEN TOKO       \n");
        printf("===================================\n");
        printf(" 1. Tampilkan Semua Barang\n");
        printf(" 2. Tambah Barang Baru\n");
        printf(" 3. Ubah Data Barang\n");
        printf(" 4. Hapus Barang\n");
        printf(" 5. Cari Berdasarkan Jenis\n");
        printf(" 6. Pantau Stok Habis\n");
        printf(" 7. Transaksi / Kasir\n");
        printf(" 8. Cari Struk Transaksi\n");
        printf(" 9. Rekap Semua Transaksi\n");
        printf("10. Keluar\n");
        printf("===================================\nPilih (1-10): ");
        
        if (scanf("%d", &pilihan) != 1) break; // Pengaman input loop
        
        switch(pilihan) {
            case 1: tampilkan_barang(list, jumlah, 0); break;
            case 2: jumlah = tambah_barang(list, jumlah); break;
            case 3: ubah_barang(list, jumlah); break;
            case 4: jumlah = hapus_barang(list, jumlah); break;
            case 5: tampilkan_barang(list, jumlah, 1); break;
            case 6: tampilkan_barang(list, jumlah, 2); break;
            case 7: transaksi_kasir(list, jumlah); break;
            case 8: riwayat_transaksi(1); break;
            case 9: riwayat_transaksi(0); break;
            case 10: printf("\nMenyimpan status dan menutup sistem...\n"); break;
            default: printf("\nInput di luar jangkauan (Pilih 1-10).\n");
        }
    } while (pilihan != 10);
    
    return 0;
}