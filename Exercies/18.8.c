#define _XOPEN_SOURCE 600

#include"tlpi_hdr.h"
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
#include<ftw.h>

typedef int (*walk_fn)(const char *, const struct stat *, int, struct FTW *);

static int walk(const char *path,walk_fn fn, int flags,int level)
{
    struct stat sb;
    struct FTW info;

    const char *slash = strrchr(dirpath, '/');/*用与找最后dirpath最后一个“/”*/

    info.base = slash ? (int)(slash - dirpath + 1) : 0;
    info.level = 0; 

    
    if(flags & FTW_PHYS){
        if(lstat(dirpath,&sb)==-1)
            errExit("lstat");
    }

    if (!S_ISDIR(sb.st_mode))
        return fn(path, &sb, FTW_F, &info);

    
    if(S_ISLNK(sb.st_mode)){
        return fn(dirpath,&sb,FTW_SL,&info);
    }

    DIR *dir = opendir(dirpath);

    if(dir==NULL)
        return fn(path, &sb, FTW_DNR, &info);

    if(!(flags & FTW_DEPTH)){
        int ret fn(dirpath,&sb,FTW_SL,&info);
        if(ret!=0){
            if(closedir(dir)==-1)
                errExit("closedir");
            return ret;
        }
    }

        
    struct dirent *entry;
        while((entry=readdir(dir))!=NULL){
            struct stat st;

            if(strcmp(".",entry->d_name)==0||strcmp("..",entry->d_name)==0)
                continue;

            if(stat(entry->d_name,&st)==-1)
                errExit("stat");

            int need_slash = len > 0 && dirpath[len - 1] != '/';
            size_t size=strlen(dirpath)+strlen(entry->d_name)+1+need_slash;
            const char*temp=malloc(size);

            if(temp==NULL)
                errExit("malloc");

            snprintf(temp, size, "%s%s%s",
                        dirpath, need_slash ? "/" : "", entry->d_name);
            walk(child_path, fn, flags, level + 1);
            free(temp);        
        }
        closedir(dir);

        if (flags & FTW_DEPTH)
            return fn(dirpath, &sb, FTW_DP, &info);
        return 0;
}

int mynftw(const char *dirpath, walk_fn fn, int nopenfd, int flags)
{
    if (dirpath == NULL || fn == NULL || nopenfd <= 0 ||
        (flags & ~(FTW_PHYS | FTW_DEPTH))) {
        errno = EINVAL;
        return -1;
    }

    return walk(dirpath, fn, flags, 0);
}

