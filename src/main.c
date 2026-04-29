// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "reports.h"
#include "filter.h"
#include "permissions.h"
#include "log.h"

int parse_args(int argc, char **argv,
               char **role, char **user, char **command, char **args_out[]) {
    if (argc < 6) {
        write(STDERR_FILENO, "ERROR: not enough arguments\n", 28);
        return 0;
    }

    if (strcmp(argv[1], "--role") != 0) {
        write(STDERR_FILENO, "ERROR: expected --role\n", 23);
        return 0;
    }
    *role = argv[2];

    if (strcmp(*role, "inspector") != 0 && strcmp(*role, "manager") != 0) {
        write(STDERR_FILENO, "ERROR: unknown role\n", 20);
        return 0;
    }

    if (strcmp(argv[3], "--user") != 0) {
        write(STDERR_FILENO, "ERROR: expected --user\n", 23);
        return 0;
    }
    *user = argv[4];

    *command = argv[5];

    *args_out = &argv[6];

    return 1;
}

int main(int argc, char **argv) {
    char *role, *user, *command;
    char **extra_args;

    if (!parse_args(argc, argv, &role, &user, &command, &extra_args))
        return 1;

    int extra_count = argc - 6;

    if (strcmp(command, "--add") == 0) {
        if (extra_count < 1) {
            write(STDERR_FILENO, "ERROR: --add requires <district>\n", 33);
            return 1;
        }
        return add_report(extra_args[0], role, user);

    } else if (strcmp(command, "--list") == 0) {
        if (extra_count < 1) {
            write(STDERR_FILENO, "ERROR: --list requires <district>\n", 34);
            return 1;
        }
        return list_reports(extra_args[0], role, user);

    } else if (strcmp(command, "--view") == 0) {
        if (extra_count < 2) {
            write(STDERR_FILENO, "ERROR: --view requires <district> <report_id>\n", 46);
            return 1;
        }
        int id = atoi(extra_args[1]);
        return view_report(extra_args[0], role, user, id);

    } else if (strcmp(command, "--remove_report") == 0) {
        if (extra_count < 2) {
            write(STDERR_FILENO, "ERROR: --remove_report requires <district> <report_id>\n", 56);
            return 1;
        }
        int id = atoi(extra_args[1]);
        return remove_report(extra_args[0], role, user, id);

    } else if (strcmp(command, "--update_threshold") == 0) {
        if (extra_count < 2) {
            write(STDERR_FILENO, "ERROR: --update_threshold requires <district> <value>\n", 55);
            return 1;
        }
        int val = atoi(extra_args[1]);
        return update_threshold(extra_args[0], role, user, val);

    } else if (strcmp(command, "--filter") == 0) {
        if (extra_count < 2) {
            write(STDERR_FILENO, "ERROR: --filter requires <district> <condition...>\n", 51);
            return 1;
        }
        return filter_reports(extra_args[0], role, user,
                              (const char **)&extra_args[1], extra_count - 1);
    } else if (strcmp(command, "--remove_district") == 0){
		if(extra_count < 1) {
			write(STDERR_FILENO, "ERROR: --remove_district requires <district>\n", 51);
            return 1;
		}
		return remove_district(role, extra_args[0]);
	}else {
        write(STDERR_FILENO, "ERROR: unknown command\n", 23);
        return 1;
    }
}