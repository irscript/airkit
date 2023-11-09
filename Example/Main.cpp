#include <stdio.h>
#include <Airkit/Core/Plat/airPrint.h>
int main(int argc, char *argv[])
{
    airkit::Print::print(2, "12345 %%");
    airkit::Print::info("test:%3d", 1);
    airkit::Print::warning("test:%3d", 1);
    airkit::Print::error("test:%3d", 1);
    airkit::Print::track("test:%3d", 1);
    printf("\n456\n");
    return 0;
}