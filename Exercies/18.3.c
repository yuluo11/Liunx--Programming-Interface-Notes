#define _XOPEN_SOURCE 700
#include<sys/stat.h>
#include<fcntl.h>
#include"tlpi_hdr"
#include<limits.h>
#include <libgen.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

char* myrealpath(const char *pathname,char *resloved){
    if (resolved == NULL) {
        resolved = malloc(PATH_MAX);
        if (resolved == NULL) 
            errExit("malloc");
    }
    // 1. 备份当前目录的 fd。打开失败直接 errExit
    int old_fd = open(".", O_RDONLY);
    if (old_fd == -1) 
        errExit("open \".\"");

    char current_path[PATH_MAX * 2];
    char dir_buf[PATH_MAX];
    char base_buf[PATH_MAX];
    char link_target[PATH_MAX];
    
    strncpy(current_path, pathname, sizeof(current_path) - 1);
    current_path[sizeof(current_path) - 1] = '\0';

    int symlink_count = 0;
    while(1){
        struct stat sb;

        strncpy(dir_buf, current_path, PATH_MAX);
        strncpy(base_buf, current_path, PATH_MAX);
        char *dname = dirname(dir_buf);
        char *bname = basename(base_buf);

        if (chdir(dir) == -1)
            errExit("chdir");

        if (lstat(base, &sb) == -1)
            errExit("lstat");
        if(S_ISLINK(sb,st_mode)){
            ssize_t n;

            if(++symlink_cout>40)
                fatal("Too many symbolic links");

            n=readlink(base,link_target,sizeof(link_target))

            if(n==-1)
                errExit("readlink");

            if ((size_t)n >= sizeof(link_target))
                fatal("Symbolic link target too long");

            link_target[n] = '\0';
            strcpy(current_path, link_target);
            continue;
        }
    }
        
}

int
main(int argc, char *argv[])
{
    char *resolved;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    resolved = myrealpath(argv[1], NULL);
    if (resolved == NULL)
        errExit("myrealpath");

    printf("%s\n", resolved);

    free(resolved);
    exit(EXIT_SUCCESS);
}
