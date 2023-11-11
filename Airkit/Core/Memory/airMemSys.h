#ifndef __AIRMEMSYS__H__
#define __AIRMEMSYS__H__

#include <Airkit/Precompiled.h>
namespace airkit
{
#ifdef Air_Enable_MemDbg
    // 内存调试信息结构
    struct MemDebugInfo
    {
        uint32_t mBoxv; // 内存越界检查值
        uint32_t mLine; // 所分配内存的文件行
        cstring mFile;  // 所分配内存的文件位置
    };
#endif
    // 内存池接口
    struct IMemPools
    {

        // 在内存池上分配上分配内存块
        virtual void *alloc(size_t size) = 0;

        // 释放内存对象
        virtual void dealloc(void *blk, size_t size) = 0;
    };

    // 内存分配系统
    struct MemSys
    {
        // c++构造函数调用
        template <typename Type, typename... Args>
        inline static Type *constructor(Type *obj, Args... args)
        {
            Type *ret = (Type *)obj;
            ::new (obj) Type(args...);
            return ret;
        }
        // c++析构函数手动调用
        template <typename Type>
        inline static void destructor(Type *obj) { obj->~Type(); }

        /**
         * @brief 内存分块、分页函数
         * @param page 需要划分的内存页的基地址
         * @param pagesize 内存页的大小
         * @param blksize 需要划分的块大小
         * @param end 最后一块的指向地址
         * @return 返回划分的内存块数量
         */
        static size_t paging(uintptr_t page, size_t pagesize, size_t blksize, uintptr_t end);

        /**
         * @brief 初始化内存管理系统：只调用一次
         */
        static void init();
        /**
         * @brief 终止化内存管理系统：只调用一次
         */
        static void terminal();

        // 在内存池上分配上分配内存块
        inline static void *alloc(size_t size) { return GlobalPools.alloc(size); }

        // 释放内存对象
        inline static void dealloc(void *blk, size_t size) { GlobalPools.dealloc(blk, size); }

    private:
        static IMemPools &GlobalPools;
    };
}
#endif // __AIRPOOLMEM_INC__