#include<fcntl.h>
#include"tlpi_hdr.h"
int my_dup(int oldfd){
    int flags,newfd;
    flags=fcntl(oldfd,F_GETFL);
    if(flags==-1)
        errExit(fcntl);
    newfd=flags;
    return newfd;
}

int my_dup2(int newfd,int oldfd){
    if (newfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (fcntl(oldfd, F_GETFL) == -1) {
        errno = EBADF;
        return -1;
    }
    if(oldfd==newfd)
        return newfd;
    close(newfd);
    return fcntl(oldfd, F_DUPFD, newfd);
}