#include "airMemSys.h"
namespace airkit
{

    size_t MemSys::paging(uintptr_t page, size_t pagesize, size_t blksize, uintptr_t end)
    {
        // 先清空一下内存
        memset((void *)page, 0, pagesize);

        // 存储可分配块
        uintptr_t mNextItem = page; // 当本页内块分配完后，指向 this
        // 计算分块数量
        size_t count = pagesize / blksize;

        uintptr_t start = page;
        auto blk = (void **)start;
        // 初始化可用分块链
        for (size_t i = 0; i < count - 1; ++i)
        {
            auto next = (start + blksize);
            *blk = (void *)next;
            blk = (void **)next;
            start = next;
        }
        // 初始化最后一块
        *blk = (void *)end;

        return count;
    }
}