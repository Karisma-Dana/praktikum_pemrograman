#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096


void tampilkan_menu() {
    printf("\n=============================================\n");
    printf("     TUGAS PRAKTIKUM ALGORITMA - MODUL 5     \n");
    printf("=============================================\n");
    printf(" 1. Split File (Pemecahan)\n");
    printf(" 2. Merge File (Penggabungan)\n");
    printf(" 3. Enkripsi File (XOR Cipher)\n");
    printf(" 4. Dekripsi File (Pemulihan)\n");
    printf(" 5. Keluar Program\n");
    printf("=============================================\n");
    printf("Pilih menu (1-5): ");
}


void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void hapus_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

long get_file_size(const char *sourcePath) {
    FILE *source = fopen(sourcePath, "rb");
    if (source == NULL) {
        return -1;
    }
    fseek(source, 0, SEEK_END);
    long fileSize = ftell(source);
    fclose(source);
    return fileSize;
}


void split_file(const char *sourcePath, const char *prefixName) {
    long fileSize = get_file_size(sourcePath);
    if (fileSize == -1) {
        printf("Error: File '%s' tidak ditemukan!\n", sourcePath);
        return;
    }

    printf("\nUkuran file asli: %ld bytes (%.2f MB)\n", fileSize, (double)fileSize / (1024 * 1024));

    double userChunkSizeInMB;
    printf("Masukkan ukuran potongan yang diinginkan (dalam MB, misal 0.01): ");
    if (scanf("%lf", &userChunkSizeInMB) != 1) {
        printf("Input harus berupa angka!\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    long chunkSizeInBytes = (long)(userChunkSizeInMB * 1024 * 1024);

    if (chunkSizeInBytes <= 0) {
        printf("Error: Ukuran potongan tidak valid!\n");
        return;
    }
    if (chunkSizeInBytes >= fileSize) {
        printf("Error: Ukuran potongan melebihi ukuran file asli. File tidak perlu dipecah!\n");
        return;
    }

    int totalParts = (fileSize + chunkSizeInBytes - 1) / chunkSizeInBytes;
    if (totalParts > 7) {
        printf("Error: Ukuran %.4f MB terlalu kecil! File akan terpecah menjadi %d bagian (Maksimal 7 bagian).\n", userChunkSizeInMB, totalParts);
        return;
    }

    FILE *source = fopen(sourcePath, "rb");
    if (source == NULL) {
        printf("Gagal membuka file asal saat proses pemecahan.\n");
        return;
    }

    char buffer[BUFFER_SIZE];
    char partName[512];
    int partCounter = 1; 
    size_t bytesRead;

    printf("\n--- Memulai Proses Split File ---\n");

    while (!feof(source)) {
        // PEMBARUAN: Format nama file menggunakan prefix kustom dari user
        sprintf(partName, "%s_part%d.bin", prefixName, partCounter);

        FILE *part = fopen(partName, "wb");
        if (part == NULL) {
            printf("Gagal membuat file potongan: %s\n", partName);
            fclose(source);
            return;
        }

        size_t totalBytesWrittenToPart = 0;
        while (totalBytesWrittenToPart < chunkSizeInBytes && 
              (bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
            
            fwrite(buffer, 1, bytesRead, part);
            totalBytesWrittenToPart += bytesRead;
        }
        
        printf("Berhasil membuat: %s (%ld bytes)\n", partName, totalBytesWrittenToPart);
        fclose(part);
        partCounter++;
    }
    
    fclose(source);
    printf("Proses split selesai! Total file potongan: %d\n", partCounter - 1);
}


void mergeFile(const char *outputPath, const char *prefixName) {
    FILE *output = fopen(outputPath, "wb");
    if (output == NULL) {
        printf("Gagal membuat file output utama: %s\n", outputPath);
        return;
    }

    char buffer[BUFFER_SIZE];
    char partName[512];
    int partCounter = 1;
    size_t bytesRead;

    printf("\n--- Memulai Proses Penggabungan File ---\n");

    while (1) {

        sprintf(partName, "%s_part%d.bin", prefixName, partCounter);
        
        FILE *part = fopen(partName, "rb");
        if (part == NULL) {
            break; 
        }

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), part)) > 0) {
            fwrite(buffer, 1, bytesRead, output);
        }

        printf("Menggabungkan %s...\n", partName);
        fclose(part);
        partCounter++;
    }
    fclose(output);
    
    if (partCounter == 1) {
        printf("Gagal: Tidak ada file potongan dengan nama awalan '%s' yang ditemukan.\n", prefixName);
    } else {
        printf("Proses penggabungan selesai! File output sukses dipulihkan: %s\n", outputPath);
    }
}

void encrypt_file(const char *sourcePath, const char *outputPath, unsigned char key) {
    FILE *source = fopen(sourcePath, "rb");
    if (source == NULL) {
        printf("Gagal membuka file asal untuk enkripsi: %s\n", sourcePath);
        return;
    }

    FILE *output = fopen(outputPath, "wb");
    if (output == NULL) {
        printf("Gagal membuat file output enkripsi: %s\n", outputPath);
        fclose(source);
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    unsigned char checksum = 0;

    printf("\n--- Memulai Proses Enkripsi File ---\n");

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char originalByte = (unsigned char)buffer[i];
            checksum = checksum ^ originalByte;
            unsigned char encryptedByte = originalByte ^ key;
            buffer[i] = (char)encryptedByte;
        }
        fwrite(buffer, 1, bytesRead, output);
    }

    fwrite(&checksum, 1, 1, output);
    printf("Berhasil membuat file terenkripsi: %s\n", outputPath);
    printf("Sidik jari digital (Checksum Asli): %d\n", checksum);

    fclose(source);
    fclose(output);
}

void decrypt_file(const char *sourcePath, const char *outputPath, unsigned char key) {
    FILE *source = fopen(sourcePath, "rb");
    if (source == NULL) {
        printf("Gagal membuka file terenkripsi: %s\n", sourcePath);
        return;
    }

    fseek(source, 0, SEEK_END);
    long totalEncryptedFileSize = ftell(source);
    
    if (totalEncryptedFileSize <= 1) {
        printf("Error: File terenkripsi tidak valid atau kosong!\n");
        fclose(source);
        return;
    }

    unsigned char checksumTercatat;
    fseek(source, -1, SEEK_END); 
    fread(&checksumTercatat, 1, 1, source);
    rewind(source);

    FILE *output = fopen(outputPath, "wb");
    if (output == NULL) {
        printf("Gagal membuat file output dekripsi: %s\n", outputPath);
        fclose(source);
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    long totalBytesProcessed = 0;
    long dataSizeToDecrypt = totalEncryptedFileSize - 1; 
    unsigned char checksumBaru = 0;

    printf("\n--- Memulai Proses Dekripsi File ---\n");

    while (totalBytesProcessed < dataSizeToDecrypt && 
          (bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        
        if (totalBytesProcessed + bytesRead > dataSizeToDecrypt) {
            bytesRead = dataSizeToDecrypt - totalBytesProcessed;
        }

        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char encryptedByte = (unsigned char)buffer[i];
            unsigned char decryptedByte = encryptedByte ^ key;
            checksumBaru = checksumBaru ^ decryptedByte;
            buffer[i] = (char)decryptedByte;
        }

        fwrite(buffer, 1, bytesRead, output);
        totalBytesProcessed += bytesRead;
    }

    fclose(source);
    fclose(output);

    printf("\nHasil Pengujian Identitas File:\n");
    printf("- Checksum Tercatat di Ekor File: %d\n", checksumTercatat);
    printf("- Checksum Hasil Hitung Ulang  : %d\n", checksumBaru);

    if (checksumTercatat == checksumBaru) {
        printf(">>> VALIDASI SUKSES: Checksum MATCH! File pulih sempurna tanpa korup.\n");
    } else {
        printf(">>> VALIDASI GAGAL: Checksum MISMATCH! File korup atau Kunci Rahasia Salah!\n");
    }
}


int main() {
    int pilihan;
    char pathAsal[512];
    char pathTujuan[512];
    char namaAwalan[256];
    int kunciInput;

    while (1) {
        tampilkan_menu();
        if (scanf("%d", &pilihan) != 1) {
            printf("Error: Masukkan pilihan berupa angka 1-5!\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer(); 

        if (pilihan == 5) {
            printf("\nTerima kasih, Boss! Program selesai.\n");
            break;
        }

        switch (pilihan) {
            case 1:
                printf("\n>>> MENU 1: SPLIT FILE <<<\n");
                printf("Masukkan path file lengkap yang mau di-split:\n--> ");
                fgets(pathAsal, sizeof(pathAsal), stdin);
                hapus_newline(pathAsal);

                printf("Masukkan nama awalan untuk file hasil potongan (misal: tugas_uts):\n--> ");
                fgets(namaAwalan, sizeof(namaAwalan), stdin);
                hapus_newline(namaAwalan);

                split_file(pathAsal, namaAwalan);
                break;

            case 2:
                printf("\n>>> MENU 2: MERGE FILE <<<\n");
                printf("Masukkan nama awalan potongan yang ingin digabung (misal: tugas_uts):\n--> ");
                fgets(namaAwalan, sizeof(namaAwalan), stdin);
                hapus_newline(namaAwalan);

                printf("Masukkan path lengkap hasil penggabungan (misal: D:\\Folder\\hasil_pulih.docx):\n--> ");
                fgets(pathTujuan, sizeof(pathTujuan), stdin);
                hapus_newline(pathTujuan);

                mergeFile(pathTujuan, namaAwalan);
                break;

            case 3:
                printf("\n>>> MENU 3: ENKRIPSI FILE <<<\n");
                printf("Masukkan path file asli yang mau dienkripsi:\n--> ");
                fgets(pathAsal, sizeof(pathAsal), stdin);
                hapus_newline(pathAsal);

                printf("Masukkan path hasil enkripsi (misal: D:\\Folder\\terkunci.dat):\n--> ");
                fgets(pathTujuan, sizeof(pathTujuan), stdin);
                hapus_newline(pathTujuan);

                printf("Masukkan Kunci Enkripsi Angka (0-255): ");
                scanf("%d", &kunciInput);
                clear_input_buffer();

                encrypt_file(pathAsal, pathTujuan, (unsigned char)kunciInput);
                break;

            case 4:
                printf("\n>>> MENU 4: DEKRIPSI FILE <<<\n");
                printf("Masukkan path file terenkripsi (.dat):\n--> ");
                fgets(pathAsal, sizeof(pathAsal), stdin);
                hapus_newline(pathAsal);

                printf("Masukkan path lengkap hasil pemulihan (misal: D:\\Folder\\file_normal.docx):\n--> ");
                fgets(pathTujuan, sizeof(pathTujuan), stdin);
                hapus_newline(pathTujuan);

                printf("Masukkan Kunci Dekripsi Angka: ");
                scanf("%d", &kunciInput);
                clear_input_buffer();

                decrypt_file(pathAsal, pathTujuan, (unsigned char)kunciInput);
                break;

            default:
                printf("Pilihan tidak valid! Masukkan angka antara 1 sampai 5.\n");
        }
    }

    return 0;
}