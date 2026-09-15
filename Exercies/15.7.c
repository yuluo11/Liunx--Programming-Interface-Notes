#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include "tlpi_hdr.h"

static int
charToFlag(char ch)
{
    switch (ch) {
    case 'a': return FS_APPEND_FL;
    case 'A': return FS_NOATIME_FL;
    case 'c': return FS_COMPR_FL;
    case 'd': return FS_NODUMP_FL;
    case 'D': return FS_DIRSYNC_FL;
    case 'i': return FS_IMMUTABLE_FL;
    case 'j': return FS_JOURNAL_DATA_FL;
    case 's': return FS_SECRM_FL;
    case 'S': return FS_SYNC_FL;
    case 't': return FS_NOTAIL_FL;
    case 'T': return FS_TOPDIR_FL;
    case 'u': return FS_UNRM_FL;
    default:
        fatal("Unknown attribute: %c", ch);
    }
}

int
main(int argc, char *argv[])
{
    int fd;
    int flags;
    int mask = 0;
    char operation;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s {+|-|=}attributes file...\n", argv[0]);

    operation = argv[1][0];

    if (operation != '+' &&
        operation != '-' &&
        operation != '=') {
        usageErr("Mode must begin with +, -, or =\n");
    }

    /* 把 aiAd 等字符转换成 FS_* 位掩码 */
    for (int i = 1; argv[1][i] != '\0'; i++)
        mask |= charToFlag(argv[1][i]);

    for (int i = 2; i < argc; i++) {
        fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            errMsg("open %s", argv[i]);
            continue;
        }

        if (ioctl(fd, FS_IOC_GETFLAGS, &flags) == -1) {
            errMsg("FS_IOC_GETFLAGS %s", argv[i]);
            close(fd);
            continue;
        }

        switch (operation) {
        case '+':
            flags |= mask;      /* 增加标志 */
            break;

        case '-':
            flags &= ~mask;     /* 删除标志 */
            break;

        case '=':
            flags = mask;       /* 只保留指定标志 */
            break;
        }

        if (ioctl(fd, FS_IOC_SETFLAGS, &flags) == -1)
            errMsg("FS_IOC_SETFLAGS %s", argv[i]);

        close(fd);
    }

    exit(EXIT_SUCCESS);
}