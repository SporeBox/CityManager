#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "filter.h"
#include "structs.h"
#include "log.h"
int parse_condition(const char *input, char *field, char *op, char *value) {
    // Copiem input-ul ca să nu modificăm originalul
    char buf[256];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    // Primul token: field
    char *token = strtok(buf, ":");
    if (!token) return 0;
    strncpy(field, token, 31);

    // Al doilea token: operator
    token = strtok(NULL, ":");
    if (!token) return 0;
    strncpy(op, token, 4);

    // Al treilea token: value
    token = strtok(NULL, ":");
    if (!token) return 0;
    strncpy(value, token, 63);

    return 1;
}

int match_condition(Report *r, const char *field,
                    const char *op, const char *value) {
    if (strcmp(field, "severity") == 0) {
        int val = atoi(value);
        int sev = r->sevLvl;
        if (strcmp(op, "==") == 0) return sev == val;
        if (strcmp(op, "!=") == 0) return sev != val;
        if (strcmp(op, "<")  == 0) return sev <  val;
        if (strcmp(op, "<=") == 0) return sev <= val;
        if (strcmp(op, ">")  == 0) return sev >  val;
        if (strcmp(op, ">=") == 0) return sev >= val;

    } else if (strcmp(field, "category") == 0) {
        // Pentru string-uri, singurii operatori cu sens sunt == și !=
        int cmp = strcmp(r->issue, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;

    } else if (strcmp(field, "inspector") == 0) {
        int cmp = strcmp(r->inspName, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;

    } else if (strcmp(field, "timestamp") == 0) {
        // Comparăm anul — poți extinde la dată completă
        int val = atoi(value);
        int year = r->time.year;
        if (strcmp(op, "==") == 0) return year == val;
        if (strcmp(op, "!=") == 0) return year != val;
        if (strcmp(op, "<")  == 0) return year <  val;
        if (strcmp(op, "<=") == 0) return year <= val;
        if (strcmp(op, ">")  == 0) return year >  val;
        if (strcmp(op, ">=") == 0) return year >= val;
    }

    return 0;
}


int filter_reports(const char *district, const char *role,
                   const char *user, const char **conditions, int num_conditions) {
    char path[512];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open reports.dat");
        return -1;
    }


    char fields[10][32];
    char ops[10][5];
    char values[10][64];

    for (int i = 0; i < num_conditions; i++) {
        if (!parse_condition(conditions[i], fields[i], ops[i], values[i])) {
            printf("ERROR: invalid condition: %s\n", conditions[i]);
            close(fd);
            return -1;
        }
    }

    Report r;
    int found = 0;


    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        int match = 1;


        for (int i = 0; i < num_conditions; i++) {
            if (!match_condition(&r, fields[i], ops[i], values[i])) {
                match = 0;
                break;
            }
        }

        if (match) {
            found++;
            printf("--------------------\n");
            printf("ID:          %d\n",   r.rapID);
            printf("Inspector:   %s\n",   r.inspName);
            printf("Location:    %.4f, %.4f\n",
                   r.coordinates.latitude, r.coordinates.longitude);
            printf("Category:    %s\n",   r.issue);
            printf("Severity:    %d\n",   r.sevLvl);
            printf("Date:        %02d/%02d/%04d %02d:%02d\n",
                   r.time.day, r.time.month, r.time.year,
                   r.time.hour, r.time.minute);
            printf("Description: %s\n",  r.description);
        }
    }

    close(fd);

    if (found == 0)
        printf("No reports match the given conditions.\n");
    else
        printf("--------------------\n%d report(s) found.\n", found);

    log_action(district, role, user, "filter");
    return 0;
}