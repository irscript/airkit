#include <Airkit/Core/Memory/airMemSys.h>

uintptr_t gbuffer[10] = {};
// 内存分页测试
int main(int argc, char **argv)
{
    auto cnt = airkit::MemSys::paging((uintptr_t)gbuffer, sizeof(gbuffer), 8, (uintptr_t)gbuffer);
    uintptr_t base = (uintptr_t)gbuffer;
    if (cnt != 10 || gbuffer[9] != base)
        return cnt;

    for (int i = 0; i < 8; i++)
    {
        if (gbuffer[i + 1] - gbuffer[i] != 8)
            return i;
    }

    return 0;
}
