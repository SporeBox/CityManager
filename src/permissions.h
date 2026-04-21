#ifndef PERMISSIONS_H
#define PERMISSIONS_H

// Întoarce 1 dacă fișierul are exact permisiunile cerute, 0 altfel
int check_permissions(const char *path, mode_t expected);

// Setează permisiunile unui fișier
int set_permissions(const char *path, mode_t mode);

// Convertește biții de permisiune în string: "rw-rw-r--"
void mode_to_string(mode_t mode, char *out); // out trebuie să fie cel puțin 10 bytes

#endif
