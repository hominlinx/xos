# xos
a little os

0. 调试: 
    * `make debug` 这个时候qemu启动
    * 开启另外一个终端, 使用vim打开工程, 执行`ConqueGdb`
    * 若调试实模式, 建议 `set arch i8086`
    * 在gdb下加载obj, `file ./obj/***` 
    * `target remote :1234` 连接qemu 就可以调试了.

1. boot 引导程序 ; init 存放kernel的最初代码; kernel存放驱动; libs是一些库(比如string, printf); mm 是内存相关
