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

    if (strcmp(role, "inspector") == 0) {

    }

    int fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) { perror("log open"); return; }
    set_permissions(path, 0644);

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