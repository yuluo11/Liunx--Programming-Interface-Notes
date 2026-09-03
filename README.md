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

## CH4
主要讲了文件的IO的系统调用，整体就是open(),read(),write(),close(),对于打开的文件，kernel会维护文件的偏移量，这时会用到系统调用lseek(),当然还了解一些文件标识符....

### 4.1代码:
里面的getopt把我折磨的不清轻，借鉴了很多大佬的写法然后不断查看资料（man手册为主），才写出来，总体的思路就是先解析命令行:
```
    while((int opt=getopt(argc,argv,"a"))!=-1)
```
后面会有一个getopt里面的全局变量optind(option index),optind会指向第一个非选项参数(例如你传入的文件名)后面就是前面说的open(),read(),write().






