/*
 * Programming Assignment 02: lsv2.0.0
 * Features:
 *  - Basic directory listing
 *  - Supports -l flag for long listing (like ls -l)
 *
 * Usage:
 *   $ ./ls-v2.0.0              -> simple list
 *   $ ./ls-v2.0.0 -l           -> long listing
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

void list_simple(const char *path);
void list_long(const char *path);
void print_file_info(const char *name, const char *path);

int main(int argc, char *argv[]) {
    int opt;
    int long_listing = 0;
    char *dir_path = ".";  // default to current directory

    // --- Task 2: Argument parsing using getopt() ---
    while ((opt = getopt(argc, argv, "l")) != -1) {
        switch (opt) {
            case 'l':
                long_listing = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // If a directory name was passed, use it
    if (optind < argc) {
        dir_path = argv[optind];
    }

    // Branch based on -l flag
    if (long_listing)
        list_long(dir_path);
    else
        list_simple(dir_path);

    return 0;
}

// --- Task 3.1: Simple listing ---
void list_simple(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files
        if (entry->d_name[0] == '.')
            continue;
        printf("%s  ", entry->d_name);
    }
    printf("\n");
    closedir(dir);
}

// --- Task 3.2: Long listing ---
void list_long(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;
        print_file_info(entry->d_name, path);
    }
    closedir(dir);
}

// --- Task 3.3: File details using stat(), getpwuid(), getgrgid(), ctime() ---
void print_file_info(const char *name, const char *path) {
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

    struct stat st;
    if (lstat(fullpath, &st) == -1) {
        perror("lstat");
        return;
    }

    // --- File type and permissions ---
    char perms[11];
    perms[0] = S_ISDIR(st.st_mode) ? 'd' :
               S_ISLNK(st.st_mode) ? 'l' :
               S_ISCHR(st.st_mode) ? 'c' :
               S_ISBLK(st.st_mode) ? 'b' :
               S_ISFIFO(st.st_mode) ? 'p' :
               S_ISSOCK(st.st_mode) ? 's' : '-';

    perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';

    // --- Owner and group ---
    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    char *owner = pw ? pw->pw_name : "?";
    char *group = gr ? gr->gr_name : "?";

    // --- Modification time ---
    char timebuf[64];
    strncpy(timebuf, ctime(&st.st_mtime), sizeof(timebuf));
    timebuf[strlen(timebuf) - 1] = '\0';  // remove newline

    // --- Print result like ls -l ---
    printf("%s %3ld %s %s %8ld %s %s\n",
           perms,
           (long)st.st_nlink,
           owner,
           group,
           (long)st.st_size,
           timebuf,
           name);
}
