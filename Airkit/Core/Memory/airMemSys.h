#ifndef __AIRMEMSYS__H__
#define __AIRMEMSYS__H__

#include <Airkit/Precompiled.h>
namespace airkit
{
    // 内存分配系统
    struct MemSys
    {
        /**
         * @brief 内存分块、分页函数
         * @param page 需要划分的内存页的基地址
         * @param pagesize 内存页的大小
         * @param blksize 需要划分的块大小
         * @param end 最后一块的指向地址
         * @return 返回划分的内存块数量
         */
        static size_t paging(uintptr_t page, size_t pagesize, size_t blksize, uintptr_t end);
    };
}
#endif // __AIRPOOLMEM_INC__