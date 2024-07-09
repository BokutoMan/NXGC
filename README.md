# 本项目为逆向工程大作业的代码整理
## 1_Injection (刚开始做，使用g++编译)
direct.cpp 是直接注入的代码

dll.cpp 是dll注入的代码

MyDll.cpp 是注入的DLL(使用

```
g++ -shared -o MyDLL.dll MyDLL.cpp
```
编译为DLL)

sleep.cpp 是测试注入用的程序
## 2_HOOK(发现g++还是有点麻烦，后面使用VS2022)
IAT.cpp 是借助Detours实现的IAT钩子代码

inline.cpp 是修改函数入口实现钩子的代码
## 3_HOOK
Dll1.cpp 是钩子植入的DLL文件，使用VS2022编译

global.cpp 是挂全局消息钩子的文件，使用VS2022编译(在多字节编码、admin权限下编译)

thread.cpp 是挂当前线程消息钩子的文件，使用VS2022编译(在多字节编码、admin权限下编译)