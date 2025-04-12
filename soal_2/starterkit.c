#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdint.h>
#include <ctype.h>

#define STARTER_KIT_FOLDER "starter_kit"
#define QUARANTINE_FOLDER "quarantine"
#define ZIP_FILE "starter_kit.zip"
#define DOWNLOAD_LINK "https://drive.google.com/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download"
#define LOG_FILE "activity.log"

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Fungsi buat mendapatkan nilai dari karakter base64
int base64_char_value(char c) {
    const char *p = strchr(base64_chars, c);
    return p ? (p - base64_chars) : -1;
}

// Fungsi buat decode string dari base64 ke string biasa
char *decode_base64(const char *input, size_t *output_length) {
    size_t input_length = strlen(input);
    if (input_length % 4 != 0) return NULL;

    *output_length = (3 * input_length) / 4;
    if (input[input_length - 1] == '=') (*output_length)--;
    if (input[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded = (unsigned char *)malloc(*output_length + 1);
    if (!decoded) return NULL;

    size_t j = 0;
    uint32_t sextet_bits = 0;
    int sextet_count = 0;

    for (size_t i = 0; i < input_length; i++) {
        if (input[i] == '=') break;
        int val = base64_char_value(input[i]);
        if (val == -1) continue;

        sextet_bits = (sextet_bits << 6) | val;
        sextet_count++;

        if (sextet_count == 4) {
            decoded[j++] = (sextet_bits >> 16) & 0xFF;
            if (j < *output_length) decoded[j++] = (sextet_bits >> 8) & 0xFF;
            if (j < *output_length) decoded[j++] = sextet_bits & 0xFF;
            sextet_bits = 0;
            sextet_count = 0;
        }
    }
    decoded[*output_length] = '\0';
    return (char *)decoded;
}

// Ngecek apakah suatu string terdiri dari karakter yang bisa ditampilkan (bukan karakter aneh)
int is_printable_string(const char *s) {
    while (*s) {
        if (!isprint((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

// Fungsi buat nulis log ke file log
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

// Untuk cek apakah file yang dicek itu file biasa (bukan folder, dll)
int is_regular_file(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

// Untuk cek dan download zip starter kit kalau belum ada, terus extract
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

// Untuk cek apakah string itu valid base64
int is_base64_string(const char *str) {
    size_t len = strlen(str);
    if (len % 4 != 0) return 0;

    for (size_t i = 0; i < len; i++) {
        if (!(isalnum(str[i]) || str[i] == '+' || str[i] == '/' || str[i] == '=')) {
            return 0;
        }
    }
    return 1;
}

// Fungsi utama untuk memindahkan file dari satu folder ke folder lain, dan decode jika di enkripsi
void move_file(const char *src_dir, const char *dst_dir, int decode_name, const char *mode) {
    DIR *dir = opendir(src_dir);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char src_path[512], dst_path[512];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
        if (!is_regular_file(src_path)) continue;

        const char *final_name = entry->d_name;
        char *decoded = NULL;

        if (decode_name) {
            if (is_base64_string(entry->d_name)) {
                size_t out_len;
                decoded = decode_base64(entry->d_name, &out_len);
                if (decoded) {
                    // Bersihkan trailing newline/whitespace
                    int len = strlen(decoded);
                    while (len > 0 && (decoded[len - 1] == '\n' || decoded[len - 1] == '\r' ||
                                       decoded[len - 1] == ' '  || decoded[len - 1] == '\t')) {
                        decoded[--len] = '\0';
                    }
        
                    if (is_printable_string(decoded)) {
                        final_name = decoded;
                    } else {
                        free(decoded);
                        decoded = NULL;
                    }
                }
                // kalau gagal decode atau is_printable_string false, pakai nama asli (final_name tetap entry->d_name)
            }
            // kalau bukan base64, final_name tetap original name (entry->d_name)
        }        

        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, final_name);
        rename(src_path, dst_path);

        char logmsg[512];
        snprintf(logmsg, sizeof(logmsg), "%s - Successfully %s %s directory.",
                 final_name, mode, strcmp(mode, "returned") == 0 ? "to starter kit" : "to quarantine");
        write_log(logmsg);

        if (decoded) free(decoded);
    }
    closedir(dir);
}

// Hapus semua file yang ada di dalam folder tertentu
void delete_files(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
        if (is_regular_file(filepath)) {
            remove(filepath);
            char logmsg[512];
            snprintf(logmsg, sizeof(logmsg), "%s - Successfully deleted.", entry->d_name);
            write_log(logmsg);
        }
    }
    closedir(dir);
}

// Fungsi ini untuk menjalankan proses daemon, yang jalan di background
// Daemon ini nanti aktif terus sambil menunggu tugas, dan bisa dimatikan pakai shutdown
void decrypt_daemon() {
    pid_t pid = fork();
    if (pid > 0) {
        exit(0);
    } else if (pid == 0) {
        setsid();

        char msg[128];
        snprintf(msg, sizeof(msg), "Successfully started decryption process with PID %d.", getpid());
        write_log(msg);

        chdir("/");
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);

        mkdir(QUARANTINE_FOLDER, 0755);

        while (1) {
            sleep(10);
        }
    }
}

// Fungsi buat matiin proses daemon dekripsi (yang jalan di background)
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

// Fungsi error handling
void print_usage(const char *invalid) {
    if (invalid) {
        fprintf(stderr, "Error: Perintah '%s' tidak dikenali.\n", invalid);
    }
    fprintf(stderr, "Gunakan salah satu perintah berikut:\n");
    fprintf(stderr, "  --decrypt     : Menyediakan 'quarantine' (daemon).\n");
    fprintf(stderr, "  --quarantine  : Mendeskripsikan file terenkripsi.\n");
    fprintf(stderr, "  --return      : Memindahkan file dari quarantine ke starter_kit.\n");
    fprintf(stderr, "  --eradicate   : Menghapus semua file di folder quarantine.\n");
    fprintf(stderr, "  --shutdown    : Mematikan proses dekripsi (daemon).\n");
}

// Fungsi utama (main) untuk menentukan perintah yang dijalankan user
int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_usage(NULL);
        return 1;
    }

    ensure_starter_kit_downloaded();

    if (strcmp(argv[1], "--decrypt") == 0) {
        mkdir(QUARANTINE_FOLDER, 0755);
        decrypt_daemon();
    } else if (strcmp(argv[1], "--quarantine") == 0) {
        mkdir(QUARANTINE_FOLDER, 0755);
        move_file(STARTER_KIT_FOLDER, QUARANTINE_FOLDER, 1, "moved to quarantine");
    } else if (strcmp(argv[1], "--return") == 0) {
        move_file(QUARANTINE_FOLDER, STARTER_KIT_FOLDER, 0, "returned");
    } else if (strcmp(argv[1], "--eradicate") == 0) {
        delete_files(QUARANTINE_FOLDER);
    } else if (strcmp(argv[1], "--shutdown") == 0) {
        shutdown_daemon();
    } else {
        print_usage(argv[1]);
        return 1;
    }

    return 0;
}
