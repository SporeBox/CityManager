#ifndef FILTER_H
#define FILTER_H

int filter_reports(const char *district, const char *role,
                   const char *user, const char **conditions, int num_conditions);

#endif