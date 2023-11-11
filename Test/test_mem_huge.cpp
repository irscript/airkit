#include <Airkit/Core/Memory/airMemSys.h>

using namespace airkit;

int main(int argc, char **argv)
{
    MemSys::init();
    const uintptr_t chkval = 0xAAAAAAAAAAAAAAAA;
    uintptr_t *arr[2048];
    for (size_t i = 0; i < ARRAYSIZE(arr); i++)
    {
        arr[i] = (uintptr_t *)MemSys::alloc((i + 1) * sizeof(uintptr_t));
        *arr[i] = chkval;
    }
    size_t errcnt = 0;
    for (size_t i = 0; i < ARRAYSIZE(arr); i++)
    {
        if (*arr[i] != chkval)
            errcnt++;
        MemSys::dealloc(arr[i], (i + 1) * sizeof(uintptr_t));
    }

    MemSys::terminal();
    return errcnt;
}