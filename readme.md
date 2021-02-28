## 说明

基于c++17 文件搜索工具

- 运行原理: 读取$PATH环境变量,解析目录列表
- 多线程遍历目录,加入到容器中。	
- 容器数据结构: map<string,set>

## usage

```shell
which.exe filename
```

## version
```shell
which.exe -v
```

## help
```shell
which.exe -h
```

## demo

```shell
PS E:\ModernCpp\ModernCpp\which> which java.exe
D:\ProgramKits\emsdk\emsdk\java\8.152_64bit\bin\java.exe
D:\ProgramKits\jdk8u281\bin\java.exe
PS E:\ModernCpp\ModernCpp\which>
```

