#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHUNK 100

typedef struct{
    float latitude;
    float longitude;
}Coord;

typedef stuct{
    int day;
    int month;
    int year;
    int hour;
    int minute;
}time_t;

typedef struct{
    int rapID;
    char inspName[30];
    Coord coordinates;
    char issue[30];
    int sevLvl;
    time_t time;
    char description[1000];
}Raport;

void checkCredentials(int argc, char* argv[3]){
    if(argc < 5){
        char temp[] = "ERROR: not enough information provided\n";
        write(2, temp, strlen(temp));
        return;
    }
    if(strcmp(argv[1], "--role") != 0){
        char *temp = malloc(CHUNK * sizeof(char));
        strcpy(temp, "ERROR: unknown command: ");
        if(strlen(temp) + strlen(argv[1]) > CHUNK) temp = realloc(temp, strlen(argv[1]) + CHUNK);
        strcat(temp, argv[1]);
        strcat(temp, "\n");
        write(2, temp, strlen(temp));
        free(temp);
        return;
    }

    if(strcmp(argv[2], "inspector") != 0 && strcmp(argv[2], "manager") != 0){
        char *temp = malloc(CHUNK * sizeof(char));
        strcpy(temp, "ERROR: unknown role: ");
        if(strlen(temp) + strlen(argv[2]) > CHUNK) temp = realloc(temp, strlen(argv[1]) + CHUNK);
        strcat(temp, argv[2]);
        strcat(temp, "\n");
        write(2, temp, strlen(temp));
        free(temp);
        return;
    }

    if(strcmp(argv[3], "--user") != 0){
        char *temp = malloc(CHUNK * sizeof(char));
        strcpy(temp, "ERROR: unknown command: ");
        if(strlen(temp) + strlen(argv[1]) > CHUNK) temp = realloc(temp, strlen(argv[1]) + CHUNK);
        strcat(temp, argv[1]);
        strcat(temp, "\n");
        write(2, temp, strlen(temp));
        free(temp);
        return;
    }
}

int add(int argc, char* argv[7]){
    Raport raport;
    strcpy(raport.rapID, argv[6]);
    strcpy(raport.inspName, argv[4]);



}

int main(int argc, char **argv){

    checkCredentials(argc, argv);

    return 0;
}
