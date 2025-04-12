# Praktikum Sistem Operasi Modul 2 - IT17

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
> Setelah mengunduh starter kit, Mafuyu menyadari bahwa nama-nama file di dalam folder `starter_kit` dienkripsi menggunakan algoritma Base64. Oleh karena itu, diminta untuk membuat sebuah proses daemon yang bertugas untuk mendekripsi nama-nama file tersebut dan memindahkannya ke folder `quarantine`. Setiap aktivitas perlu dicatat ke dalam file log. Perintah yang digunakan untuk menjalankan proses ini adalah:  
>  
> `./starterkit --decrypt`

### Penyelesaian B

Setelah folder `starter_kit` berhasil diekstraksi, program akan menjalankan sebuah **proses daemon** yang bertanggung jawab untuk **menyediakan proses latar belakang**. Daemon ini tidak secara langsung melakukan dekripsi, tetapi ia **menjadi proses aktif** yang dapat dikendalikan, misalnya untuk dimatikan (`--shutdown`). Proses pemindahan dan dekripsi file terjadi saat menjalankan perintah `--quarantine`.

### **Fungsi `decrypt_daemon()`**
```c
pid_t pid = fork();
```
- Membuat proses baru menggunakan `fork()`.
- `fork()` akan menghasilkan dua proses: **proses induk (parent)** dan **proses anak (child)**.

```c
if (pid > 0) {
    exit(0);
}
```
- Jika proses yang sedang berjalan adalah parent, maka ia akan **langsung keluar** dari fungsi.
- Hal ini dilakukan agar **hanya proses anak** yang melanjutkan sebagai daemon.

```c
else if (pid == 0) {
    setsid();
```
- Jika ini adalah proses anak, maka ia akan menjadi session leader dengan `setsid()`.
- Artinya, proses ini akan benar-benar terpisah dari terminal — menjadi **proses daemon**.

```c
char msg[128];
snprintf(msg, sizeof(msg), "Successfully started decryption process with PID %d.", getpid());
write_log(msg);
```
- Menuliskan pesan log ke file `activity.log`, menunjukkan bahwa proses dekripsi (daemon) telah dimulai.

```c
chdir("/");
fclose(stdin);
fclose(stdout);
fclose(stderr);
```
- Mengganti direktori kerja ke root (`/`), dan menutup tiga file descriptor standar (input, output, error).
- Ini adalah **prosedur umum** dalam pembuatan daemon agar benar-benar berjalan di background dan tidak bergantung pada terminal.

```c
mkdir(QUARANTINE_FOLDER, 0755);
```
- Membuat folder `quarantine` jika belum tersedia.
- Folder ini akan menjadi lokasi file-file yang sudah berhasil didekripsi.

```c
while (1) {
    sleep(10);
}
```
- Loop tanpa akhir (`while(1)`) digunakan agar proses daemon tetap berjalan.
- `sleep(10)` digunakan agar proses ini tidak membebani CPU — ia akan “tidur” selama 10 detik di setiap siklusnya.
- **Proses ini tidak melakukan dekripsi langsung**, tetapi bertugas menjaga agar daemon aktif.  
  Dekripsi dan pemindahan file dilakukan ketika perintah `--quarantine` dijalankan.

### Kesimpulan
Fungsi `decrypt_daemon()` membuat program berjalan sebagai proses **daemon (background service)** yang:
- Terpisah dari terminal
- Tidak langsung melakukan pemrosesan
- Tetap aktif dan menunggu instruksi lain
- Mencatat informasi ke dalam log
- Membuat folder karantina jika belum ada

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