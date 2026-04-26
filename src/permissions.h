#ifndef PERMISSIONS_H
#define PERMISSIONS_H

int check_permissions(const char *path, mode_t expected);

int set_permissions(const char *path, mode_t mode);

void mode_to_string(mode_t mode, char *out);

#endif
