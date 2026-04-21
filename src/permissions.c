// permissions.c
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include "permissions.h"

int check_permissions(const char *path, mode_t expected) {
    struct stat st;
    if (stat(path, &st) < 0) {
        perror("stat");
        return 0;
    }
    // Comparăm doar cei 9 biți de permisiune (maschează tipul fișierului)
    mode_t actual = st.st_mode & 0777;
    return actual == expected;
}

int set_permissions(const char *path, mode_t mode) {
    if (chmod(path, mode) < 0) {
        perror("chmod");
        return -1;
    }
    return 0;
}

// Construim string-ul bit cu bit
void mode_to_string(mode_t mode, char *out) {
    out[0] = (mode & S_IRUSR) ? 'r' : '-';
    out[1] = (mode & S_IWUSR) ? 'w' : '-';
    out[2] = (mode & S_IXUSR) ? 'x' : '-';
    out[3] = (mode & S_IRGRP) ? 'r' : '-';
    out[4] = (mode & S_IWGRP) ? 'w' : '-';
    out[5] = (mode & S_IXGRP) ? 'x' : '-';
    out[6] = (mode & S_IROTH) ? 'r' : '-';
    out[7] = (mode & S_IWOTH) ? 'w' : '-';
    out[8] = (mode & S_IXOTH) ? 'x' : '-';
    out[9] = '\0';
}
