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

# Soal 1 #
*Oleh : Balqis Sani Sabillah*

## Deskripsi Soal ## 
Kamu terbangun secara tiba-tiba di suatu lokasi yang tidak diketahui, saat kamu melihat sekitar yang terlihat hanyalah kegelapan dan sebuah pintu dengan dua mata berwarna yang melihatmu dari lubang pintu tersebut.
Ia merupakan naga bernama Cyrus yang menjaga pintu tersebut dan kamu harus meng-input password yang benar untuk masuk. Karena tidak mungkin untuk menebak password, Ia memberikanmu sebuah clue Clues.zip. Untungnya, kamu merupakan Directory Lister yang jago sehingga clue ini dapat berubah menjadi sebuah password. 
[Author: Fico/ purofuro]

### Tipe soal A ###

**A. Downloading the Clues**

Karena kamu telah diberikan sebuah link Clues oleh Cyrus, kamu membuat file bernama `action.c` yang dimana kalau dijalankan seperti biasa tanpa argumen tambahan akan mendownload dan meng-unzip file tersebut secara langsung. Saat kamu melihat isi dari Clues tersebut, isinya berupa 4 folder yakni ClueA - ClueD dan di masing-masing folder tersebut terdapat .txt files dan isinya masih tidak jelas, mungkin beberapa kata di dalam .txt file yang dapat dicari di inventory website? (Note: inventory bersifat untuk seru-seruan saja).  Jangan lupa untuk menghapus Clues.zip setelah diekstrak dan buatlah apabila folder Clues sudah ada, maka command tersebut tidak akan mendownload `Clues.zip` lagi apabila dijalankan.

### JAWABAN ###

```
void run_command(char *argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv);
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

```
### Keterangan ###
### `pid_t pid = fork();` ###
- `fork()` = digunakan untuk membuat proses anak (child process).
- pid akan berisi: 0 di proses anak, >0 (PID anak) di proses induk.
### `if (pid == 0)` ###
- Artinya kita sekarang berada di proses anak.
- Di sini, kita jalankan perintah terminal pakai `execvp():`
### `execvp(argv[0], argv);` ###
- argv[0] → nama program (misalnya "wget").
- argv → daftar argumen, termasuk argv[0].
### `perror("exec failed");` ###
- Kalau `execvp()` gagal , akan mencetak pesan error ke terminal.
### `exit(EXIT_FAILURE);` ###
- Kalau gagal `exec`, proses anak keluar dengan status gagal `(EXIT_FAILURE)`.


```
void download_and_extract() {
    if (access("Clues", F_OK) == 0) {
        printf("Clues folder already exists. Skipping download.\n");
        return;
    }

    printf("Downloading Clues.zip...\n");
    char *wget_args[] = {"wget", "-O", "Clues.zip", "https://drive.usercontent.google.com/u/0/uc?id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK&export=downloadL/Clues.zip", NULL}; 
    run_command(wget_args);

    printf("Unzipping Clues.zip...\n");
    char *unzip_args[] = {"unzip", "-o", "Clues.zip", NULL};
    run_command(unzip_args);

    printf("Deleting Clues.zip...\n");
    remove("Clues.zip");
}
```

 ### Keterangan ###
 ### `if (access("Clues", F_OK) == 0)`### 
 - Mengecek apakah folder `Clues` sudah ada, `F_OK` : cek keberadaan file.
 - `printf("Clues folder already exists. Skipping download.\n");` = jika file Clues sudah terdownload maka akan keluar kalimat ini
 ### `char *wget_args[] = {"wget", "-O", "Clues.zip", "https://...", NULL}; run_command(wget_args);` ### 
 - download file dari link dan simpan sebagai Clues.zip.
 - Dipanggil dengan `run_command()`, yang akan menjalankan perintah ini di terminal.
### `char *unzip_args[] = {"unzip", "-o", "Clues.zip", NULL}; run_command(unzip_args);` ###
- `-o` artinya: overwrite jika file sudah ada.
- Akan mengekstrak isi Clues.zip (berupa folder Clues berisi petunjuk/teks).
### `remove("Clues.zip");` ###
- Menghapus file `Clues.zip` setelah selesai diekstrak. 


## Tipe Soal B ## 
**B. Filtering the Files**

Karena kebanyakan dari file tersebut berawal dengan 1 huruf atau angka, kamu pun mencoba untuk memindahkan file-file yang hanya dinamakan dengan 1 huruf dan 1 angka tanpa special character kedalam folder bernama Filtered. Kamu tidak suka kalau terdapat banyak clue yang tidak berguna jadi disaat melakukan filtering, file yang tidak terfilter dihapus. Karena kamu tidak ingin membuat file kode lagi untuk filtering, maka kamu menggunakan file sebelumnya untuk filtering file-file tadi dengan menambahkan argumen saat ingin menjalankan action.c

Contoh penggunaan:
puro@furo:~$ ./action -m Filter

### Jawaban ###

```
int is_valid_file(const char *name) {
    return strlen(name) == 5 && isalnum(name[0]) && name[1] == '.' && name[2] == 't' && name[3] == 'x' && name[4] == 't'; }`
```
### Keterangan ###
- ### `int is_valid_file(const char *name) { return strlen(name) == 5 && isalnum(name[0]) && name[1] == '.' && name[2] == 't' && name[3] == 'x' && n [4] == 't'; }` ###
- Tujuan: Mengecek apakah nama file sesuai format seperti `A.txt`, `1.txt`, `x.txt`, dll.
- `strlen(name)` == 5 → Panjang nama harus 5 karakter.
- `isalnum(name[0])` = Karakter pertama harus angka atau huruf.
- `name[1] == '.' && name[2] == 't' && name[3] == 'x' && name[4] == 't'` =  Sisa karakter harus persis .txt.

```
int is_single_char_filename(const char *name) {
    return strlen(name) == 5 &&
           ((isdigit(name[0]) || isalpha(name[0])) && isalnum(name[0])) &&
           name[1] == '.' && strcmp(name + 1, ".txt") == 0;
}
```
### Keterangan ###
- ### `int is_single_char_filename(const char *name) { return strlen(name) == 5 && ((isdigit(name[0]) || isalpha(name[0])) && isalnum(name[0])) && name[1] == '.' && strcmp(name + 1, ".txt") == 0; }` ### 
- `strlen(name)` == 5 → Sama, panjang harus 5.
- `isdigit(name[0]) || isalpha(name[0])` → Karakter pertama boleh huruf atau angka.
- `isalnum(name[0])` → Karakter pertama juga harus alfanumerik (sebenarnya ini redundant).
- `strcmp(name + 1, ".txt")` == 0 → Mengecek karakter dari posisi ke-1 sampai akhir adalah .txt.
- Jadi, intinya dia mengecek apakah nama file itu seperti: a.txt, 9.txt, Z.txt, dll — satu karakter, lalu .txt.


```
void filter_files() {
    mkdir("Filtered", 0755);
    DIR *dir = opendir("Clues");
    if (!dir) {
        perror("opendir Clues");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char subdir[MAX_PATH];
        snprintf(subdir, MAX_PATH, "Clues/%s", entry->d_name);
        DIR *sub = opendir(subdir);
        if (!sub) continue;

        struct dirent *file;
        while ((file = readdir(sub)) != NULL) {
            if (is_single_char_filename(file->d_name)) {
                char src[MAX_PATH], dst[MAX_PATH];
                snprintf(src, MAX_PATH, "%s/%s", subdir, file->d_name);
                snprintf(dst, MAX_PATH, "Filtered/%s", file->d_name);
                rename(src, dst);
            } else if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
                char path[MAX_PATH];
                snprintf(path, MAX_PATH, "%s/%s", subdir, file->d_name);
                remove(path);
            }
        }
        closedir(sub);
    }
    closedir(dir);
}

int is_digit_filename(const char *name) {
    return isdigit(name[0]);
}

```

### Keterangan ###
### `mkdir("Filtered", 0755);` ###
- Ini membuat folder bernama Filtered.
- 0755 adalah permission Unix:
- 7 (rwx): owner bisa baca, tulis, eksekusi
- 5 (r-x): grup bisa baca dan eksekusi
- 5 (r-x): others bisa baca dan eksekusi
- Jadi: folder ini bisa diakses dan dibaca orang lain juga.

### `DIR *dir = opendir("Clues");` ###
- Membuka folder Clues/ → agar kita bisa membaca isinya.
- DIR *dir adalah pointer ke struktur direktori.

### `if (!dir) { perror("opendir Clues"); return; }` ###
- Kalau folder Clues/ tidak bisa dibuka (misalnya tidak ada), tampilkan error dengan perror() dan keluar dari fungsi.

 ### `while ((entry = readdir(dir)) != NULL) { ... }` ###
- Ini loop untuk membaca isi folder Clues/.
- entry akan berisi satu per satu entri (file atau folder) yang ada di Clues/.

### `if (entry->d_type != DT_DIR || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;` ###
- Lewati jika:
- Entitas tersebut bukan folder (d_type != DT_DIR)
- Atau nama foldernya adalah . atau .. (folder khusus sistem)
- Jadi hanya proses subfolder-subfolder valid di dalam Clues/.

### `char subdir[MAX_PATH]; snprintf(subdir, MAX_PATH, "Clues/%s", entry->d_name);` ###
- Membuat path lengkap ke subfolder.

### `DIR *sub = opendir(subdir); if (!sub) continue;` ###
- Buka subfolder-nya.
- Kalau gagal dibuka, skip dan lanjut ke folder berikutnya.

### `while ((file = readdir(sub)) != NULL) { ... }` ###
- Loop untuk membaca isi subfolder yang sedang dibuka.

### `if (is_single_char_filename(file->d_name)) { ... }` ### 
- Cek apakah file sesuai format X.txt atau 3.txt (total 5 karakter).
- Buat path asal (src) dan path tujuan (dst)
- Lalu rename() untuk memindahkan file dari src ke dst.

### `else if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) { ... }` ### 
Kalau file tidak sesuai format, dan bukan . atau .., maka:
- Buat path lengkap ke file
- Hapus file tersebut.

### `closedir(sub); dan closedir(dir);` ###
- Setelah selesai baca subfolder, tutup dengan closedir(sub).
- Setelah semua subfolder diproses, tutup juga folder utama dengan closedir(dir).

### `int is_single_char_filename(const char *name) { return strlen(name) == 5 && ((isdigit(name[0]) || isalpha(name[0])) && isalnum(name[0])) && name[1] == '.' && strcmp(name + 1, ".txt") == 0; }` ###
- Panjang nama = 5 (contoh: A.txt, 3.txt)
- Karakter pertama adalah huruf/angka (isalpha() atau isdigit())
- 4 karakter terakhir adalah .txt

## Tipe Soal C ## 

**C. Combine the File Content**

Di setiap file .txt yang telah difilter terdapat satu huruf dan agar terdapat progress, Cyrus memberikan clue tambahan untuk meletakan/redirect isi dari setiap .txt file tersebut kedalam satu file yaitu Combined.txt dengan menggunakan FILE pointer. Tetapi, terdapat urutan khusus saat redirect isi dari .txt tersebut, yaitu urutannya bergantian dari .txt dengan nama angka lalu huruf lalu angka lagi lalu huruf lagi. Lalu semua file .txt sebelumnya dihapus. Seperti halnya tadi, agar efisien kamu ingin menjalankan action.c dengan argumen tambahan.

Contoh urutan:
1.txt
a.txt
2.txt
b.txt
dst..

		Contoh penggunaan:
		puro@furo:~$ ./action -m Combine

### Jawaban ###

```
void combine_files() {
    FILE *out = fopen("Combined.txt", "w");
    if (!out) {
        perror("fopen Combined.txt");
        return;
    }

    struct dirent *entry;
    DIR *dir = opendir("Filtered");
    if (!dir) {
        perror("opendir Filtered");
        fclose(out);
        return;
    }

    char *filenames[100];
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (is_single_char_filename(entry->d_name)) {
            filenames[count] = strdup(entry->d_name);
            count++;
        }
    }
    closedir(dir);

    // Sort filenames: angka, file, angka, file
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (filenames[i][0] > filenames[j][0]) {
                char *tmp = filenames[i];
                filenames[i] = filenames[j];
                filenames[j] = tmp;
            }
        }
    }

    int use_digit = 1;
    int written = 0;
    while (written < count) {
        for (int i = 0; i < count; i++) {
            if ((use_digit && isdigit(filenames[i][0])) || (!use_digit && isalpha(filenames[i][0]))) {
                char path[MAX_PATH];
                snprintf(path, MAX_PATH, "Filtered/%s", filenames[i]);
                FILE *in = fopen(path, "r");
                if (in) {
                    char c;
                    while ((c = fgetc(in)) != EOF)
                        fputc(c, out);
                    fclose(in);
                    remove(path);
                    written++;
                    use_digit = !use_digit;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < count; i++)
        free(filenames[i]);
    fclose(out);
}

```
### Keterangan ### 
### `FILE *out = fopen("Combined.txt", "w"); if (!out) { perror("fopen Combined.txt"); return; }` ###
- Buat file Combined.txt untuk ditulis ("w" = write).
- Kalau gagal (misalnya permission error), tampilkan pesan error dan keluar dari fungsi.

### `DIR *dir = opendir("Filtered"); if (!dir) { perror("opendir Filtered"); fclose(out); return; }` ###
- Buka folder Filtered/ untuk dibaca isinya.
- Kalau gagal, tampilkan error dan keluar (dengan menutup file out dulu).

### `char *filenames[100]; int count = 0; while ((entry = readdir(dir)) != NULL) { if (is_single_char_filename(entry->d_name)) { filenames[count] = strdup(entry->d_name); count++; } }` ### 
- Loop ini mengumpulkan semua file *.txt yang valid (pakai is_single_char_filename()).
- Disimpan ke array filenames[].
- Fungsi strdup() akan menyimpan salinan string nama file.

### `for (int i = 0; i < count - 1; i++) { for (int j = i + 1; j < count; j++) { if (filenames[i][0] > filenames[j][0]) { char *tmp = filenames[i]; filenames[i] = filenames[j]; filenames[j] = tmp; } } }` ### 
- ` for (int i = 0; i < count - 1; i++) ` = Loop luar: Ambil 1 file (anggap sebagai file "sekarang").
-  `for (int j = i + 1; j < count; j++)` = Loop dalam: Bandingkan file sekarang dengan semua file setelahnya.
-    `if (filenames[i][0] > filenames[j][0]) ` = Kalau huruf/angka pertama file i lebih besar dari file j (misalnya 'c' > 'a', atau '2' > '1') 
- ` char *tmp = filenames[i]; filenames[i] = filenames[j]; filenames[j] = tmp; ` = Tukar posisi mereka supaya file yang lebih kecil ada di depan.

### ` int use_digit = 1; int written = 0; while (written < count) { for (int i = 0; i < count; i++) { if ((use_digit && isdigit(filenames[i][0])) || (!use_digit && isalpha(filenames[i][0]))) { } } }` ### 
- `use_digit = 1:` mulai dengan mencari file yang diawali angka (0-9)
- Setelah dapat file yang sesuai:
- Buka file → salin isinya ke `Combined.txt`
- Hapus file asal dari `Filtered/`
- Ganti `use_digit = !use_digit` → agar selanjutnya cari huruf
### `char path[MAX_PATH]; snprintf(path, MAX_PATH, "Filtered/%s", filenames[i]); FILE *in = fopen(path, "r"); if (in) { char c; while ((c = fgetc(in)) != EOF) fputc(c, out); fclose(in); remove(path); written++; use_digit = !use_digit; break; } `### 
- Bangun path ke file (Filtered/1.txt, dll).
- Buka file input.
- Salin karakter demi karakter ke Combined.txt.
- Setelah itu: Tutup file , Hapus file aslinya (remove()), Tambah hitungan written, Ganti giliran (dari angka → huruf, atau sebaliknya)
### `for (int i = 0; i < count; i++) free(filenames[i]); fclose(out); ` ###
- Setelah semua selesai:
- `free()` setiap nama file yang disimpan dengan `strdup()`
- Tutup file `Combined.txt`
 
## Tipe Soal D ## 
**D. Decode the file** 

Karena isi Combined.txt merupakan string yang random, kamu 
memiliki ide untuk menggunakan Rot13 untuk decode string tersebut dan meletakan hasil dari yang telah di-decode tadi kedalam file bernama Decoded.txt. Jalankan file action.c dengan argumen tambahan untuk proses decoding ini. 

Contoh penggunaan:
puro@furo:~$ ./action -m Decode

```
void decode_rot13() {
    FILE *in = fopen("Combined.txt", "r");
    FILE *out = fopen("Decoded.txt", "w");

    if (!in || !out) {
        perror("fopen Combined.txt / Decoded.txt");
        return;
    }

    char c;
    while ((c = fgetc(in)) != EOF) {
        if ('a' <= c && c <= 'z')
            c = (c - 'a' + 13) % 26 + 'a';
        else if ('A' <= c && c <= 'Z')
            c = (c - 'A' + 13) % 26 + 'A';
        fputc(c, out);
    }

    fclose(in);
    fclose(out);
}

### Jawaban ###

```
### Keterangan ###
### `FILE *in = fopen("Combined.txt", "r"); FILE *out = fopen("Decoded.txt", "w");` ###
- Buka file input (`Combined.txt`) untuk dibaca ("r")
- Buka file output (`Decoded.txt`) untuk ditulis ("w")
### `if (!in || !out) { perror("fopen Combined.txt / Decoded.txt"); return; }` ###
- Kalau salah satu file gagal dibuka (mungkin filenya nggak ada), tampilkan pesan error lalu keluar dari fungsi.
### `char c; while ((c = fgetc(in)) != EOF)` ### 
- Baca satu karakter dari file input, selama belum mencapai EOF (End of File).
### `    if ('a' <= c && c <= 'z') c = (c - 'a' + 13) % 26 + 'a'; ` ###

- kalau karakter c adalah huruf kecil (a sampai z), geser 13 posisi.
Contoh: 'b' → 'o', 'z' → 'm'.
- Cara kerjanya:
- 'c' - 'a': ubah huruf ke angka 0–25
- +13 % 26: geser 13 posisi (dan wrap ke awal kalau lewat z)
- + 'a': ubah angka balik jadi huruf

### `    else if ('A' <= c && c <= 'Z') c = (c - 'A' + 13) % 26 + 'A'; ` ###
- Sama kayak sebelumnya, tapi untuk huruf besar (A sampai Z).

## Tipe Soal E ##
**E. Password Check**

Karena kamu sudah mendapatkan password tersebut, kamu mencoba untuk mengecek apakah password yang sudah kamu dapatkan itu benar atau tidak dengan cara di-input ke lokasi tadi.
Notes: Berikan error handling yakni memberi tahu command yang benar jika salah argumen.

### Jawaban ###

```
void show_usage() {
    printf("Usage:\n");
    printf("./action               --> Download dan unzip Clues.zip\n");
    printf("./action -m Filter     --> Filter files into Filtered/\n");
    printf("./action -m Combine    --> Combine contents into Combined.txt\n");
    printf("./action -m Decode     --> Decode Combined.txt into Decoded.txt using Rot13\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        download_and_extract();
    } else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        if (strcmp(argv[2], "Filter") == 0)
            filter_files();
        else if (strcmp(argv[2], "Combine") == 0)
            combine_files();
        else if (strcmp(argv[2], "Decode") == 0)
            decode_rot13();
        else
            show_usage();
    } else {
        show_usage();
    }
    return 0;
}
```
### Keterangan ### 
- `argc` = jumlah argumen dari command line
- `argv` = array string berisi argumen tersebut
- `./action -m Combine` = 
- argc = 3
- argv[0] = "./action"
- argv[1] = "-m"
- argv[2] = "Combine"
### `if (argc == 1) { download_and_extract(); }` ###
- kalau nggak ada argumen tambahan `(cuma ./action)` , maka:
- Jalankan fungsi `download_and_extract()`
- Fungsinya: download file `Clues.zip` dan ekstrak ke folder `Clues/`
### `else if (argc == 3 && strcmp(argv[1], "-m") == 0) ` ###
-  Kalau ada 3 argumen dan argumen kedua adalah -m, maka:
- Cek isi argumen ketiga:
- `   if (strcmp(argv[2], "Filter") == 0) filter_files(); ` = Kalau `Filter` → jalanin `filter_files()`
- `   else if (strcmp(argv[2], "Combine") == 0) combine_files(); ` = Kalau `Combine` → jalanin `combine_files()`
- `   else if (strcmp(argv[2], "Decode") == 0) decode_rot13();` = Kalau `Decode` → jalanin `decode_rot13()`
- `     else show_usage(); ` = Kalau gak cocok salah satu, tampilin instruksi bantuan.
- `  else { show_usage(); }` = Kalau argumennya aneh-aneh atau jumlahnya salah, tampilkan `show_usage()` juga.

## Kendala

## Dokumentasi

### Soal A
![](assets/Soal%201A.png)

![](assets/Lanjutan%20soal%202A.png)

### Soal B
![](assets/Soal%201B.png)

### Soal C
![](assets/Soal%201C.png)

### Soal D
![](assets/Soal%201D.png)

### Soal E
![](assets/1Soal%20E%20password%20check.png)

Error Check :

![](assets/1Pling%20akhir%20error%20check.png)







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

---

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

---

### Soal Tipe C
> Karena Kanade adalah orang yang sangat pemalas (kecuali jika membuat musik), maka tambahkan juga fitur untuk memindahkan file yang ada pada directory starter kit ke directory karantina, dan begitu juga sebaliknya.

Penggunaan:

- ./starterkit --quarantine (pindahkan file dari directory starter kit ke karantina)
- ./starterkit --return (pindahkan file dari directory karantina ke starter kit)

### Penyelesaian C 
Pada bagian ini, program harus dapat menangani dua perintah:

- `--quarantine`: memindahkan file dari `starter_kit` ke folder `quarantine`, dengan mendekripsi nama file jika terenkripsi menggunakan Base64.
- `--return`: memindahkan file dari `quarantine` kembali ke `starter_kit` tanpa proses dekripsi.

---

### Perintah: `--quarantine`

Saat pengguna menjalankan perintah `--quarantine`, program akan memanggil fungsi:
```c
move_file(STARTER_KIT_FOLDER, QUARANTINE_FOLDER, 1, "moved to quarantine");
```
Artinya:
- `src_dir`: folder asal (`starter_kit`)
- `dst_dir`: folder tujuan (`quarantine`)
- `decode_name`: 1 (artinya dekripsi nama file dilakukan)
- `mode`: untuk keperluan logging

#### Penjelasan Per Baris Kode pada Fungsi `move_file()` (Bagian Quarantine)
```c
DIR *dir = opendir(src_dir);
```
Membuka folder `starter_kit`.

```c
if (!dir) return;
```
Jika folder tidak bisa dibuka, fungsi langsung keluar.

```c
struct dirent *entry;
```
Untuk menyimpan informasi setiap file.

```c
while ((entry = readdir(dir)) != NULL) {
```
Loop membaca isi direktori satu per satu.

```c
if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
```
Lewati entri direktori spesial `.` dan `..`.

```c
char src_path[512], dst_path[512];
snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
```
Membentuk path lengkap file dari direktori asal.

```c
if (!is_regular_file(src_path)) continue;
```
Hanya proses file biasa, bukan direktori.

```c
const char *final_name = entry->d_name;
char *decoded = NULL;
```
Persiapkan nama file akhir. Jika bisa didekripsi, variabel `final_name` akan diganti.

```c
if (decode_name) {
    if (is_base64_string(entry->d_name)) {
        size_t out_len;
        decoded = decode_base64(entry->d_name, &out_len);
```
Jika `decode_name == 1` dan nama file adalah string Base64, maka dilakukan proses decode.

```c
        if (decoded) {
            int len = strlen(decoded);
            while (len > 0 && (decoded[len - 1] == '\n' || decoded[len - 1] == '\r' || decoded[len - 1] == ' ' || decoded[len - 1] == '\t')) {
                decoded[--len] = '\0';
            }
```
Membersihkan whitespace dari hasil dekripsi.

```c
            if (is_printable_string(decoded)) {
                final_name = decoded;
            } else {
                free(decoded);
                decoded = NULL;
            }
        }
    }
}
```
Jika hasil decode bisa dibaca, gunakan sebagai nama final.

```c
snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, final_name);
rename(src_path, dst_path);
```
Bentuk path tujuan dan pindahkan file.

```c
char logmsg[512];
snprintf(logmsg, sizeof(logmsg), "%s - Successfully %s %s directory.", entry->d_name, mode, strcmp(mode, "returned") == 0 ? "to starter kit" : "to quarantine");
write_log(logmsg);
```
Tulis aktivitas ke file `activity.log`.

```c
if (decoded) free(decoded);
```
Jika hasil decode dialokasikan, hapus alokasinya.

---

### Perintah: `--return`

Saat perintah `--return` dijalankan, program akan menjalankan fungsi berikut:
```c
move_file(QUARANTINE_FOLDER, STARTER_KIT_FOLDER, 0, "returned");
```

Perbedaannya:
- `decode_name`: bernilai 0 → artinya tidak dilakukan dekripsi pada nama file.
- File hanya dipindahkan dari `quarantine` ke `starter_kit` tanpa ubahan nama.

#### Penjelasan Proses Return
Semua penjelasan kode di atas juga berlaku, hanya saja bagian:
```c
if (decode_name)
```
Tidak akan dijalankan karena nilainya adalah 0.

Dengan begitu, seluruh file dipindahkan apa adanya dan langsung dicatat aktivitasnya di file `activity.log`.

---

### Kesimpulan

- Pada `--quarantine`, file dari `starter_kit` dipindahkan ke `quarantine`. Jika namanya terenkripsi Base64, nama akan didekripsi.
- Pada `--return`, semua file dari `quarantine` dipindahkan kembali ke `starter_kit` tanpa proses dekripsi.
- Semua aktivitas dicatat ke dalam log.



### Soal Tipe D
> Mafuyu ingin memiliki fitur untuk menghapus seluruh file yang berada di folder `quarantine`. Artinya, ketika dijalankan perintah:
>
> ```bash
> ./starterkit --eradicate
> ```
>
> Maka seluruh file di dalam folder `quarantine` harus dihapus. Aktivitas penghapusan file harus dicatat ke dalam `activity.log`.

### Penyelesaian Tipe D
Program akan mengeksekusi fungsi `delete_files()` yang akan membaca seluruh file dalam folder `quarantine`, lalu menghapusnya satu per satu. Setiap kali file berhasil dihapus, informasi penghapusan akan dicatat ke dalam file `activity.log`.

### Fungsi yang Digunakan
```c
void delete_files(const char *dir_path)
```

### Penjelasan Per Baris:
```c
DIR *dir = opendir(dir_path);
```
- Membuka direktori `quarantine` (atau folder mana pun yang diberikan di argumen `dir_path`).
- `DIR` adalah pointer ke direktori, digunakan untuk membaca isinya.

```c
if (!dir) return;
```
- Jika folder tidak bisa dibuka (misalnya tidak ada), maka fungsi langsung keluar tanpa melakukan apa-apa.

```c
struct dirent *entry;
```
- `entry` digunakan untuk menyimpan informasi setiap entri (file atau folder) yang ditemukan di dalam direktori.

```c
while ((entry = readdir(dir)) != NULL) {
```
- Melakukan loop untuk membaca semua isi direktori satu per satu.

```c
if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
```
- Mengabaikan file spesial “.” dan “..” yang menunjukkan direktori saat ini dan induk direktori.

```c
char filepath[512];
snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
```
- Menyusun path lengkap file (misalnya `quarantine/password.txt`).

```c
if (is_regular_file(filepath)) {
```
- Mengecek apakah `filepath` adalah file biasa, bukan direktori lain.

```c
remove(filepath);
```
- Menghapus file dari sistem menggunakan fungsi `remove()`.

```c
char logmsg[512];
snprintf(logmsg, sizeof(logmsg), "%s - Successfully deleted.", entry->d_name);
write_log(logmsg);
```
- Menuliskan log ke file `activity.log` bahwa file berhasil dihapus.
- Format log-nya sesuai waktu dan nama file yang dihapus.

```c
}
```
- Penutup dari `if (is_regular_file(filepath))`.

```c
}
```
- Penutup dari `while`.

```c
closedir(dir);
```
- Menutup direktori setelah seluruh file dibaca dan diproses.

### Kesimpulan
Saat dijalankan dengan `--eradicate`, program:
1. Membaca seluruh isi folder `quarantine`.
2. Menghapus file satu per satu.
3. Menuliskan setiap aktivitas penghapusan ke dalam `activity.log`.

---

### Soal Tipe E
Setelah proses dekripsi selesai dan file berhasil dipindahkan ke folder `quarantine`, Mafuyu ingin menghentikan proses daemon yang sebelumnya dijalankan menggunakan perintah `--decrypt`.

Program harus menyediakan perintah `--shutdown` untuk menghentikan proses daemon yang berjalan di background.

### Penyelesaian E

Ketika pengguna menjalankan perintah:
```bash
./starterkit --shutdown
```
Program akan mencari proses daemon `./starterkit --decrypt` yang sedang berjalan, lalu menghentikannya menggunakan sinyal `SIGTERM`. Proses penghentian ini juga akan dicatat di file `activity.log`.

Fungsi yang Digunakan: `shutdown_daemon()`

Berikut adalah kode dari fungsi:
```bash
void shutdown_daemon() {
    FILE *cmd = popen("ps -ef | grep './starterkit --decrypt' | grep -v grep", "r");
    if (!cmd) return;
    char buf[256];
    while (fgets(buf, sizeof(buf), cmd)) {
        char user[64], proc[16];
        sscanf(buf, "%s %s", user, proc);
        pid_t pid = atoi(proc);
        kill(pid, SIGTERM);
        char msg[128];
        snprintf(msg, sizeof(msg), "Successfully shut off decryption process with PID %d.", pid);
        write_log(msg);
    }
    pclose(cmd);
}
```
**Keterangan :**
```bash
FILE *cmd = popen("ps -ef | grep './starterkit --decrypt' | grep -v grep", "r");
```
- Menjalankan perintah shell untuk mencari proses `./starterkit --decrypt`.
- popen membuka proses dan membaca outputnya seperti file.

```bash
if (!cmd) return;
```
- Jika perintah gagal dijalankan, maka fungsi keluar tanpa melakukan apapun.
```bash
char buf[256];
while (fgets(buf, sizeof(buf), cmd)) {
```
- Membaca setiap baris hasil dari perintah ps yang berisi informasi proses.
```bash
char user[64], proc[16];
sscanf(buf, "%s %s", user, proc);
```
- Mengambil user dan PID dari baris proses (PID akan digunakan untuk dihentikan).
```bash
pid_t pid = atoi(proc);
```
- Mengubah string PID menjadi integer.
```bash
kill(pid, SIGTERM);
```
- Mengirim sinyal SIGTERM ke PID tersebut untuk menghentikan proses daemon.
```bash
char msg[128];
snprintf(msg, sizeof(msg), "Successfully shut off decryption process with PID %d.", pid);
write_log(msg);
```
- Membuat dan menuliskan log bahwa proses daemon telah dihentikan.
```bash
pclose(cmd);
```
- Menutup proses popen.

### Kesimpulan
Perintah `--shutdown` sangat berguna untuk menghentikan proses daemon --decrypt yang berjalan secara terus-menerus di background. Proses ini dilakukan dengan mencari PID dari daemon, mengirim sinyal `SIGTERM`, dan mencatat aksi tersebut ke file `activity.log`.

---


### Soal Tipe F
Jika pengguna salah mengetikkan perintah (misalnya `--wrongoption`), program harus memberikan pesan error yang sesuai dan menampilkan daftar perintah yang benar. Hal ini penting agar pengguna tidak bingung dan tahu bagaimana cara menggunakan program dengan benar.

Program dijalankan dengan cara:
```bash
./starterkit --perintah_tidak_valid
```

### Penyelesaian F

Untuk menangani error, program menggunakan fungsi:
```c
void print_usage(const char *invalid)
```
Fungsi ini akan:
- Menampilkan pesan error jika parameter tidak valid
- Menampilkan daftar perintah yang benar

Pemanggilan fungsi ini dilakukan di `main()` saat jumlah argumen salah (`argc != 2`) atau argumen tidak cocok dengan pilihan yang tersedia.

#### Contoh Implementasi:
```c
void print_usage(const char *invalid) {
    if (invalid) {
        fprintf(stderr, "Error: Perintah '%s' tidak dikenali.
", invalid);
    }
    fprintf(stderr, "Gunakan salah satu perintah berikut:
");
    fprintf(stderr, "  --decrypt     : Menyediakan 'quarantine' (daemon).
");
    fprintf(stderr, "  --quarantine  : Mendeskripsikan file terenkripsi.
");
    fprintf(stderr, "  --return      : Memindahkan file dari quarantine ke starter_kit.
");
    fprintf(stderr, "  --eradicate   : Menghapus semua file di folder quarantine.
");
    fprintf(stderr, "  --shutdown    : Mematikan proses dekripsi (daemon).
");
}
```

### Penjelasan Per Baris

```c
if (invalid) {
    fprintf(stderr, "Error: Perintah '%s' tidak dikenali.
", invalid);
}
```
- Jika `invalid` tidak NULL, maka tampilkan pesan error bahwa perintah tidak dikenal.
- Pesan dikirim ke **stderr** agar diperlakukan sebagai error oleh sistem atau shell.

```c
fprintf(stderr, "Gunakan salah satu perintah berikut:
");
```
- Menampilkan heading bahwa daftar perintah valid akan segera ditampilkan.

```c
fprintf(stderr, "  --decrypt     : Menyediakan 'quarantine' (daemon).
");
fprintf(stderr, "  --quarantine  : Mendeskripsikan file terenkripsi.
");
fprintf(stderr, "  --return      : Memindahkan file dari quarantine ke starter_kit.
");
fprintf(stderr, "  --eradicate   : Menghapus semua file di folder quarantine.
");
fprintf(stderr, "  --shutdown    : Mematikan proses dekripsi (daemon).
");
```
- Menampilkan lima opsi yang valid, agar pengguna tahu pilihan perintah apa saja yang tersedia.

#### Di Fungsi main():
```c
if (argc != 2) {
    print_usage(NULL);
    return 1;
}
```
- Jika jumlah argumen tidak tepat, tampilkan petunjuk tanpa menunjukkan nama perintah yang salah.

```c
} else {
    print_usage(argv[1]);
    return 1;
}
```
- Jika argumen ada tetapi tidak cocok dengan perintah yang dikenal, tampilkan peringatan dan nama perintah tersebut.

### Kesimpulan

Fitur ini memastikan bahwa program lebih ramah terhadap pengguna. Dengan memberikan informasi saat terjadi kesalahan, pengguna jadi tahu:
- Bahwa perintahnya salah
- Apa saja perintah yang benar

### Soal Tipe G
> Terakhir, untuk mencatat setiap penggunaan program ini, Kanade beserta Mafuyu ingin menambahkan log dari setiap penggunaan program ini dan menyimpannya ke dalam file bernama activity.log.

### Penyelesaian Tipe G
Setiap aktivitas dari program starterkit, baik saat mendekripsi, memindahkan file, menghapus file, hingga mematikan daemon, harus dicatat ke dalam file log bernama activity.log. Untuk melakukan ini, program menggunakan fungsi `write_log()` yang ditulis khusus untuk mencatat pesan tertentu ke file tersebut.

#### Fungsi `write_log()`
Berikut adalah implementasi dari fungsi `write_log`:
```bash
void write_log(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log == NULL) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(log, "[%02d-%02d-%04d][%02d:%02d:%02d] - %s\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec,
            message);
    fclose(log);
}
```

Penjelasan :
```bash
FILE *log = fopen(LOG_FILE, "a");
```
- Membuka file `activity.log` dalam mode append (a) agar pesan baru ditambahkan ke akhir file, bukan menimpa.
- `LOG_FILE` merupakan macro yang nilainya "activity.log".
```bash
if (log == NULL) return;
```
- Kalau file `activity.log` tidak bisa dibuka, program langsung keluar dari fungsi agar tidak menyebabkan error lebih lanjut.
```bash
time_t now = time(NULL);
struct tm *t = localtime(&now);
```
- Mengambil waktu saat ini dan mengubahnya menjadi format waktu lokal.
```bash
fprintf(log, "[%02d-%02d-%04d][%02d:%02d:%02d] - %s\n", ...)
```
- Mencetak pesan log ke dalam file dengan format: `[DD-MM-YYYY][HH:MM:SS] - Pesan Log`
```bash
fclose(log);
```
- Menutup file setelah menulis agar data benar-benar tersimpan ke disk dan tidak korup.

#### Contoh Pemakaian `write_log()` di Program
Pemanggilan fungsi ini terjadi di banyak tempat, misalnya:
```bash
snprintf(msg, sizeof(msg), "Successfully started decryption process with PID %d.", getpid());
write_log(msg);
```
### Kesimpulan
- Fitur logging ini sangat penting karena:
- Memberikan jejak aktivitas program.
- Memudahkan debugging jika ada kesalahan.
- Meningkatkan transparansi penggunaan program.

Dengan adanya activity.log, pengguna seperti Mafuyu dan Kanade bisa mengetahui dengan jelas aktivitas yang telah dilakukan oleh program.

## Dokumentasi
#### Soal A & B
![command decrypt](assets/command2b.PNG)

Ketika menjalankan perintah di atas maka akan muncul file dan folder baru sebagai berikut

![command decrypt](assets/strukturFolder2b.PNG)

![command decrypt](assets/successUnzip.PNG)

File zip dari link berhasil di unzip ke folder `starter_kit`, folder `quarantine` dibuat, dan muncul file `activity.log` untuk mencatat aktivitas. Dari perintah `./starterkit --decrypt` akan muncul pesan sebagai berikut

![command decrypt](assets/log2b.PNG)

### Soal C (`--quarantine`)
![command quarantine](assets/command2c.PNG)

Ketika menjalankan perintah di atas maka file di `starter_kit` akan di pindahkan ke `quarantine` sekaligus nama file yang terenkripsi akan di decrypt

![command quarantine](assets/decryptToQuarantine.PNG)

Aktivitas tersebut juga di catat di `activity.log`

![](assets/log2c.PNG)

### Soal C (`--return`)
![command return](assets/command2d.PNG)

Ketika menjalankan perintah di atas maka file yang tadinya ada di folder `quarantine` akan dipindahkan ke `starter_kit` kembali.

![](assets/moveToStarterKit.PNG)

Dan akan mencatat aktivitas di log file

![](assets/log2d.PNG)

### Soal D
![command eradicate](assets/command1e.PNG)

Ketika menjalankan perintah di atas maka program akan menghapus seluruh file yang ada di folder `quarantine`

![](assets/deleteFile(eradicate).PNG)

Dan akan mencatat di log file

![](assets/log2e.PNG)

### Soal E
Ketika menjalankan perintah **`./starterkit --shutdown`** maka proses daemon dari program ini akan di berhentikan, berikut merupakan pengecekannya menggunakan command `ps aux | starterkit`

![](assets/shutdown.PNG)

Lalu aktivitas juga akan di catat di log file dengan pesan sebagai berikut

![](assets/successShutdown.PNG)


### Soal F
Di soal ini diminta untuk menambahkan error handling, misalnya ketika saya mengetikkan command berikut akan muncul pesan di terminal

![command decrypt](assets/errorHandling.PNG)

### Soal G
Untuk soal ini sudah di dokumentasikan di atas


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
_**Oleh : **_

## Deskripsi Soal
Suatu hari, Nobita menemukan sebuah alat aneh di laci mejanya. Alat ini berbentuk robot kecil dengan mata besar yang selalu berkedip-kedip. Doraemon berkata, "Ini adalah Debugmon! Robot super kepo yang bisa memantau semua aktivitas di komputer!" Namun, alat ini harus digunakan dengan hati-hati. Jika dipakai sembarangan, bisa-bisa komputer Nobita malah error total! 


## Jawaban
### Soal Tipe A
> Soal

### Penyelesaian A

## Kendala

## Dokumentasi
