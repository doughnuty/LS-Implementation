#include "my_ls.h"

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

int cmpName(char* name1, char* name2) {
    int a = getLength(name1), b = getLength(name2);
    for(int i = 0; i < a; i++){
        if(name1[i] > name2[i]) return 1;
        else if(name2[i] > name1[i]) return 2;
    }
    if(a > b) return 1;
    else if(a < b) return 2;
    
    return 0;
}

char** timeSort(char** files, int size, char* namedir){
    char *temp;
    struct stat ibuf;
    struct stat jbuf;
    int count = 5, checker = -1;
    
    while(count != 0) {
        checker++;
        count = 0;
        int i = 0, j = 1;
        for(; j < size; j++) {
            temp = setName(files[i], namedir);
            int sti = stat(temp, &ibuf);
            free(temp);
            temp = setName(files[j], namedir);
            int stj = stat(temp, &jbuf);
            free(temp);
            if(sti != 0 || stj != 0) {
                //printf("error stat\n");
                break;
            }
            
            if (ibuf.st_mtime < jbuf.st_mtime) {
                temp = files[i];
                files[i] = files[j];
                files[j] = temp;    
                count++;
            }
            else if (ibuf.st_mtime == jbuf.st_mtime) {
                if (ibuf.st_mtim.tv_nsec < jbuf.st_mtim.tv_nsec) {
                    temp = files[i];
                    files[i] = files[j];
                    files[j] = temp;
                    count++;
                }
                else if (ibuf.st_mtim.tv_nsec == jbuf.st_mtim.tv_nsec){
                   int cmp = cmpName(files[i], files[j]);
                    if(cmp == 1){
                        temp = files[i];
                        files[i] = files[j];
                        files[j] = temp;
                        count++;
                    } 
                }
                
            }   
            i++;
        }
    }
    
    return files;
}

char** lexicoSort(char** files, int count){
    char *temp;
    int j = 1, checker = 1;
    
    while(checker != 0) 
    {
        checker = 0;
        j = 1;
        for(int i = 0; i < count - 1; i++)
        {
            int cmp = cmpName(files[i], files[j]);
            if(cmp == 1)
            {
                temp = files[i];
                files[i] = files[j];
                files[j] = temp;
                checker++;
            }
            j++;
        }
    }

    return files;
}

int displayRecursively(char* namedir, struct Cases cases){
    printf("\n");
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
    int count = 0, dirs = 0, j = 0, out;

    if(cases.case_R) {
        printf("%s:\n", namedir);
    }

    directory = opendir(namedir);
    if(directory == NULL) {
        printf("NULL\n");
        return 1;
    }
    while((ptr = readdir(directory)) != NULL) {
        count++;
        if(cmpName(ptr->d_name, "..") == 0 || cmpName(ptr->d_name, ".") == 0) continue; //Not include .. and . to the directory list to avoid infinite loop
        else if((cases.case_a || ptr->d_name[0] != '.') && cases.case_R) { //Not include hidden (dot) directories if -a parameter isn't present
            char* name = setName(ptr->d_name, namedir); //Change name of the file so the whole path will be seen
            int st = stat(name, &path_stat);
            if(st != 0) {
                printf("Error opening directory %s\n", name);
                return 1;
            }
            if (S_ISDIR(path_stat.st_mode)) {
                dirs++;
            }
            free(name);
        }
    }
    closedir(directory);

    char **files = (char**)malloc(sizeof(char*)*(count)), **directories = (char**)malloc(sizeof(char*)*dirs);

    directory = opendir(namedir);
    for(int i = 0; i < count; i++) {
        ptr = readdir(directory);
        if(ptr == NULL) return 1;
        files[i] = ptr->d_name;
        if(cmpName(ptr->d_name, "..") == 0 || cmpName(ptr->d_name, ".") == 0) continue;
        else if((cases.case_a || ptr->d_name[0]!='.') && cases.case_R) {
            char* temp = setName(ptr->d_name, namedir);
            int st = stat(temp, &path_stat);
            if(S_ISDIR(path_stat.st_mode) && st == 0) {
                directories[j] = ptr->d_name;
                j++;
            }
            free(temp);
        }
    }
    if(cases.case_t) {
        timeSort(files, count, namedir);
        timeSort(directories, dirs, namedir);
    }

    else {
        lexicoSort(files, count);
        lexicoSort(directories, dirs);
    }

    for(int i = 0; i < count; i++) { //Printing files found
        if(cases.case_a || files[i][0] != '.') {
            printf("%s  ", files[i]);
        }
    }
    free(files);

    for(int i = 0; i < dirs; i++) { //recursing
        printf("\n");
        directories[i] = setName(directories[i], namedir);
        out = displayRecursively(directories[i], cases);
        if(out != 0) return out;
        free(directories[i]);
    }
    free(directories);
    closedir(directory);
    return 0;
}