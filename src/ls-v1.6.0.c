#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>

#define COLOR_DIR  "\033[1;34m"
#define COLOR_EXE  "\033[1;32m"
#define COLOR_LINK "\033[1;36m"
#define COLOR_RESET "\033[0m"

int long_format = 0;
int horizontal_format = 0;
int recursive_flag = 0;

void print_permissions(mode_t mode) {
    printf( (S_ISDIR(mode)) ? "d" : (S_ISLNK(mode)) ? "l" : "-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

void print_colorized_name(struct stat *st, const char *name) {
    if (S_ISDIR(st->st_mode))
        printf(COLOR_DIR "%s" COLOR_RESET, name);
    else if (S_ISLNK(st->st_mode))
        printf(COLOR_LINK "%s" COLOR_RESET, name);
    else if (st->st_mode & S_IXUSR)
        printf(COLOR_EXE "%s" COLOR_RESET, name);
    else
        printf("%s", name);
}

void list_directory(const char *path);

void print_entry(const char *dir_path, const char *name) {
    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dir_path, name);

    struct stat st;
    if (lstat(fullpath, &st) == -1) {
        perror("stat");
        return;
    }

    if (long_format) {
        print_permissions(st.st_mode);
        printf(" %ld ", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);
        printf("%s %s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");
        printf("%5ld ", st.st_size);

        char timebuf[80];
        struct tm *tm = localtime(&st.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
        printf("%s ", timebuf);

        print_colorized_name(&st, name);
        printf("\n");
    } else {
        print_colorized_name(&st, name);
        if (horizontal_format)
            printf("  ");
        else
            printf("\n");
    }
}

void list_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    printf("\n%s:\n", path);

    char **subdirs = NULL;
    int subdir_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        print_entry(path, entry->d_name);

        if (recursive_flag) {
            char fullpath[PATH_MAX];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

            struct stat st;
            if (lstat(fullpath, &st) == 0 && S_ISDIR(st.st_mode)) {
                subdirs = realloc(subdirs, sizeof(char*) * (subdir_count + 1));
                subdirs[subdir_count] = strdup(fullpath);
                subdir_count++;
            }
        }
    }

    closedir(dir);

    if (recursive_flag) {
        for (int i = 0; i < subdir_count; i++) {
            list_directory(subdirs[i]);
            free(subdirs[i]);
        }
        free(subdirs);
    }
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "lxR")) != -1) {
        switch (opt) {
            case 'l': long_format = 1; break;
            case 'x': horizontal_format = 1; break;
            case 'R': recursive_flag = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-x] [-R] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";

    list_directory(path);

    return 0;
}
