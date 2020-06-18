/* The following program is a simple implementation of my_ls
 * it supports such commands as
 * -a:
 *      includes directory entries whose name begin with a dot (.)
 * -t:
 *      sorts by the time modified starting with the most resent
 * -R:
 *      lists recursively all found subdirectories
 * and their combinations.
 */
#include "my_ls.h"
#include <stdio.h>
/* Functions allowed: malloc(3) free(3) printf(3) open(2) close(2) lseek(2) read(2) write(2) stat(2) opendir(2) closedir(2) readdir(2)*/

int main(int argc,char **argv){

    struct Cases cases = myGetopt(argc, argv);
    if (cases.case_err) return 1;
    if(cases.fcount > 0) // depending on the type of argument passed, prints either filename or the directory contents
    { 
        for(int i = 0; i < cases.fcount; i++)
        {
            if(i == cases.fcount - 1 && cases.dcount > 0) // to avoid extra new lines
            {
                if(!cases.case_R) printf("%s:\n", cases.files[i]);
                scanDir(cases.files[i], cases);
            }
            else if(cases.fcount > 1 && cases.dcount > 0) // to print if multiple arguments passed and some of them are directories
            {
                printf("%s:\n", cases.files[i]);
                scanDir(cases.files[i], cases);
                printf("\n\n");
                cases.dcount--;
            }
            else printf("%s  ", cases.files[i]); // simple filename print
        }
        cases.fcount--;
    }
    
    else // if no filename arguments were passed
        scanDir(cases.files[cases.fcount], cases);

    printf("\n");
    free(cases.files);
    
    return 0;
}