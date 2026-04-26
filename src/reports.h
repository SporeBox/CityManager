#ifndef REPORTS_H
#define REPORTS_H

#include "structs.h"

int init_district(const char *district);

int add_report(const char *district, const char *role, const char *user);
int list_reports(const char *district, const char *role, const char *user);
int view_report(const char *district, const char *role, const char *user, int id);
int remove_report(const char *district, const char *role, const char *user, int id);
int update_threshold(const char *district, const char *role, const char *user, int value);

#endif