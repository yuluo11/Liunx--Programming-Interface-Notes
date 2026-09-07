#include "tlpi_hdr.h"
#include<fcntl.h>
#include<sys/stat.h>
int main(int argc,char *argv[]){
    int fd;
    if(argc!=2||strcmp(argv[1],"--help")==0)
        usageErr("%s file\n",argv[0]);

    fd=open(argv[1],O_WRONLY|O_APPEND);
    if(fd==-1)
        errExit("open");
    off_t offset_after_write=lseek(fd,0,SEEK_SET);
    if(off_t offset_after_write==-1)
        errExit("lseek");
    if (write(fd, "hello world", 11) == -1) {
        errExit("write");
    }

    off_t offset_after_write = lseek(fd, 0, SEEK_CUR);

    if(offset_after_write==-1)
        errExit("lseek");

    printf("Offset after write: %ld\n", (long)offset_after_write);

    if(close(fd)==-1)
        errExit("close");

    return 0;
}