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

### 5.1代:
现在电脑是64位，不好去测试,代码就不写了（

### [5.2代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/Exercies/5.2.c):
虽然lseek将游标设置为零，但是write依然受O_APPEND的控制，从文件末尾开始写入数据

### [5.3代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/5.3.c):
没啥好说的.

### [5.4代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/5.4.c):
感觉这几个题目都在讲系统调用的原子性

### 5.5:
这个题目感觉没有写的必要(其实就是想偷懒)，然后就算当然共享

### 5.6:
输出是 Gidday world,因为共享游标后面的在就是fd3又是独立的open()所以offset一直在0-6,改动也一在0-6,后面world不变，即位Gidday world

### [5.7代码](https://github.com/yuluo11/Liunx--Programming-Interface-Notes/blob/main/5.7.c):
整体的思路就是遍历数组得到数据长度，然后malloc动态分配内存，然后write/read一次性写入/读取，用memcpy把各个小缓冲区的内容按顺序拷进内存，当然值得注意的是这里还要考虑到写入或者奋发时的off_set最后就是free，这里read相对复杂一些










