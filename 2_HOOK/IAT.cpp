#include <iostream>
#include <Windows.h>
//包含Detour的头文件和库文件
#include "./include/detours.h"
#pragma comment (lib,"detours.lib")
using namespace std;

//保存函数原型（用指针存储要拦截的API）
void (*OldGetLocalTime)(LPSYSTEMTIME) = GetLocalTime;

//拦截后要执行的操作（这里是将时间的分改为52）
void NewGetLocalTime(LPSYSTEMTIME lpSystemTime) {
	OldGetLocalTime(lpSystemTime);
	lpSystemTime->wMinute = 52;
}

//下钩子函数
void StartHook() {
	//开始事务
	DetourTransactionBegin();
	//更新线程信息
	DetourUpdateThread(GetCurrentThread());
	//将拦截的函数附加到原函数的地址上
	DetourAttach(&(PVOID&)OldGetLocalTime, NewGetLocalTime);
	//结束事务
	DetourTransactionCommit();
}

//撤钩子函数
void EndHook() {
	//开始detours事务
	DetourTransactionBegin();
	//更新线程信息 
	DetourUpdateThread(GetCurrentThread());
	//将拦截的函数从原函数的地址上解除
	DetourDetach(&(PVOID&)OldGetLocalTime, NewGetLocalTime);
	//结束detours事务
	DetourTransactionCommit();
}

int main()
{
	//获取本地时间
	SYSTEMTIME time, time2, time3;
	GetLocalTime(&time);
	cout << "下钩子前调用GetLocalTime：" << time.wHour << ":" << time.wMinute << endl;
	//下钩子
	StartHook();

	//下钩子后再次获取本地时间
	GetLocalTime(&time2);
	cout << "下钩子后调用GetLocalTime：" << time2.wHour << ":" << time2.wMinute << endl;

	//撤钩子
	EndHook();

	//撤钩子后再次获取本地时间
	GetLocalTime(&time3);
	cout << "撤钩子后调用GetLocalTime：" << time3.wHour << ":" << time3.wMinute << endl;
	return 0;
}
