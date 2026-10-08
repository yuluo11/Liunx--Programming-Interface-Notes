#define _XOPEN_SOURCE 600

#include<nfw.h>
#include"tlpi_hdr.h"
#include<fcntl.h>
#include<sys/stat.h>

static int countFileType(const *pathname,const struct stat *sb,
                    int tflag,struct FTW *ftwbuf)
{
    if (S_ISREG(statbuf->st_mode))
		++regular_file;
	if (S_ISDIR(statbuf->st_mode))
		++dir_file;
	if (S_ISLNK(statbuf->st_mode))
		++sym_link;
    return 0;
}
int main(int *argc,char *argv[]){
    int flag=0;
    if(argc!=2||strcmp(argv[1],"--help")==0)
        usageErr("dir");
    flag|=FTW_PHYS;
    if (nftw(argv[1], count_file, 10, flag) == -1)
		errExit("nftw");

    printf("Regular files takes about %.2lf%%\n", (double)regular_file / (regular_file + dir_file + sym_link) * 100);
	printf("Dirent  files takes about %.2lf%%\n", (double)dir_file / (regular_file + dir_file + sym_link) * 100);
	printf("symbol  links takes about %.2lf%%\n", (double)sym_link / (regular_file + dir_file + sym_link) * 100);

    return 0;
}