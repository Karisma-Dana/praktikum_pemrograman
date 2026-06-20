#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
    char nama[50];
    int parent;
} data_keluarga;


void tampilkan_menu() {
    printf("\n=============================================\n");
    printf("     TUGAS PRAKTIKUM ALGORITMA - MODUL 6     \n");
    printf("=============================================\n");
    printf(" 1. tampilkan silsilah keluarga\n");
    printf(" 2. cari manusia dalam keluarga dan susun silsilahnya\n");
    printf(" 3. Keluar Program\n");
    printf("=============================================\n");
    printf("Pilih menu (1-3): ");
}


int cariParent(data_keluarga *db, int n, char *target){
    target[strcspn(target, "\n")] = 0;
    for (int i = 0; i < n; i++){
        if (strcmp(db[i].nama, target) == 0){
            return i;
        }
    }
    return -1;
}

void printSilsilah(data_keluarga *db, int n, int parentIndex, int level, char *namaParent) {
    for (int i = 0; i < n; i++) {
        if (db[i].parent == parentIndex) {
            
            for (int j = 0; j < level; j++) printf("\t");
            
            if (level == 0) {
                printf("%s (root)\n", db[i].nama);
            } else {
                printf("%s (anak %s)\n", db[i].nama, namaParent);
            }
            
            //rekrusif fungsi
            printSilsilah(db, n, i, level + 1, db[i].nama);
        }
    }
}

void tampilkanSilsilahJalur(data_keluarga *db, int n, char *cariNama) {
    int idx_Nama = cariParent(db, n, cariNama);
    
    if (idx_Nama == -1) {
        printf("gagal: nama yang anda cari tidak ada di silsilah keluarga\n");
        return;
    }

    // Hitung panjang jalur (count)
    int count = 1;
    int tempIndex = idx_Nama;
    while (tempIndex != 0 && db[tempIndex].parent != -1) {
        tempIndex = db[tempIndex].parent;
        count++;
    }

    // Simpan jalur ke dalam array
    int silsilah[count];
    int atasan = idx_Nama;
    for (int i = 0; i < count; i++) {
        if (i == 0) {
            silsilah[i] = idx_Nama;
        } else {
            atasan = db[atasan].parent;
            silsilah[i] = atasan;
        }
    }

    // penyusunan silsilah urutan
    printf("\nSILSILAH KELUARGA %s (URUTAN)\n", cariNama);
    for (int i = count - 1; i >= 0; i--) {
        printf("%s", db[silsilah[i]].nama);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

int main(){

    int n;
    printf("masukan jumlah manusia(node) dikeluarga anda = ");
    scanf("%d", &n);
    getchar();

    data_keluarga *database = (data_keluarga *)malloc(n * sizeof(data_keluarga));
    for (int i = 0; i < n; i++){
        strcpy(database[i].nama, "00000");
        database[i].parent = -2;
    }
    
    for (int i = 0; i < n; i ++){
        if (i == 0){
            printf("masukan kepala keluarga (root) = ");
            fgets(database[i].nama, 50, stdin);
            database[i].nama[strcspn(database[i].nama, "\n")] = 0;
            database[i].parent = -1;
        }else{
            char name[50];
            char parent[50];
            printf("masukan nama = ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0; //

            printf("masukan nama parent = ");
            fgets(parent, sizeof(parent), stdin);

            int checkParent = cariParent(database, n, parent);
            if (checkParent != -1){
                strcpy(database[i].nama, name);
                database[i].parent = checkParent;
            }else{
                printf("gagal: parent tidak ada dalam daftar keluarga\n");
                i--;
            }           
        }

    }

    int pilihan;
    while(1){
        tampilkan_menu();
        if (scanf("%d", &pilihan) != 1) {
            printf("Error: Masukkan pilihan berupa angka 1-5!\n");
            getchar();
            continue;
        }
        getchar();
        
        

        if (pilihan == 3) {
            printf("\nTerima kasih, Boss! Program selesai.\n");
            break;
        }

        switch (pilihan)
        {
        case  1:
            printf("\n--- Silsilah Keluarga ---\n");
            printSilsilah(database, n, -1, 0, "");
            break;
        case 2:
            char cariNama[50];
            printf("masukan nama yang anda ingin cari = ");
            fgets(cariNama, sizeof(cariNama), stdin);
            
            cariNama[strcspn(cariNama, "\n")] = 0;
            
            tampilkanSilsilahJalur(database, n, cariNama);
         
            break;
        
        default:
            break;
        }
        
    }













}