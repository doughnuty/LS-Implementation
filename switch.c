#include <sys/stat.h>
#include <limits.h>
#include "my_ls.h"
#include <errno.h>
extern int errno;
void clear(struct Cases* args){
    args->case_a = false;
    args->case_R = false;
    args->case_t = false;
    args->case_err = false;
    args->name = ".";
}
size_t getLength(const char *str){
    size_t length = 0;
    while (*str++) length++;
    return length;
}
char* setName(const char* name, const char* path){
    int pathLen = getLength(path), nameLen = getLength(name);
    char* ans = (char*)malloc(sizeof(char)*(pathLen+nameLen+2));
    for(int i = 0; i < pathLen; i++){
        ans[i] = path[i];
    }
    ans[pathLen] = '/';
    for(int j = 1; j <= nameLen; j++){
        ans[pathLen+j] = name[j-1];
    }
    ans[pathLen+nameLen+1] = '\0';
    return ans;
}
int cmpName(char* name1, char* name2){
    int a = getLength(name1), b = getLength(name2);
    if(b < a || a < b) return 1;
    for(int i = 0; i < a; i++){
        if(name1[i]!=name2[i]) return 1;
    }
    return 0;
}
struct Cases myGetopt(int argc, char **argv){
    struct stat path_stat;
    struct Cases args;  
    clear(&args);
    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-') {
            int length = getLength(argv[i]);
            for(int j = 1; j < length; j++){
                switch (argv[i][j]){
                    case 'R':
                        args.case_R = true;
                        continue;
                    case 'a':
                        args.case_a = true;
                        continue;
                    case 't':
                        args.case_t = true;
                        continue;
                    default:
                        printf("Incorrect argument: %c\n", argv[i][j]);
                        args.case_err = true;
                        return args;
                }
            }
        }
        else {
            stat(argv[i], &path_stat);
            if(S_ISDIR(path_stat.st_mode)) args.name = argv[i];
        }
    }
    return args;
}
char** timeSort(char** files, int start, int count){
    register int i, j;

    char *temp;
    struct stat ibuf;
    struct stat jbuf;
    struct stat xbuf;

    i = start; j = count;

    stat(files[i], &ibuf);
    stat(files[j], &jbuf);
    stat(files[(start+count)/2], &xbuf);

    do {
        while((ibuf.st_mtime < xbuf.st_mtime) && (i < count)) {
            i++;
            stat(files[i], &ibuf);
        }
        while((jbuf.st_mtime > xbuf.st_mtime) && (j > start))  {
            j--;
            stat(files[j], &jbuf);
        }
        if(i <= j) {
            temp = files[i];
            files[i] = files[j];
            files[j] = temp;
            i++; j--;
        }

    } while(i <= j);

    if(start < j) timeSort(files, start, j);
    if(i < count) timeSort(files, i, count);
    return files;
}
char** ForwardSort(char** files, int count){
    char *temp;
    for(int i=0; i < count-1; i++){
        for(int j=0; j < count-1-i; j++){
            if(files[j] != files[j+1]){
                temp = files[j];
                files[j] = files[j+1];
                files[j+1] = temp;
            }
        }
    }
    return files;
}
int displayRecursively(char* namedir, struct Cases cases){
    printf("recursing %s\n", namedir);
    int out = scanDir(namedir, cases);
    if(out != 0) {
        printf("Recursion error on directory %s\n", namedir);
        return 1;
    }
    return 0;
}
int scanDir(char* namedir, struct Cases cases){
    DIR* directory;
    struct dirent* ptr;
    struct stat path_stat;
    int count = 0, dirs = 0, j = 0, out, maxNameSize = 0;
    if(cases.case_R) {
        printf("%s:\n", namedir);
    }
    directory = opendir(namedir);
    if(directory == NULL) {
        printf("NULL");
        return 1;}
    while((ptr = readdir(directory)) != NULL) {
        count++;
        if(getLength(ptr->d_name)>maxNameSize) maxNameSize = getLength(ptr->d_name);
        if(cmpName(ptr->d_name, "..") + cmpName(ptr->d_name, ".") < 2) continue;
        else if((cases.case_a || ptr->d_name[0] != '.') && cases.case_R) {
            char* name = setName(ptr->d_name, namedir);
            int st = stat(name, &path_stat);
            if(st != 0) {
                name = setName(ptr->d_name, "/exp");
                st = stat(name, &path_stat);
            }
            if (S_ISDIR(path_stat.st_mode)) {
                dirs++;
            }
            free(name);
        }
    }
    //printf("Count is %d; Dirs is %d\n", count, dirs);
    closedir(directory);
    char **files = (char**)malloc(sizeof(char*)*(count)), **directories = (char**)malloc(sizeof(char*)*dirs);
    directory = opendir(namedir);
    for(int i = 0; i < count; i++) {
        ptr = readdir(directory);
        if(ptr == NULL) return 1;
        files[i] = ptr->d_name;
        if(cmpName(ptr->d_name, "..") + cmpName(ptr->d_name, ".") < 2) continue;
        else if((cases.case_a || ptr->d_name[0]!='.') && cases.case_R) {
            char* temp = setName(ptr->d_name, namedir);
            int st = stat(temp, &path_stat);
            if(S_ISDIR(path_stat.st_mode)&& st == 0) {
                directories[j] = setName(ptr->d_name, namedir);
                //printf("Name reset to %s\n", directories[j]);
                j++;
            }
            free(temp);
        }
    }
    //if(cases.case_t) files = timeSort(files, 0, count);
    //else ForwardSort(files, count);
    for(int i = 0; i < count; i++) {
        if(cases.case_a || files[i][0] != '.') {
            printf("%s  ", files[i]);
        }
    }
    closedir(directory);
    for(int i = 0; i < dirs; i++) {
        printf("\n");
        out = displayRecursively(directories[i], cases);
        if(out != 0) return out;
        free(directories[i]);
    }
    free(files);
    free(directories);
    return 0;
}

