#include <sys/stat.h>
#include<fcntl.h>
#include"tlpi_hdr"
int main(int *argc,char *argv[]){
    struct stat sb;
    mode_t mode;
    for (int i = 1; i < argc; i++) {
        if (stat(argv[i], &sb) == -1) {
            errMsg("stat: %s", argv[i]);
            continue;
        }
    
    mode=sb.st_mode|S_IRUSR | S_IRGRP | S_IROTH;
    if (S_ISDIR(sb.st_mode) ||
            (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
            mode |= S_IXUSR | S_IXGRP | S_IXOTH;
    }
    if (chmod(argv[i], mode) == -1)
            errMsg("chmod: %s", argv[i]);
    }      
    exit(EXIT_SUCCESS);
}
