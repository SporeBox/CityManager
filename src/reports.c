#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "reports.h"
#include "permissions.h"
#include "log.h"
#include "structs.h"

static void build_path(char *buf, size_t size,
                       const char *district, const char *file) {
    snprintf(buf, size, "%s/%s", district, file);
}

int init_district(const char *district) {
    struct stat st;


    if (stat(district, &st) < 0) {
        if (mkdir(district, 0750) < 0) {
            perror("mkdir");
            return -1;
        }
    }

    chmod(district, 0750);

    char path[512];


    build_path(path, sizeof(path), district, "reports.dat");
    if (stat(path, &st) < 0) {
        int fd = open(path, O_CREAT | O_WRONLY, 0664);
        if (fd < 0) { perror("open reports.dat"); return -1; }
        close(fd);
        chmod(path, 0664);
    }


    build_path(path, sizeof(path), district, "district.cfg");
    if (stat(path, &st) < 0) {
        int fd = open(path, O_CREAT | O_WRONLY, 0640);
        if (fd < 0) { perror("open district.cfg"); return -1; }

        write(fd, "threshold=1\n", 12);
        close(fd);
        chmod(path, 0640);
    }


    build_path(path, sizeof(path), district, "logged_district");
    if (stat(path, &st) < 0) {
        int fd = open(path, O_CREAT | O_WRONLY, 0644);
        if (fd < 0) { perror("open logged_district"); return -1; }
        close(fd);
        chmod(path, 0644);
    }


    char link_name[256];
    char target[512];
    snprintf(link_name, sizeof(link_name), "active_reports-%s", district);
    build_path(target, sizeof(target), district, "reports.dat");


    struct stat lst;
    if (lstat(link_name, &lst) < 0) {

        if (symlink(target, link_name) < 0) {
            perror("symlink");

        }
    }


    return 0;
}

int add_report(const char *district, const char *role, const char *user) {

    if (init_district(district) < 0) return -1;

    char path[512];
    build_path(path, sizeof(path), district, "reports.dat");

    if (!check_permissions(path, 0664)) {
        write(STDERR_FILENO,
              "ERROR: reports.dat permissions incorrect\n", 41);
        return -1;
    }


    int fd = open(path, O_RDWR | O_APPEND);
    if (fd < 0) { perror("open reports.dat"); return -1; }


    struct stat st;
    fstat(fd, &st);
    int num_records = st.st_size / sizeof(Report);

    Report r;
    memset(&r, 0, sizeof(Report));
    r.rapID = num_records + 1;
    strncpy(r.inspName, user, sizeof(r.inspName) - 1);


    printf("X (latitude): ");
    scanf("%f", &r.coordinates.latitude);

    printf("Y (longitude): ");
    scanf("%f", &r.coordinates.longitude);

    printf("Category (road/lighting/flooding/other): ");
    scanf("%29s", r.issue);

    printf("Severity level (1/2/3): ");
    scanf("%d", &r.sevLvl);
    if (r.sevLvl < 1 || r.sevLvl > 3) {
        write(STDERR_FILENO, "ERROR: severity must be 1, 2 or 3\n", 35);
        close(fd);
        return -1;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    r.time.day    = t->tm_mday;
    r.time.month  = t->tm_mon + 1;
    r.time.year   = t->tm_year + 1900;
    r.time.hour   = t->tm_hour;
    r.time.minute = t->tm_min;

    int c; while ((c = getchar()) != '\n' && c != EOF);

    printf("Description: ");
    fgets(r.description, sizeof(r.description), stdin);

    r.description[strcspn(r.description, "\n")] = '\0';

    if (write(fd, &r, sizeof(Report)) != sizeof(Report)) {
        perror("write report");
        close(fd);
        return -1;
    }

    close(fd);

    printf("Report #%d added to district '%s'\n", r.rapID, district);
    log_action(district, role, user, "add");
    return 0;
}

int list_reports(const char *district, const char *role, const char *user) {
    char path[512];
    build_path(path, sizeof(path), district, "reports.dat");

    struct stat st;
    if (stat(path, &st) < 0) {
        write(STDERR_FILENO, "ERROR: district not found\n", 26);
        return -1;
    }

    char perm_str[10];
    mode_to_string(st.st_mode & 0777, perm_str);

    char time_buf[64];
    struct tm *mt = localtime(&st.st_mtime);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", mt);

    printf("=== reports.dat | permissions: %s | size: %lld bytes | modified: %s ===\n",
           perm_str, (long long)st.st_size, time_buf);

    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror("open reports.dat"); return -1; }

    Report r;
    int count = 0;

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        count++;
        printf("--------------------\n");
        printf("ID:          %d\n",    r.rapID);
        printf("Inspector:   %s\n",    r.inspName);
        printf("Location:    %.4f, %.4f\n", r.coordinates.latitude,
                                             r.coordinates.longitude);
        printf("Category:    %s\n",    r.issue);
        printf("Severity:    %d\n",    r.sevLvl);
        printf("Date:        %02d/%02d/%04d %02d:%02d\n",
               r.time.day, r.time.month, r.time.year,
               r.time.hour, r.time.minute);
        printf("Description: %s\n",   r.description);
    }

    if (count == 0)
        printf("No reports in district '%s'\n", district);

    close(fd);
    log_action(district, role, user, "list");
    return 0;
}

int view_report(const char *district, const char *role,
                const char *user, int id) {
    char path[512];
    build_path(path, sizeof(path), district, "reports.dat");

    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror("open reports.dat"); return -1; }

    Report r;
    int found = 0;

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.rapID == id) {
            found = 1;
            printf("=== Report #%d ===\n",  r.rapID);
            printf("Inspector:   %s\n",      r.inspName);
            printf("Location:    %.4f, %.4f\n",
                   r.coordinates.latitude, r.coordinates.longitude);
            printf("Category:    %s\n",      r.issue);
            printf("Severity:    %d\n",      r.sevLvl);
            printf("Date:        %02d/%02d/%04d %02d:%02d\n",
                   r.time.day, r.time.month, r.time.year,
                   r.time.hour, r.time.minute);
            printf("Description: %s\n",      r.description);
            break;
        }
    }

    close(fd);

    if (!found) {
        printf("ERROR: report #%d not found in district '%s'\n", id, district);
        return -1;
    }

    log_action(district, role, user, "view");
    return 0;
}

int remove_district(const char *role, const char* districtName) {
    if (strcmp(role, "manager") != 0) {
        write(STDERR_FILENO,
              "ERROR: only manager can remove reports\n", 39);
        return -1;
    }

    char command[100];

    strcpy(command, "rm -rf ");
    strcat(command, districtName);
    if (access(districtName, F_OK) == 0) pid_t delete_dirP = system(command);
    else write(STDERR_FILENO,"The file does not exist!\n", 30);

    strcpy(command, "rm active_reports-");
    strcat(command, districtName);
    system(command);

    write(STDERR_FILENO,"District deleted succesfully\n", 30);

    return 0;
}

int remove_report(const char *district, const char *role,
                  const char *user, int id) {

    if (strcmp(role, "manager") != 0) {
        write(STDERR_FILENO,
              "ERROR: only manager can remove reports\n", 39);
        return -1;
    }

    char path[512];
    build_path(path, sizeof(path), district, "reports.dat");


    int fd = open(path, O_RDWR);
    if (fd < 0) { perror("open reports.dat"); return -1; }

    struct stat st;
    fstat(fd, &st);
    int num_records = st.st_size / sizeof(Report);

    int target_index = -1;
    Report r;

    for (int i = 0; i < num_records; i++) {
        lseek(fd, (off_t)i * sizeof(Report), SEEK_SET);
        read(fd, &r, sizeof(Report));
        if (r.rapID == id) {
            target_index = i;
            break;
        }
    }

    if (target_index < 0) {
        printf("ERROR: report #%d not found\n", id);
        close(fd);
        return -1;
    }

    for (int i = target_index + 1; i < num_records; i++) {
        lseek(fd, (off_t)i * sizeof(Report), SEEK_SET);
        read(fd, &r, sizeof(Report));

        lseek(fd, (off_t)(i - 1) * sizeof(Report), SEEK_SET);
        write(fd, &r, sizeof(Report));
    }

    off_t new_size = (off_t)(num_records - 1) * sizeof(Report);
    ftruncate(fd, new_size);

    close(fd);

    printf("Report #%d removed from district '%s'\n", id, district);
    log_action(district, role, user, "remove_report");
    return 0;
}

int update_threshold(const char *district, const char *role,
                     const char *user, int value) {
    if (strcmp(role, "manager") != 0) {
        write(STDERR_FILENO,
              "ERROR: only manager can update threshold\n", 41);
        return -1;
    }

    char path[512];
    build_path(path, sizeof(path), district, "district.cfg");

    if (!check_permissions(path, 0640)) {
        write(STDERR_FILENO,
              "ERROR: district.cfg permissions have been altered, refusing write\n",
              67);
        return -1;
    }

    int fd = open(path, O_WRONLY | O_TRUNC);
    if (fd < 0) { perror("open district.cfg"); return -1; }

    char buf[64];
    int len = snprintf(buf, sizeof(buf), "threshold=%d\n", value);
    write(fd, buf, len);
    close(fd);

    printf("Threshold updated to %d in district '%s'\n", value, district);
    log_action(district, role, user, "update_threshold");
    return 0;
}
