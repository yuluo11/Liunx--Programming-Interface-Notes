# Linux 系统编程手册学习note
参考:[man手册](https://man7.org/linux/man-pages/)

## CH2-3
主要讲基本概念和系统调用，里面的为所有程序所使用的错误处理函数有点看不懂

### 3.1代码:
```
int reboot(int magic,int magic2,int cmd,void *arg)
```
magic必须等于LINUX_REBOOT_MAGIC1(16进制为0xfee1dead，很有意思哈)  

magic2必须等于LINUX_REBOOT_MAGIC2A(16进制为0x05121996,算是一个彩蛋，它有四个合法值，这个就是其中之一linus大女儿的生日)

## CH4:
主要讲了文件的IO的系统调用，整体就是open(),read(),write(),close(),对于打开的文件，kernel会维护文件的偏移量，这时会用到系统调用lseek(),当然还了解一些文件标识符....

### [4.1代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/Exercies/tee.c):
里面的getopt把我折磨的不清轻，借鉴了很多大佬的写法然后不断查看资料（man手册为主），才写出来，总体的思路就是先解析命令行:
```
while((int opt=getopt(argc,argv,"a"))!=-1)
```
后面会有一个getopt里面的全局变量optind(option index),optind会指向第一个非选项参数(例如你传入的文件名)后面就是前面说的open(),read(),write().

### 4.2：
有点没看懂....

## CH5:
整体来讲，感觉围绕原子操作在讲，然后就算文件描述符（fd）.

### 5.1:
现在电脑是64位，不好去测试,代码就不写了（

### [5.2代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/Exercies/5.2.c):
虽然lseek将游标设置为零，但是write依然受O_APPEND的控制，从文件末尾开始写入数据

### [5.3代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/Exercies/5.3.c):
没啥好说的.

### [5.4代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/Exercies/5.4.c):
感觉这几个题目都在讲系统调用的原子性

### 5.5:
这个题目感觉没有写的必要(其实就是想偷懒)，然后就算当然共享

### 5.6:
输出是 Gidday world,因为共享游标后面的在就是fd3又是独立的open()所以offset一直在0-6,改动也一在0-6,后面world不变，即位Gidday world

### [5.7代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/Exercies/5.7.c):
整体的思路就是遍历数组得到数据长度，然后malloc动态分配内存，然后write/read一次性写入/读取，用memcpy把各个小缓冲区的内容按顺序拷进内存，当然值得注意的是这里还要考虑到写入或者分发时的off_set最后就是free，这里read相对复杂一些

## CH15
说是文件属性，不过我觉得大部分内容是在讲权限的内容

### 15.1:
这里就不做展示了
a和b就略了.
c
```
                        目录     文件
创建文件:                  wx       -
打开读：                   x        r
打开写：                   x        w
删除文件：                 wx       -
重命名：                   wx       -
重命名存在：               wx       -    （会覆盖）

```

设置 sticky 位后，删除或重命名目录中的文件时，通常只有以下用户可以操作：
1. 文件所有者
2. 目录所有者
3. root 或具有相应特权的用户

### 15.2
不会，我们是通过stat获取信息，如果改了就没有意义了

### 15.3
只是将
```
printf("Last file access:         %s", ctime(&sb->st_atime));
printf("Last file modification:   %s", ctime(&sb->st_mtime));
printf("Last status change:       %s", ctime(&sb->st_ctime));

```
改为
```
printf("Last file access:         %.24s.%09ld\n",
       ctime(&sb->st_atime), sb->st_atim.tv_nsec);

printf("Last file modification:   %.24s.%09ld\n",
       ctime(&sb->st_mtime), sb->st_mtim.tv_nsec);

printf("Last status change:       %.24s.%09ld\n",
       ctime(&sb->st_ctime), sb->st_ctim.tv_nsec);

```
这里就不具体写代码了

### 15.4
等学完前面进程再来补

### 15.5
```
mode_t oldMask;

oldMask = umask(0);     /* 取得旧值，同时暂时将 umask 设置为 0 */
umask(oldMask);         /* 恢复原来的 umask */

```

### 15.6代码

### 15.7代码
rwx 这些是对于用户组其他 i-node标志对于的是文件本身的权限

## CH18
readdir_r()在现代已被废弃

### 18.1
这里就是用unlink断开了原先的链接(原进程依然进行)，然后创建新的同名文件，分配一个全新的i-node，新的可执行内容被写入这个i-node

### 18.2
说实话查了半天资料也没有搞懂个所以然

### 18.4
这个现代已经淘汰了，就不想写了
