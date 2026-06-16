#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

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
    unsigned char checksum = 0; // Inisialisasi awal biner 00000000

    printf("\n--- Memulai Proses Enkripsi File ---\n");

    // Loop membaca file asli per blok buffer
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        // Proses per 1 byte di dalam buffer
        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char originalByte = (unsigned char)buffer[i];

            // 1. Hitung checksum berantai dari data asli sebelum diacak
            checksum = checksum ^ originalByte;

            // 2. Enkripsi byte asli menggunakan Kunci Tunggal
            unsigned char encryptedByte = originalByte ^ key;

            // Taruh kembali hasil enkripsi ke buffer untuk ditulis
            buffer[i] = (char)encryptedByte;
        }
        // Tulis blok yang sudah terenkripsi ke file output
        fwrite(buffer, 1, bytesRead, output);
    }

    // 3. SELESAI LOOP: Tulis 1 byte checksum di ujung paling belakang file output!
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

    // 1. Cari tahu ukuran file terenkripsi terlebih dahulu
    fseek(source, 0, SEEK_END);
    long totalEncryptedFileSize = ftell(source);
    
    // Validasi jika file kosong atau tidak memiliki byte checksum
    if (totalEncryptedFileSize <= 1) {
        printf("Error: File terenkripsi tidak valid atau kosong!\n");
        fclose(source);
        return;
    }

    // 2. Ambil 1 byte checksum tercatat di ujung paling belakang file
    unsigned char checksumTercatat;
    fseek(source, -1, SEEK_END); // Mundur 1 byte dari akhir file
    fread(&checksumTercatat, 1, 1, source);

    // Kembalikan kursor ke awal file untuk mulai mendekripsi data
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
    // Batas data terenkripsi adalah ukuran total dikurangi 1 byte milik ekor checksum
    long dataSizeToDecrypt = totalEncryptedFileSize - 1; 
    unsigned char checksumBaru = 0;

    printf("\n--- Memulai Proses Dekripsi File ---\n");

    // Loop membaca data, tapi batasi agar byte checksum di paling ekor tidak ikut didekripsi
    while (totalBytesProcessed < dataSizeToDecrypt && 
          (bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        
        // Antisipasi jika buffer terakhir ikut membaca byte checksum paling ujung
        if (totalBytesProcessed + bytesRead > dataSizeToDecrypt) {
            bytesRead = dataSizeToDecrypt - totalBytesProcessed;
        }

        // Proses per 1 byte di dalam buffer
        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char encryptedByte = (unsigned char)buffer[i];

            // 1. Dekripsi data acak kembali ke wujud asli menggunakan kunci yang sama
            unsigned char decryptedByte = encryptedByte ^ key;

            // 2. Hitung ulang checksum dari data yang baru saja dipulihkan
            checksumBaru = checksumBaru ^ decryptedByte;

            buffer[i] = (char)decryptedByte;
        }

        fwrite(buffer, 1, bytesRead, output);
        totalBytesProcessed += bytesRead;
    }

    fclose(source);
    fclose(output);

    // 3. VALIDASI AKHIR: Cocokkan sidik jari checksum-nya!
    printf("Hasil Pengujian Identitas File:\n");
    printf("- Checksum Tercatat di Ekor File: %d\n", checksumTercatat);
    printf("- Checksum Hasil Hitung Ulang  : %d\n", checksumBaru);

    if (checksumTercatat == checksumBaru) {
        printf(">>> VALIDASI SUKSES: Checksum MATCH! File pulih sempurna tanpa korup.\n");
    } else {
        printf(">>> VALIDASI GAGAL: Checksum MISMATCH! File korup atau Kunci Rahasia Salah!\n");
    }
}

int main() {
    // Jalur file praktik milikmu, Boss
    const char *fileAsli = "D:\\Programming_karisma\\C\\praktikum_algoritma\\modul5_enkripsi\\etika_profesiUTS.docx";
    const char *fileTerenkripsi = "D:\\Programming_karisma\\C\\praktikum_algoritma\\modul5_enkripsi\\etika_terenkripsi.locked";
    const char *filePulih = "D:\\Programming_karisma\\C\\praktikum_algoritma\\modul5_enkripsi\\etika_profesiUTS_RESTORED.docx";

    // Tentukan kunci tunggal (1 byte), misal angka 42
    unsigned char kunciRahasia = 42;

    // 1. Jalankan Enkripsi
    encrypt_file(fileAsli, fileTerenkripsi, kunciRahasia);

    // 2. Jalankan Dekripsi
    decrypt_file(fileTerenkripsi, filePulih, kunciRahasia);

    return 0;
}