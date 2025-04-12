# Praktikum Sistem Operasi Modul 1 - IT17

## Anggota Kelompok

| NRP        | Nama                            |
|:----------:|:-------------------------------:|
| 5027241006 | Nabilah Anindya Paramesti       |
| 5027241092 | Muhammad Khairul Yahya          |
| 5027241002 | Balqis Sani Sabillah            |


## Daftar Isi

- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)
- [Soal 4](#soal-4)

# Soal 1
_**Oleh : A**_

## Deskripsi Soal

## Jawaban
### Soal Tipe A
> Soal

### Penyelesaian A

## Kendala

## Dokumentasi

# Soal 2
_**Oleh : Nabilah Anindya Paramesti**_

## Deskripsi Soal
Pada suatu hari, Kanade ingin membuat sebuah musik baru beserta dengan anggota grup musik lainnya, yaitu Mizuki Akiyama, Mafuyu Asahina, dan Ena Shinonome. Namun sialnya, komputer Kanade terkena sebuah virus yang tidak diketahui. Setelah dianalisis oleh Kanade sendiri, ternyata virus ini bukanlah sebuah trojan, ransomware, maupun tipe virus berbahaya lainnya, melainkan hanya sebuah malware biasa yang hanya bisa membuat sebuah perangkat menjadi lebih lambat dari biasanya.

## Jawaban
### Soal Tipe A
> Sebagai teman yang baik, Mafuyu merekomendasikan Kanade untuk mendownload dan unzip sebuah starter kit berisi file - file acak (sudah termasuk virus) melalui [link berikut](https://drive.google.com/file/d/1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS/view) agar dapat membantu Kanade dalam mengidentifikasi virus - virus yang akan datang. Jangan lupa untuk menghapus file zip asli setelah melakukan unzip.

### Penyelesaian A
Pada soal ini diminta untuk melakukan proses download dan ekstraksi file starter_kit.zip apabila folder starter_kit belum ada. Setelah proses ekstraksi selesai, file zip harus dihapus.

Proses pengecekan dan pengunduhan dilakukan melalui fungsi `ensure_starter_kit_downloaded()` yang memeriksa apakah direktori starter_kit sudah ada, dan jika belum, maka dilakukan pengunduhan file zip, dilanjutkan dengan ekstraksi dan penghapusan file zip.

```bash
void ensure_starter_kit_downloaded() {
    struct stat st;
    if (stat(STARTER_KIT_FOLDER, &st) == -1) {
        pid_t pid = fork();
        if (pid == 0) {
            int nullfd = open("/dev/null", O_WRONLY);
            dup2(nullfd, STDOUT_FILENO);
            dup2(nullfd, STDERR_FILENO);
            close(nullfd);
            execlp("wget", "wget", DOWNLOAD_LINK, "-O", ZIP_FILE, NULL);
            exit(1);
        } else {
            wait(NULL);
            pid = fork();
            if (pid == 0) {
                int nullfd = open("/dev/null", O_WRONLY);
                dup2(nullfd, STDOUT_FILENO);
                dup2(nullfd, STDERR_FILENO);
                close(nullfd);
                execlp("unzip", "unzip", ZIP_FILE, "-d", STARTER_KIT_FOLDER, NULL);
                exit(1);
            } else {
                wait(NULL);
                remove(ZIP_FILE);
            }
        }
    }
}
```

Keterangan :
- `struct stat st;` = Mendeklarasikan variabel `st` bertipe `struct stat`. Struct ini digunakan untuk menyimpan informasi apakah file tersebut ada, apakah berupa direktori, dsb.
- `stat(STARTER_KIT_FOLDER, &st)` = Mengecek apakah folder starter_kit ada. Jika tidak ada `(return -1)`, maka akan masuk ke blok `if`.
- `pid_t pid = fork();` = fork() membuat proses anak (child process). Proses ini akan digunakan untuk menjalankan perintah download ZIP. Jika `pid == 0`, berarti ini adalah proses anak. Maka proses anak akan menjalankan wget
- Output dari proses wget diarahkan ke `/dev/null` agar tidak tampil di layar.
- Setelah proses unduhan selesai `(wait(NULL))`, dilakukan `fork()` kembali untuk menjalankan unzip terhadap file zip.
- Proses unzip juga diarahkan outputnya ke `/dev/null`.
- Setelah selesai, file `starter_kit.zip` dihapus menggunakan `remove()` agar tidak tersisa di sistem.

🤹 Alur Program `ensure_starter_kit_downloaded()`
```bash
if (stat(STARTER_KIT_FOLDER, &st) == -1) {
    pid_t pid = fork();
    if (pid == 0) {
        // CHILD: Download starter_kit.zip via wget
        execlp("wget", ...);
        exit(1);  // <- Kalau wget gagal
    } else {
        // PARENT: Tunggu proses anak selesai download
        wait(NULL);

        // Fork lagi untuk unzip
        pid = fork();
        if (pid == 0) {
            // CHILD: Ekstrak file zip
            execlp("unzip", ...);
            exit(1);  // <- Kalau unzip gagal
        } else {
            // PARENT: Tunggu unzip selesai, lalu hapus file zip
            wait(NULL);
            remove(ZIP_FILE);
        }
    }
}
```

### Soal Tipe B
> Setelah mendownload starter kit tersebut, Mafuyu ternyata lupa bahwa pada starter kit tersebut, tidak ada alat untuk mendecrypt nama dari file yang diencrypt menggunakan algoritma Base64. Oleh karena itu, bantulah Mafuyu untuk membuat sebuah directory karantina yang dapat mendecrypt nama file yang ada di dalamnya (Hint: gunakan daemon). Penggunaan: `./starterkit --decrypt`

### Penyelesaian B
Setelah folder starter_kit berhasil diekstrak, buat proses daemon yang akan melakukan dekripsi nama file terenkripsi (base64) dan memindahkan file yang berhasil didekripsi ke dalam folder quarantine. Setiap aktivitas harus dicatat ke dalam file activity.log.

Permintaan soal B dijawab melalui dua bagian utama:
1. Fungsi `decrypt_daemon()`
2. Fungsi `move_file()` dengan parameter decode aktif (decode_name = 1)

**1. Fungsi `decrypt_daemon()`**
```bash
void decrypt_daemon() {
    pid_t pid = fork();
    if (pid > 0) {
        exit(0);
    } else if (pid == 0) {
        setsid();
```
- Membuat proses child dengan `fork()`.
- Proses parent keluar langsung dengan `exit(0)`.
- Proses child dijadikan session leader menggunakan `setsid()` sehingga menjadi daemon.
```bash
char msg[128];
        snprintf(msg, sizeof(msg), "Successfully started decryption process with PID %d.", getpid());
        write_log(msg);
```
- Menuliskan log bahwa proses dekripsi telah dimulai.
```bash
        chdir("/");
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
```
- Pindah direktori kerja ke root `(/)` dan menutup file descriptor standar agar daemon berjalan di background dengan aman.
```bash
   mkdir(QUARANTINE_FOLDER, 0755);
```
- Membuat folder quarantine jika belum ada.
```bash
        while (1) {
            sleep(10);
        }
    }
}
```
- Loop ini membuat proses daemon tetap hidup (running di background), walaupun belum melakukan tugas seperti mendekripsi file secara otomatis. karena tugas dekripsi tidak terjadi di sini, tapi dijalankan lewat `--quarantine`. Jadi bisa dibilang loop ini membuat proses daemon tetap eksis, dan bisa dimatikan nanti pakai` --shutdown`.

**2. Fungsi `move_file()` dengan `decode_name = 1`**

Fungsi ini dipanggil oleh bagian main() saat user memberikan perintah `--quarantine`. Berikut bagian yang relevan:
```bash
move_file(STARTER_KIT_FOLDER, QUARANTINE_FOLDER, 1, "moved to quarantine");
```
```bash
DIR *dir = opendir(src_dir);
if (!dir) return;
```
- `DIR *dir = opendir(src_dir);` membuka folder sumber (src_dir), misalnya "starter_kit" atau "quarantine".
- Fungsi opendir() mengembalikan pointer ke direktori yang dibuka. Hasilnya disimpan dalam dir, yang akan digunakan untuk membaca isi folder tersebut satu per satu.
- `if (!dir) return;` Jika opendir() gagal (misalnya karena folder tidak ada atau tidak punya izin akses), maka dir bernilai NULL. Maka program langsung return, artinya fungsi move_file() akan berhenti dan tidak memproses apa pun.

```bash
struct dirent *entry;
while ((entry = readdir(dir)) != NULL) {
```
- `struct dirent *entry;` Struktur dirent mewakili entri (file atau folder) dalam sebuah direktori.
- ` while ((entry = readdir(dir)) != NULL)` readdir() akan membaca satu per satu isi folder sampai habis. Setiap entri (misalnya nama file) akan disimpan dalam entry. Loop ini akan terus berjalan sampai semua isi folder dibaca (alias readdir() mengembalikan NULL).

```bash
if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
```
- Mengecek apakah entri itu adalah "." (direktori itu sendiri) atau ".." (direktori induk)
- Maka kalau nama file-nya "." atau "..", langsung continue, alias lompat ke iterasi berikutnya.

 ```bash
char src_path[512], dst_path[512];
snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
```
- Buat path lengkap ke file sumber (src_path) = Misalnya src_dir = "starter_kit" dan entry->d_name = "ZmlsZS50eHQ=", maka hasilnya:
`src_path = "starter_kit/ZmlsZS50eHQ="` Ini akan digunakan untuk mengecek file, dan untuk di-rename nanti.

```bash
if (!is_regular_file(src_path)) continue;
```
- Cek apakah src_path adalah file biasa. `Fungsi is_regular_file()` menggunakan `stat()` untuk memastikan bahwa ini adalah file biasa, bukan folder atau symlink. Kalau bukan file, maka dilewati.

```bash
const char *final_name = entry->d_name;
char *decoded = NULL;
```
- Siapkan nama file tujuan, `final_name` awalnya adalah nama asli file. Tapi kalau decode_name == 1, maka file akan dicoba untuk didekode dari Base64, dan final_name akan diubah ke hasil decode. Decoded digunakan untuk menyimpan hasil decode sementara (jika ada).

**Bagian dari fungsi move_file() yang melakukan dekripsi:**
```bash
if (decode_name) {
    if (!is_base64_string(entry->d_name)) {
        continue;
    }
    size_t out_len;
    decoded = decode_base64(entry->d_name, &out_len);
```
- Mengecek apakah nama file adalah string base64 menggunakan is_base64_string().
- Jika iya, lakukan decoding dengan decode_base64().
```bash
    if (decoded) {
        int len = strlen(decoded);
        while (len > 0 && (decoded[len - 1] == '\n' || decoded[len - 1] == '\r' || decoded[len - 1] == ' ' || decoded[len - 1] == '\t')) {
            decoded[--len] = '\0';
        }
        if (is_printable_string(decoded)) {
            final_name = decoded;
        } else {
            free(decoded);
            continue;
        }
    } else {
        continue;
    }
```
- Membersihkan karakter whitespace dan newline dari hasil decoding.
- Mengecek apakah hasil decoding adalah string yang bisa dibaca (printable).
- Jika valid, gunakan nama hasil decoding untuk memindahkan file.
```bash
snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, final_name);
rename(src_path, dst_path);
```
- Menyusun path tujuan dan memindahkan file dari starter_kit ke quarantine.
```bash
snprintf(logmsg, sizeof(logmsg), "%s - Successfully %s %s directory.",
         final_name, mode, strcmp(mode, "returned") == 0 ? "to starter kit" : "to quarantine");
write_log(logmsg);
```
- Menuliskan aktivitas pemindahan ke file log activity.log.

## Kendala

## Dokumentasi


# Soal 3
_**Oleh : C**_

## Deskripsi Soal

## Jawaban
### Soal Tipe A
> Soal

### Penyelesaian A

## Kendala

## Dokumentasi


# Soal 4
_**Oleh : Nabilah Anindya Paramesti**_

## Deskripsi Soal
Suatu hari, Nobita menemukan sebuah alat aneh di laci mejanya. Alat ini berbentuk robot kecil dengan mata besar yang selalu berkedip-kedip. Doraemon berkata, "Ini adalah Debugmon! Robot super kepo yang bisa memantau semua aktivitas di komputer!" Namun, alat ini harus digunakan dengan hati-hati. Jika dipakai sembarangan, bisa-bisa komputer Nobita malah error total! 


## Jawaban
### Soal Tipe A
> Soal

### Penyelesaian A

## Kendala

## Dokumentasi