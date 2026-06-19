#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


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
    long fileSize = get_file_size(sourcePath); // ukuran data dalam bentuk bytes
    // mengecek size file yang akan di split
    const char *ext = strrchr(sourcePath, '.');
    if (ext == NULL){
        ext = "";
    }
    if (fileSize == -1) {
        printf("Error: File '%s' tidak ditemukan!\n", sourcePath);
        return;
    }

    printf("\nUkuran file asli: %ld bytes (%.2f MB)\n", fileSize, (double)fileSize / (1024 * 1024));

    double userChunkSizeInMB; // untuk simpen ukuran potongan file dalam bentuk MB
    printf("Masukkan ukuran potongan yang diinginkan (dalam MB, misal 0.01): ");
    if (scanf("%lf", &userChunkSizeInMB) != 1) {
        printf("Input harus berupa angka!\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    // convert ukuran hasil potongan dari megabytes ke bytes (1mb = 1024 * 1024 )
    long chunkSizeInBytes = (long)(userChunkSizeInMB * 1024 * 1024); // simpen fizesize dalam bentuk bytes

    // check hasil potongan agar tidak minus 
    if (chunkSizeInBytes <= 0) {
        printf("Error: Ukuran potongan tidak valid!\n");
        return;
    }

    // untuk check hasil potongan ga lebih dari filesize (size file dalam bentuk bytes)
    if (chunkSizeInBytes >= fileSize) {
        printf("Error: Ukuran potongan melebihi ukuran file asli. File tidak perlu dipecah!\n");
        return;
    }

    // menentukan total file hasil split
    int totalParts = (fileSize + chunkSizeInBytes - 1) / chunkSizeInBytes; // untuk mengihindari kesalahan pembagian. misalkan 10 / 3 akan hasilnya file akan terbagi menjadi 4 bagian. intinya untuk pembulatan keatas
    // if (totalParts > 7) {
    //     printf("Error: Ukuran %.4f MB terlalu kecil! File akan terpecah menjadi %d bagian (Maksimal 7 bagian).\n", userChunkSizeInMB, totalParts);
    //     return;
    // }
    printf("file akan dipencah menjadi %d\n", totalParts);

    FILE *source = fopen(sourcePath, "rb"); // membuka file output
    if (source == NULL) {
        printf("Gagal membuka file asal saat proses pemecahan.\n");
        return;
    }

    char buffer[BUFFER_SIZE];
    char partName[512];
    int partCounter = 1; 
    size_t bytesRead;

    printf("\n--- Memulai Proses Split File ---\n");

    while (!feof(source)) { // melakukan while loop dari awal file sampai ujung file 
       
        sprintf(partName, "%s_part%d.bin", prefixName, partCounter); // mencetak nama file ke partName

        FILE *part = fopen(partName, "wb");
        if (part == NULL) {
            printf("Gagal membuat file potongan: %s\n", partName);
            fclose(source);
            return;
        }

        size_t totalBytesWrittenToPart = 0;
        size_t bytesToRead;

        // Di dalam loop utama split_file, setelah fopen(part, "wb") sukses:
        if (partCounter == 1) {
            // Kita buat kesepakatan: 16 byte pertama part 1 khusus untuk string ekstensi
            char extHeader[16] = {0}; 
            strncpy(extHeader, ext, sizeof(extHeader) - 1);
            
            // Tulis 16 byte header ini ke part 1
            fwrite(extHeader, 1, sizeof(extHeader), part);
        }

        // ambil satu per satu (sejumlah ukuran buffer) sampai sebanyak chucksizeBytes nya
        while (totalBytesWrittenToPart < chunkSizeInBytes) {
            size_t bytesLeft = chunkSizeInBytes - totalBytesWrittenToPart;

            //penyendokan
            // Jika sisa kuota lebih besar dari gayung (4096), sendok full 4096.
            // Jika sisa kuota lebih kecil, sendok sejumlah sisa kuotanya saja!. 
            if (bytesLeft < sizeof(buffer)){
                bytesToRead = bytesLeft;
            }else{
                bytesToRead = sizeof(buffer);
            }

            bytesRead = fread(buffer, 1, bytesToRead, source);    // mengambil data dari souce sejumlah buffer dan disimpan di variable bytes Read

            if(bytesRead == 0){
                break;
            }
            
            // menulis dta yang disimpan di bytesRead ke file pecahan yang namanya variable part
            fwrite(buffer, 1, bytesRead, part);
            totalBytesWrittenToPart += bytesRead; // update 
        }
        
        printf("Berhasil membuat: %s (%ld bytes)\n", partName, totalBytesWrittenToPart);
        fclose(part);
        partCounter++;
    }
    
    fclose(source);
    printf("Proses split selesai! Total file potongan: %d\n", partCounter - 1);
}


void mergeFile(const char *outputDir, const char *prefixName) {
    char partName[512];
    char extFormat[16] = {0}; // Wadah untuk menyimpan ekstensi (misal: ".docx")
    char namaFileLengkap[1024];

    // 1. Buka part 1 terlebih dahulu untuk mengambil header ekstensi
    sprintf(partName, "%s_part1.bin", prefixName);
    FILE *part1 = fopen(partName, "rb");
    if (part1 == NULL) {
        printf("Gagal: File pecahan pertama (%s) tidak ditemukan!\n", partName);
        return;
    }

    // Baca 16 byte pertama dari part 1
    fread(extFormat, 1, 16, part1);
    fclose(part1); 

    // 2. Rangkai path output lengkap secara otomatis
    // Menggabungkan direktori tujuan, nama awalan, kata "_pulih", dan ekstensi aslinya
    // Contoh hasil: D:\Folder\tugas_uts_pulih.docx
    sprintf(namaFileLengkap, "%s\\%s_pulih%s", outputDir, prefixName, extFormat);

    FILE *output = fopen(namaFileLengkap, "wb");
    if (output == NULL) {
        printf("Gagal membuat file output utama di: %s\n", namaFileLengkap);
        return;
    }

    char buffer[BUFFER_SIZE];
    int partCounter = 1;
    size_t bytesRead;

    printf("\n--- Memulai Proses Penggabungan File ---\n");
    printf("Format file asli terdeteksi: %s\n", extFormat[0] != '\0' ? extFormat : "Tidak ada ekstensi");

    while (1) {
        sprintf(partName, "%s_part%d.bin", prefixName, partCounter);
        
        FILE *part = fopen(partName, "rb");
        if (part == NULL) {
            break; // Jika part berikutnya tidak ada, hentikan perburuan
        }

        // 3. KHUSUS PART 1: Lewati 16 byte pertama agar header tidak ikut tergabung ke file asli
        if (partCounter == 1) {
            fseek(part, 16, SEEK_SET);
        }

        // Mesin pemindah data
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), part)) > 0) {
            fwrite(buffer, 1, bytesRead, output);
        }

        printf("Berhasil menggabungkan %s...\n", partName);
        fclose(part);
        partCounter++;
    }
    
    fclose(output);
    
    if (partCounter == 1) {
        printf("Gagal: Terjadi kesalahan saat membaca file pecahan.\n");
        remove(namaFileLengkap); // Hapus file kosong yang terlanjur dibuat
    } else {
        printf("Proses penggabungan selesai! File output sukses dipulihkan: %s\n", namaFileLengkap);
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

                // User HANYA perlu memasukkan foldernya saja
                printf("Masukkan folder tujuan hasil pemulihan (misal: D:\\Folder):\n--> ");
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

void decrypt_file(const char *sourcePath, const char *outputPath, unsigned char key) {
    // 1. Membuka file terenkripsi (source) dan file tujuan (output)
    FILE *source = fopen(sourcePath, "rb");
    if (source == NULL) {
        printf("Gagal membuka file terenkripsi: %s\n", sourcePath);
        return;
    }

    // 2. Mengecek ukuran file untuk mencari posisi Checksum di ekor file
    fseek(source, 0, SEEK_END);
    long totalEncryptedFileSize = ftell(source);
    
    // 3. Mengambil nilai Checksum dari 1 byte terakhir
    unsigned char checksumTercatat;
    fseek(source, -1, SEEK_END); 
    fread(&checksumTercatat, 1, 1, source);
    rewind(source); // Mengembalikan kursor ke awal file untuk mulai membaca data

    // 4. Membuka file output untuk menyimpan hasil dekripsi
    FILE *output = fopen(outputPath, "wb");
    if (output == NULL) {
        printf("Gagal membuat file output dekripsi: %s\n", outputPath);
        fclose(source);
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    long totalBytesProcessed = 0;
    long dataSizeToDecrypt = totalEncryptedFileSize - 1; // Ukuran data asli (tanpa checksum)
    unsigned char checksumBaru = 0;

    printf("\n--- Memulai Proses Dekripsi File ---\n");

    // 5. Proses utama: Membaca file dan melakukan XOR balik
    while (totalBytesProcessed < dataSizeToDecrypt && 
          (bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        
        // Memastikan tidak membaca byte checksum sebagai data
        if (totalBytesProcessed + bytesRead > dataSizeToDecrypt) {
            bytesRead = dataSizeToDecrypt - totalBytesProcessed;
        }

        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char encryptedByte = (unsigned char)buffer[i];
            
            // Proses Dekripsi: XOR kembali dengan kunci yang sama
            unsigned char decryptedByte = encryptedByte ^ key;
            
            // Hitung checksum baru dari data yang sudah didekripsi
            checksumBaru = checksumBaru ^ decryptedByte;
            buffer[i] = (char)decryptedByte;
        }

        fwrite(buffer, 1, bytesRead, output);
        totalBytesProcessed += bytesRead;
    }

    fclose(source);
    fclose(output);

    // 6. Validasi akhir: Apakah file benar-benar pulih sempurna?
    printf("\nHasil Pengujian Identitas File:\n");
    printf("- Checksum Tercatat di Ekor File: %d\n", checksumTercatat);
    printf("- Checksum Hasil Hitung Ulang  : %d\n", checksumBaru);

    if (checksumTercatat == checksumBaru) {
        printf(">>> VALIDASI SUKSES: Checksum MATCH! File pulih sempurna tanpa korup.\n");
    } else {
        printf(">>> VALIDASI GAGAL: Checksum MISMATCH! File korup atau Kunci Rahasia Salah!\n");
    }
}