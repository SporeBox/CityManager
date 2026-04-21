#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "log.h"
#include "permissions.h"

void log_action(const char *district, const char *role,
                const char *user,    const char *action) {
    char path[256];
    snprintf(path, sizeof(path), "%s/logged_district", district);

    // Verificăm: inspectorii nu au voie să scrie în log (644 → doar owner scrie)
    // În simularea noastră, managerul e owner → inspector nu poate scrie
    // Dar din moment ce rulăm cu același user Unix, verificăm prin convenție de rol
    if (strcmp(role, "inspector") == 0) {
        // Inspectorul nu scrie în log — managerul o face în numele sistemului
        // Totuși, cerința spune că log-ul înregistrează ORICE acțiune
        // Alegem să scriem întotdeauna, dar verificăm permisiunile formal
    }

    // Deschidem în append
    int fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) { perror("log open"); return; }
    set_permissions(path, 0644);

    // Obținem timestamp-ul sistemului
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char buf[512];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s\t%s\n",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec,
             user, role, action);

    write(fd, buf, strlen(buf));
    close(fd);
}