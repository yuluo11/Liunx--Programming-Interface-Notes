#include<sys/stat.h>
#include<fcntl.h>
#include"tlpi_hdr.h"
#define buf_size 1024
#define max_file 1024
/*
    整体思路就是先解析命令行参数，用到getopt，后续就是open(),read(),write()
    里面有个0666,我现在还是不太了解正常来说应该按书上的是S_IRUSR | S_IWUSR | 
              S_IRGRP | S_IWGRP | 
              S_IROTH | S_IWOTH)
*/
int main(int argc,char *argv[]){
    int is_append=0;
    int opt=0;
    char buffer[buf_size];
    int tee_fd[max_file];
    int fd_no=0;
    ssize_t numRead;
    ssize_t numWirte;
    while((opt=getopt(argc,argv,"a"))!=-1){
        switch(opt){
        case 'a':
            is_append=1;
            break;
        default:/* '?' */
            exit(0);
        }
    }
    int flags=O_CREAT | O_WRONLY | (is_append?O_APPEND:O_TRUNC);
    if(optind<argc){
        for(int i=optind;i<argc;++i){
            if (fd_no >= max_file) {
                fprintf(stderr, "Error: Too many target files (max %d)\n", MAX_FILES);
                exit(EXIT_FAILURE);
    }
            int fd=open(argv[i],flags,0666);
            if(fd<0){
                perror(argv[i]);
                exit(EXIT_FAILURE);
            }
            tee_fd[fd_no++]=fd;
        }
    }
    while((numRead=read(STDIN_FILENO,buffer,buf_size))>0){
        for(int i=0;i<fd_no;++i){
            if(write(tee_fd[i],buffer,numRead)!=numRead)
                exit(1);
        }
    }
    if(numRead<0)
        exit(1);
    for(int i=0;i<fd_no;++i){
        close(tee_fd[i]);
    }
    return 0;
}
