#include"tlpi_hdr.h"
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/uio.h>
#define IOV_MAX 1024
stuct iovec{
    void *iov_base;
    size_t iov_len
}
ssize_t my_writev(int fd,const stuct iovec *iov,int iovcnt){
    size_t total_len=0;
    size_t off_set=0;
    size_t numWrite=0;
    if(iovcnt<0||iovcnt>IOV_MAX)
        return -1;
    for(size_t i=0;i<iovcnt;++i){
        total_len+=iov[i].iov_len;
    }
    char *buf=malloc(total_len);
    if(buf==NULL)
        return -1;
    for(size_t i=0;i<iovcnt;++i){
        memcpy(buf+off_set,iov[i].iov_base,iov[i].iov_len);
        off_set+=iov[i].iov_len;
    }
    if((numWrite=write(fd,buf,total_len))==-1)
        errExit("write");
    free(buf);
    return numWrite;
}

size_t my_readv(int fd,const stuct iovec *iov,int iovcnt){
    size_t total_len=0;
    size_t off_set=0;
    size_t numRead=0;
    if(iovcnt<0||iovcnt>IOV_MAX)
        return -1;
    for(size_t i=0;i<iovcnt;++i){
        total_len+=iov[i].iov_len;
    }
    char *buf=malloc(total_len);
    if(buf==NULL)
        return -1;
    if((numRead=read(fd,buf,total_len))==-1)
        errExit("read");
    size_t remaining = (size_t)numRead;
    for (int i = 0; i < iovcnt && remaining > 0; ++i){
        size_t to_copy = (iov[i].iov_len < remaining) ? iov[i].iov_len : remaining;
        memcpy(iov[i].iov_base, buf + off_set, to_copy);
        off_set += to_copy;    // 大内存游标往后推
        remaining -= to_copy;  // 待分配字节减少
    }
    free(buf);
    return numRead;
}