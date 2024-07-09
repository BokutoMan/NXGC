#include <iostream>
#include <windows.h>  // 包含Windows.h头文件以使用Sleep函数

int main() {
    std::cout << "Sleeping for 100 seconds..." << std::endl;
    
    // 调用Sleep函数来使程序沉睡100秒钟（单位为毫秒，所以100秒是100000毫秒）
    Sleep(100000);

    std::cout << "Awake after 100 seconds!" << std::endl;

    return 0;
}
