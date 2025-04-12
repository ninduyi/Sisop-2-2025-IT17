#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>

#define MAX_PATH 512

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

int is_valid_file(const char *name) {
    return strlen(name) == 5 && isalnum(name[0]) && name[1] == '.' && name[2] == 't' && name[3] == 'x' && name[4] == 't';
}

int is_single_char_filename(const char *name) {
    return strlen(name) == 5 &&
           ((isdigit(name[0]) || isalpha(name[0])) && isalnum(name[0])) &&
           name[1] == '.' && strcmp(name + 1, ".txt") == 0;
}

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

