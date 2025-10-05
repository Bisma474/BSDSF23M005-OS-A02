/*
 * ls-v2.0.0 — Feature-4 (adds -x flag for horizontal display)
 * - Default: column display (down then across)
 * - With -x option: horizontal display (across then down)
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
#include <sys/ioctl.h>
#include <limits.h>

/* prototypes */
void list_simple(const char *path, int horizontal);
void list_long(const char *path);
void print_file_info(const char *name, const char *path);

/* helpers */
char **read_dir_all(const char *path, int *out_count, int *out_maxlen, int show_hidden) {
    DIR *dir = opendir(path);
    if (!dir) { perror("opendir"); *out_count = 0; *out_maxlen = 0; return NULL; }

    int cap = 64;
    char **names = malloc(cap * sizeof(char *));
    if (!names) { closedir(dir); perror("malloc"); return NULL; }

    int n = 0, maxlen = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; /* skip hidden */
        if (n >= cap) {
            cap *= 2;
            char **tmp = realloc(names, cap * sizeof(char *));
            if (!tmp) { perror("realloc"); break; }
            names = tmp;
        }
        names[n] = strdup(entry->d_name);
        if (!names[n]) { perror("strdup"); break; }
        int L = (int)strlen(entry->d_name);
        if (L > maxlen) maxlen = L;
        n++;
    }
    closedir(dir);
    *out_count = n;
    *out_maxlen = maxlen;
    return names;
}

void free_names(char **names, int n) {
    if (!names) return;
    for (int i = 0; i < n; ++i) free(names[i]);
    free(names);
}

int get_term_width(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) return w.ws_col;
    return 80;
}

/* ----- Display modes ----- */

/* Down-then-across (Feature-3) */
void display_down_across(char **names, int n, int maxlen) {
    if (n <= 0) { printf("\n"); return; }
    int term_w = get_term_width();
    int col_w = maxlen + 2;
    int cols = term_w / col_w;
    if (cols < 1) cols = 1;
    int rows = (n + cols - 1) / cols;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int idx = c * rows + r;
            if (idx < n)
                printf("%-*s", col_w, names[idx]);
        }
        printf("\n");
    }
}

/* Across-then-down (Feature-4) */
void display_across_then_down(char **names, int n, int maxlen) {
    if (n <= 0) { printf("\n"); return; }
    int term_w = get_term_width();
    int col_w = maxlen + 2;
    int cols = term_w / col_w;
    if (cols < 1) cols = 1;

    for (int i = 0; i < n; i++) {
        printf("%-*s", col_w, names[i]);
        if ((i + 1) % cols == 0)
            printf("\n");
    }
    if (n % cols != 0)
        printf("\n");
}

/* --------------------------- */

int main(int argc, char *argv[]) {
    int opt;
    int long_listing = 0;
    int horizontal = 0;
    char *dir_path = ".";

    while ((opt = getopt(argc, argv, "lx")) != -1) {
        switch (opt) {
            case 'l': long_listing = 1; break;
            case 'x': horizontal = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-x] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc)
        dir_path = argv[optind];

    if (long_listing)
        list_long(dir_path);
    else
        list_simple(dir_path, horizontal);

    return 0;
}

void list_simple(const char *path, int horizontal) {
    int n = 0, maxlen = 0;
    char **names = read_dir_all(path, &n, &maxlen, 0);
    if (!names) return;

    if (horizontal)
        display_across_then_down(names, n, maxlen);
    else
        display_down_across(names, n, maxlen);

    free_names(names, n);
}

/* long listing unchanged */
void list_long(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) { perror("opendir"); return; }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        print_file_info(entry->d_name, path);
    }
    closedir(dir);
}

void print_file_info(const char *name, const char *path) {
    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

    struct stat st;
    if (lstat(fullpath, &st) == -1) { perror("lstat"); return; }

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

    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);

    char timebuf[64];
    strncpy(timebuf, ctime(&st.st_mtime), sizeof(timebuf));
    timebuf[strlen(timebuf) - 1] = '\0';

    printf("%s %3ld %s %s %8ld %s %s\n",
           perms,
           (long)st.st_nlink,
           pw ? pw->pw_name : "?",
           gr ? gr->gr_name : "?",
           (long)st.st_size,
           timebuf,
           name);
}
