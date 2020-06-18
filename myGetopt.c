#include <stdlib.h>
#include "my_ls.h"

size_t getLength(const char *str);

void clear(struct Cases* args) // set all arguments to default
{
    args->case_a = false;
    args->case_R = false;
    args->case_t = false;
    args->case_err = false;
    args->files = (char**)malloc(sizeof(char*));
    args->files[0] = ".";
    args->dcount = 0;
    args->fcount = 0;
}

struct Cases myGetopt(int argc, char **argv)
{
    struct stat path_stat;
    struct Cases args;
    clear(&args);
    int incor = 0; // to check how many incorrect arguments
    for(int i = 1; i < argc; i++) //parsing arguments
    {
        if(argv[i][0] == '-')
        {
            int length = getLength(argv[i]);
            for(int j = 1; j < length; j++)
            {
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
        else 
        {
            if (args.fcount == 0) 
            {
                free(args.files);
                args.files = (char**)malloc(sizeof(char*)*(argc - 1));
            }

            int st = stat(argv[i], &path_stat);
            if (st != 0)
            {
                printf("my_ls: cannot access '%s': No such file or directory\n", argv[i]);
                args.fcount--;
                incor++;
            } 
            else args.files[args.fcount] = argv[i]; //remember arguments passed

            if(S_ISDIR(path_stat.st_mode)) args.dcount++;
            args.fcount++;
        }
    }

    if(args.fcount > 1) 
    {
        if(args.case_t) timeSort(args.files, args.fcount, ".");
        else lexicoSort(args.files, args.fcount);
    }
    else if (incor > 0 && args.fcount <= 0) args.case_err = true;
    return args;
}