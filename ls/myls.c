/*
 * myls - A simplified implementation of the Unix ls command.
 *
 * Lists files and directories with the following options:
 *   -a  Include hidden files.
 *   -l  Display detailed file information.
 *   -R  List subdirectories recursively.
 *   -t  Sort by modification time, newest first.
 *   -r  Reverse the sorting order.
 *   -i  Display inode numbers.
 *   -s  Display allocated disk space in blocks.
 *
 * Options can be combined, for example: ./myls -alR
 * File names are color-coded by file type.
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<errno.h>
#include <inttypes.h>
#include<getopt.h>
#include <sys/sysmacros.h>

#define MAXSIZE 1000

typedef struct {
    int show_all;       // -a
    int long_format;    // -l
    int recursive;      // -R
    int sort_by_time;   // -t
    int reverse;        // -r
    int show_inode;     // -i
    int show_blocks;    // -s
} Options;

typedef struct{
    Options options;
    char **paths;
    int path_count;
}Arguments;

typedef struct{
    char *name;
    char *full_path;
    struct stat info;
}Entry;

typedef struct{
    Entry items[MAXSIZE];
    size_t count;
}EntryList;

int list_path(const char *path, const Options *options);
int list_dir(const char *path, const Options *options);

int collect_entry(const char *dir_path, const char *name,
                  EntryList *list);

void sort_entries(EntryList *list, const Options *options);

void print_entry(const char *name, const char *full_path,
                 const struct stat *info, const Options *options);

static void format_permissions(mode_t mode, char text[11])
{
    strcpy(text, "----------");

    if (S_ISDIR(mode))       text[0] = 'd';
    else if (S_ISLNK(mode))  text[0] = 'l';
    else if (S_ISCHR(mode))  text[0] = 'c';
    else if (S_ISBLK(mode))  text[0] = 'b';
    else if (S_ISFIFO(mode)) text[0] = 'p';
    else if (S_ISSOCK(mode)) text[0] = 's';
    else if (!S_ISREG(mode)) text[0] = '?';

    const mode_t bits[9] = {
        S_IRUSR, S_IWUSR, S_IXUSR,
        S_IRGRP, S_IWGRP, S_IXGRP,
        S_IROTH, S_IWOTH, S_IXOTH
    };
    const char symbols[] = "rwxrwxrwx";

    for (size_t i = 0; i < 9; i++) {
        if (mode & bits[i]) 
            text[i + 1] = symbols[i];
    }

    if (mode & S_ISUID) text[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID) text[6] = (mode & S_IXGRP) ? 's' : 'S';
    if (mode & S_ISVTX) text[9] = (mode & S_IXOTH) ? 't' : 'T';
}

static void print_link_target(const char *full_path)
{
    size_t capacity = 256;
    char *target = malloc(capacity + 1);
    if (target == NULL) {
        perror("malloc");
        return;
    }

    for (;;) {
        ssize_t length = readlink(full_path, target, capacity);
        if (length == -1) {
            perror(full_path);
            break;
        }
        if ((size_t)length < capacity) {
            target[length] = '\0';
            printf(" -> %s", target);
            break;
        }

        if (capacity > (SIZE_MAX - 1) / 2) {
            errno = EOVERFLOW;
            perror(full_path);
            break;
        }

        size_t new_capacity = capacity * 2;
        char *temp = realloc(target, new_capacity + 1);
        if (temp == NULL) {
            perror("realloc");
            break;
        }
        target=temp;
        capacity = new_capacity;
    }
    free(target);
}

void print_color(const char *name, const struct stat *info, int use_color)
{
    const char *color = "";

    if (use_color) {
        if (S_ISDIR(info->st_mode)) {
            color = "\033[34m";      // Blue: directory
        } else if (S_ISLNK(info->st_mode)) {
            color = "\033[36m";      // Cyan: symbolic link
        } else if (S_ISREG(info->st_mode) &&
                   (info->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
            color = "\033[32m";      // Green: executable file
        }
    }

    printf("%s%s%s", color, name, *color ? "\033[0m" : "");
}


int parse_args(int argc, char *argv[], Arguments *args){
    *args = (Arguments){0};
    int opt;
    while((opt=getopt(argc,argv,"alRtris")) !=-1){
        switch (opt) {
            case 'a': args->options.show_all = 1;     break;
            case 'l': args->options.long_format = 1;  break;
            case 'R': args->options.recursive = 1;    break;
            case 't': args->options.sort_by_time = 1; break;
            case 'r': args->options.reverse = 1;      break;
            case 'i': args->options.show_inode = 1;   break;
            case 's': args->options.show_blocks = 1;  break;
            default:
                return EXIT_FAILURE;
        }
    }
    args->paths = argv + optind;
    args->path_count = argc - optind;
    return 0;
}

int list_path(const char *path,const Options *options){

    struct stat sb;
    if(lstat(path,&sb)==-1){
        perror(path);
        return -1;
    }
    if(S_ISDIR(sb.st_mode))
        return list_dir(path,options);

    print_entry(path,path,&sb,options);

    return 0;
}

int list_dir(const char *path,const Options *options){
    EntryList list = {0};
    struct dirent *dp;
    DIR *dirp;
    dirp=opendir(path);

    if(dirp==NULL){
        perror(path);
        return -1;
    }

    int result=0;

    for(;;){
        errno=0;
        dp=readdir(dirp);
        if(dp==NULL){
            if(errno!=0){
                perror(path);
                result=-1;
            }
            break;
        }

        if (!options->show_all && dp->d_name[0] == '.')
            continue; 
        
        if (collect_entry(path, dp->d_name, &list) == -1) {
            result = -1;
            break;
        }
    }

    if (closedir(dirp) == -1){
        perror(path);
        result = -1;
    }

    sort_entries(&list,options);

    if (options->recursive) 
        printf("%s:\n", path);  

    for(size_t i=0;i<list.count;i++){
        print_entry(
            list.items[i].name,
            list.items[i].full_path,
            &list.items[i].info,
            options
        );
    }

    if (options->recursive) {
        for (size_t i = 0; i < list.count; i++) {

            if (strcmp(list.items[i].name, ".") == 0 ||
                strcmp(list.items[i].name, "..") == 0) {
                continue;
            }

            if (S_ISDIR(list.items[i].info.st_mode)) {

                putchar('\n');

                if (list_dir(list.items[i].full_path, options) == -1) {
                    result = -1;
                }

            }
        }
    }

    for (size_t i = 0; i < list.count; i++) {
        free(list.items[i].name);
        free(list.items[i].full_path);
    }
    
    return result;
}

int collect_entry(const char *dir_path,const char *name,EntryList *list){
    if (list->count >= MAXSIZE){
        fprintf(stderr, "%s: too many entries\n", dir_path);
        return -1;
    }

    size_t size = strlen(dir_path) + strlen(name) + 2;

    char *full_path = malloc(size);
    if (full_path == NULL){
        perror("malloc");
        return -1;
    }

    snprintf(full_path, size, "%s/%s", dir_path, name);

    if(lstat(full_path,&list->items[list->count].info)==-1){
        perror(full_path);
        free(full_path);
        return -1;
    }
    char *name_copy = malloc(strlen(name) + 1);

    if (name_copy == NULL) {
        perror("malloc");
        free(full_path);
        return -1;
    }

    strcpy(name_copy, name);

    list->items[list->count].name = name_copy;
    list->items[list->count].full_path = full_path;
    list->count++;
    return 0;
} 

void sort_entries(EntryList *list, const Options *options){
    if(options->sort_by_time){
        for(size_t i=1;i<list->count;i++){
            Entry temp = list->items[i];
            size_t j;
            for(j=i;j>0;j--){
                struct timespec left = list->items[j-1].info.st_mtim;
                struct timespec key = temp.info.st_mtim;
                if (left.tv_sec < key.tv_sec ||
                    (left.tv_sec == key.tv_sec && left.tv_nsec < key.tv_nsec)) {
                    list->items[j] = list->items[j-1];
                }else if(left.tv_sec == key.tv_sec &&
                        left.tv_nsec == key.tv_nsec &&
                        strcmp(list->items[j-1].name, temp.name) > 0){
                            list->items[j] = list->items[j-1];
                }else{
                    break;
                }
            }
            list->items[j] = temp; 
        }
    }else{
        for(size_t i=1;i<list->count;i++){
                Entry temp=list->items[i];
                size_t j;
                for( j=i;j>0;j--){
                    if(strcmp(list->items[j-1].name,temp.name)>0){
                        list->items[j]=list->items[j-1];
                }else{
                    break;
                }
            }
            list->items[j]=temp;
        }
    }
    if(options->reverse){
        size_t count=list->count;
        for(size_t i=count;i>count/2;i--){
            Entry temp = list->items[i - 1];

            list->items[i-1] = list->items[count-i];
            list->items[count-i] = temp;
        }
    }
}

void print_entry(const char *name,const char *full_path,const struct stat *info,const Options *options){
    
    if(options->show_inode)
        printf("%ju ",(uintmax_t)info->st_ino);
    
    if(options->show_blocks){
        intmax_t blocks = (intmax_t)info->st_blocks;
        printf("%jd ", blocks / 2 + (blocks % 2 != 0));
    }

    if(options->long_format){
        char permissions[11];
        format_permissions(info->st_mode, permissions);
        printf("%s %ju ", permissions, (uintmax_t)info->st_nlink);

        struct passwd *pw = getpwuid(info->st_uid);
        if (pw != NULL) printf("%s ", pw->pw_name);
        else printf("%ju ", (uintmax_t)info->st_uid);

        struct group *gr = getgrgid(info->st_gid);
        if (gr != NULL) printf("%s ", gr->gr_name);
        else printf("%ju ", (uintmax_t)info->st_gid);

        if (S_ISCHR(info->st_mode) || S_ISBLK(info->st_mode)) {
            printf("%ju, %ju ", (uintmax_t)major(info->st_rdev),
                   (uintmax_t)minor(info->st_rdev));
        } else {
            printf("%jd ", (intmax_t)info->st_size);
        }

        time_t modified = info->st_mtime;
        struct tm *tm_info = localtime(&modified);
        char time_text[64];
        if (tm_info != NULL &&
            strftime(time_text, sizeof(time_text), "%Y-%m-%d %H:%M",
                     tm_info) != 0) {
            printf("%s ", time_text);
        } else {
            printf("? ");
        }

    }

    print_color(name, info, isatty(STDOUT_FILENO));

    if (options->long_format && S_ISLNK(info->st_mode))
        print_link_target(full_path);

    putchar('\n');
    
}

int main(int argc, char *argv[])
{
    Arguments args;

    if (parse_args(argc, argv, &args) != 0) {
        return EXIT_FAILURE;
    }

    int result = EXIT_SUCCESS;

    if (args.path_count == 0) {

        if (list_path(".", &args.options) != 0) {
            result = EXIT_FAILURE;
        }

    } else {

        for (int i = 0; i < args.path_count; i++) {

            if (list_path(args.paths[i], &args.options) != 0) 
                result = EXIT_FAILURE;
            
        }
    }

    return result;
}