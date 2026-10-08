#include<fcntl.h>
#include"tlpi_hdr.h"
#include<sys/stat.h>
#include<limits.h>
#include<dirent.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

char *mygetcwd(char *buf,size_t size){
    char result[PATH_MAX]="";
    char temp[PATH_MAX];
    int old_fd=open(".",O_RDONLY);

    if(old_fd==-1)
        errExit("open");


    struct dirent *entry;
    struct stat candidate;

    for(;;){
        struct stat current;
        struct stat parent;

        if(stat(".",&current)==-1||stat("..",&parent)==-1)
            errExit("stat");

        if (current.st_ino == parent.st_ino &&
        current.st_dev == parent.st_dev)
        break;

        if(chdir(".."))
            errExit("chdir");

        DIR *dir=opendir(".");

        if(dir==NULL)
            errExit("opendir");

        while((entry=readdir(dir))!=NULL){
            
            if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)
                continue;
            if(lstat(entry->d_name,&candidate))
                errExit("lstat");
            if(candidate.st_ino==current.st_ino&&
                candidate.st_dev==current.st_dev){
                int n = snprintf(temp, sizeof(temp),
                "/%s%s", entry->d_name, result);

            if (n < 0 || (size_t)n >= sizeof(temp))
                fatal("Path too long");

                strcpy(result, temp);
                break;
           }
        }

        if (entry == NULL)
            fatal("Cannot find directory name");

        if(closedir(dir)==-1)
            errExit("closedir");
    }   

    if(result[0]=='\0')
        strcpy(result,"/");
        
    if(strlen(result)>=size){
        errno = ERANGE;
        return NULL;
    }

        if(fchdir(old_fd)==-1)
            errExit("fchdir");

    
    if(close(old_fd)==-1)
        errExit("close");

    strcpy(buf,result);

    return buf;
}


int main(int argc, char *argv[])
{
    char before[PATH_MAX];
    char result[PATH_MAX];
    char after[PATH_MAX];

    if (argc > 2 ||
        (argc == 2 && strcmp(argv[1], "--help") == 0))
        usageErr("%s [directory]\n", argv[0]);

    /* 可选：先进入指定目录，再开始测试 */
    if (argc == 2 && chdir(argv[1]) == -1)
        errExit("chdir");

    if (getcwd(before, sizeof(before)) == NULL)
        errExit("getcwd before");

    if (mygetcwd(result, sizeof(result)) == NULL)
        errExit("mygetcwd");

    if (getcwd(after, sizeof(after)) == NULL)
        errExit("getcwd after");

    printf("System getcwd: %s\n", before);
    printf("My getcwd:     %s\n", result);
    printf("Current dir:   %s\n", after);

    if (strcmp(before, result) != 0) {
        fprintf(stderr, "FAIL: incorrect pathname\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(before, after) != 0) {
        fprintf(stderr, "FAIL: working directory changed\n");
        exit(EXIT_FAILURE);
    }

    printf("PASS\n");
    exit(EXIT_SUCCESS);
}