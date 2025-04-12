#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>

#define LOG_FILE "debugmon.log" // this is where the log are written
#define MAX_PROCESSES 1024
#define MAX_LINE_LEN 1024

typedef struct {
    pid_t pid;
    char command[MAX_LINE_LEN];
    float cpu_usage;
    float memory_usage;
} ProcessInfo;

//menulis log ke file daemonlog
// format log: [dd:mm:yyyy]-[hh:mm:ss]_[process_name]_STATUS(status)
void write_log(const char *process_name, const char *status) {
    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    
    char timestamp[20];
    strftime(timestamp, 20, "[%d:%m:%Y]-[%H:%M:%S]", tm_info);
    
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        fprintf(log, "%s_%s_STATUS(%s)\n", timestamp, process_name, status);
        fclose(log);
    }
}

//execute a command and return its exit status
//this function uses fork and exec to run the command in a child process
int execute_command(const char *cmd, char *const args[]) {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    
    if (pid == 0) {
        // Child process
        execvp(cmd, args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

//get processes for a specific user and fill the ProcessInfo array
//this function reads /proc/[pid]/status and /proc/[pid]/stat files to get process info
int get_user_processes(const char *username, ProcessInfo *processes, int *count) {
    DIR *dir;
    struct dirent *entry;
    char path[MAX_LINE_LEN];
    char line[MAX_LINE_LEN];
    FILE *fp;
    uid_t uid = -1;
    
    // Get UID from username using getpwnam
    struct passwd *pwd = getpwnam(username);
    if (!pwd) {
        fprintf(stderr, "User %s not found\n", username);
        return -1;
    }
    uid = pwd->pw_uid;
    
    *count = 0;
    
    dir = opendir("/proc");
    if (!dir) {
        perror("opendir");
        return -1;
    }
    
    while ((entry = readdir(dir)) != NULL && *count < MAX_PROCESSES) {
        if (entry->d_type != DT_DIR) continue;
        
        char *endptr;
        pid_t pid = strtol(entry->d_name, &endptr, 10);
        if (*endptr != '\0') continue; // Not a PID
        
        snprintf(path, sizeof(path), "/proc/%d/status", pid);
        fp = fopen(path, "r");
        if (!fp) continue;
        
        uid_t process_uid = -1;
        char name[MAX_LINE_LEN] = "";
        
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "Uid:", 4) == 0) {
                sscanf(line + 4, "%u", &process_uid);
            } else if (strncmp(line, "Name:", 5) == 0) {
                strncpy(name, line + 6, sizeof(name));
                char *newline = strchr(name, '\n');
                if (newline) *newline = '\0';
            }
        }
        fclose(fp);
        
        if (process_uid == uid) {
            processes[*count].pid = pid;
            strncpy(processes[*count].command, name, MAX_LINE_LEN);
            (*count)++;
        }
    }
    
    closedir(dir);
    return 0;
}

// List processes for a specific user and print them
// This function formats the output in a table-like structure
void list_processes(const char *username) {
    ProcessInfo processes[MAX_PROCESSES];
    int count = 0;
    
    if (get_user_processes(username, processes, &count) == 0) {
        printf("%-10s %-30s %-10s %-10s\n", "PID", "COMMAND", "CPU %", "MEMORY (MB)");
        for (int i = 0; i < count; i++) {
            printf("%-10d %-30s %-10.2f %-10.2f\n", 
                   processes[i].pid, 
                   processes[i].command, 
                   processes[i].cpu_usage,
                   processes[i].memory_usage);
        }
    }
}

// Function to run in daemon mode
// This function forks the process and runs in the background, monitoring user processes
void daemon_mode(const char *username) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0) {
        // Parent exits
        write_log("debugmon", "DAEMON_STARTED");
        exit(EXIT_SUCCESS);
    }
    
    // Child becomes daemon
    umask(0);
    setsid();
    
    // Change working directory
    if ((chdir("/")) < 0) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }
    
    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Main daemon loop
    while (1) {
        ProcessInfo processes[MAX_PROCESSES];
        int count = 0;
        
        if (get_user_processes(username, processes, &count) == 0) {
            for (int i = 0; i < count; i++) {
                write_log(processes[i].command, "RUNNING");
            }
        }
        
        sleep(5); // Check every 5 seconds
    }
}

//function to stop monitoring
//this function kills the daemon process and logs the action
void stop_monitoring(const char *username) {
    //find and kill the daemon process
    char *const args[] = {"pkill", "-f", "debugmon daemon", NULL};
    if (execute_command("pkill", args) == 0) {
        write_log("debugmon", "STOPPED");
        printf("Monitoring stopped for user %s\n", username);
    } else {
        fprintf(stderr, "Failed to stop monitoring for user %s\n", username);
    }
}

//function to fail processes for a specific user
//this function kills all processes for the user and blocks new processes from starting
void fail_processes(const char *username) {
    //Kill all user processes
    const char *kill_args[] = {"pkill", "-9", "-u", username, NULL};
    if (execute_command("pkill", kill_args) != 0) {
        fprintf(stderr, "Failed to kill processes for user %s\n", username);
    }

    //block new processes
    char *block_args[] = {"usermod", "-L", username, NULL};
    if (execute_command("usermod", block_args) != 0) {
        fprintf(stderr, "Failed to lock account for user %s\n", username);
    }
    
    //write log to file
    write_log("debugmon", "USER_BLOCKED");
    printf("All processes for user %s have been terminated\n", username);
    printf("User %s is now blocked from running new processes\n", username);
}

//function to revert processes for a specific user
//this function unblocks the user account and unfreezes the cgroup
//it also logs the action
void revert_processes(const char *username) {
    // Unblock user account
    char *const unlock_args[] = {"usermod", "-U", username, NULL};
    if (execute_command("usermod", unlock_args) != 0) {
        fprintf(stderr, "Failed to unlock account for user %s\n", username);
        write_log("debugmon", "REVERT_FAILED");
        return;  // Early return on failure
    }

    write_log("debugmon", "USER_UNBLOCKED");
    printf("User %s has been unblocked\n", username);
    printf("New processes can now be started\n");
}
   
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <command> <user>\n", argv[0]);
        fprintf(stderr, "Commands: list, daemon, stop, fail, revert\n");
        return EXIT_FAILURE;
    }
    
    const char *command = argv[1];
    const char *user = argv[2];
    
    if (strcmp(command, "list") == 0) {
        list_processes(user);
    } else if (strcmp(command, "daemon") == 0) {
        daemon_mode(user);
    } else if (strcmp(command, "stop") == 0) {
        stop_monitoring(user);
    } else if (strcmp(command, "fail") == 0) {
        fail_processes(user);
    } else if (strcmp(command, "revert") == 0) {
        revert_processes(user);
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}