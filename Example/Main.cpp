#include <stdio.h>
#include <Airkit/Core/Plat/airPrint.h>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "zh_CN.UTF-8");
    airkit::Print::print(2, "12345 %%");
    airkit::Print::info("测试:%3d", 1);
    airkit::Print::warning("警告:%3d", 1);
    airkit::Print::error("错误:%3d", 1);
    airkit::Print::track("跟踪:%3d", 1);
    printf("\n456\n");
    return 0;
}