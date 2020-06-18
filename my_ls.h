//
// Created by USer on 15.05.2020.
//
#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#ifndef NEW_CASES_H
#define NEW_CASES_H
struct Cases 
{
    bool case_a;
    bool case_R;
    bool case_t;
    bool case_err;
    bool path;
    int dcount;
    int fcount;
    char** files;
};
void clear(struct Cases* args);
struct Cases myGetopt(int argc, char **argv);
int scanDir(char* namedir, struct Cases cases);
char** timeSort(char** files, int size, char* namedir);
char** lexicoSort(char** files, int count);
#endif //NEW_CASES_H
