#include<fcntl.h>
#include"tlpi_hdr.h"
#include<sys/stat.h>
#define BUF_SIZE 1024
int main(int argc,char *argv[]){
    int fd;
    Boolean use_append;
    if(argc<2||argc>4||strcmp(argv[1],"--help")==0)
        usageErr("%s file\n",argv[0]);
    
    int num_bytes=getInt(argv[2],GN_GT_0,"num-bytes");
    use_append=(argc<4);    
    int flags=O_WRONLY|O_CREAT;
    if(use_apend)
        flags|=O_APPEND;
    fd=open(argv[1],flags,0644);
    if(fd==-1)
        errExit("open");
    char ch='a';
    for(int i=0;i<num_bytes;++i){
        if(!use_apend){
            if(lseek(fd,0,SEEK_END)==-1)
                errExit("lseek");
        }
        if(write(fd,&ch,1)==-1)
            errExit("write");   
    }
    if(close(fd)==-1)
        errExit("close");
    return 0;
}