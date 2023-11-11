#include "airMemSys.h"
#include <Airkit/Core/STL/airIList.h>
#include <Airkit/Core/STL/airAlign.h>
#include <Airkit/Core/Plat/airPrint.h>
namespace airkit
{
#define _Mem_Err_Bounds                                                                              \
    {                                                                                                \
        Print::error("MemSys: Memory out of bounds!\n");                                             \
        Print::error("\tfile: %s\n\tfunc: %s\n\t line:%d\n", this_file(), this_func(), this_line()); \
        exit(-1);                                                                                    \
    }

    // ---------------------内存系统相关枚举值---------------------------
    struct MemSysEnum
    {
        enum : uintptr_t
        {
            HugePageSize = 4 * 1024 * 1024, // 巨页内存大小 4M 4 * 1024 * 1024
            PoolPageSize = 4 * 1024,        // 内存池大小 4K 4 * 1024
            BlockMaxSize = 512,             // 最大内存块

#ifdef Air_CPU_Bit64
            BlockAlignSize = 8,  // 内存地址块对齐大小
            IndexShiftValue = 3, // 查找引索的偏移值
#else
            BlockAlignSize = 16, // 内存地址块对齐大小
            IndexShiftValue = 4, // 查找引索的偏移值
#endif

            PoolCount = BlockMaxSize / BlockAlignSize, // 内存池的种类
            HugeNodeCount = 6,                         // 可用巨页管理节点数量
            PageNodeCount = 14,                        // 可用池页管理节点数量
            BigNodeCount = 14,                         // 大内存块管理节点数量

            CheckBoxValue = 0xAAAAAAAA, // 内存越界检查值
        };
    };

    // -------------------------------基础管理数据结构----------------------------------
    // 子页块管理节点
    struct MemChunkNode
    {
        uintptr_t mPage;      // 页、块的地址
        uintptr_t mNextChunk; // 下一个可用的池页/块

        uint32_t mTotal : 31; // 分页/块总数量
        uint32_t mUsing : 1;  // 该节点是否在使用
        int32_t mCount;       // 可用分页/块数量

        // 分页
        inline void paging(uintptr_t page, size_t pagesize, size_t blksize)
        {
            mCount = MemSys::paging(page, pagesize, blksize, (uintptr_t)this);
            mTotal = mCount;
            mUsing = 1;
            mNextChunk = page;
            mPage = page;
        }
        // 是否使用完成
        inline bool isempty() const { return mNextChunk == (uintptr_t)this; }
        // true:全部空闲,可以释放内存页
        inline bool canfree() const { return mCount == mTotal; }
        // 获取内存块，没有获得返回nullptr
        inline uintptr_t get()
        {

            uintptr_t ret = 0;
            // 存在可用的
            if (mNextChunk != (uintptr_t)this)
            {
                ret = mNextChunk;
                // 更新下一个可用块地址
                mNextChunk = (uintptr_t) * ((void **)ret);
                --mCount;
            }

            return ret;
        }
        inline void back(uintptr_t blk)
        {
            // 是否发生内存越界
            if (blk == mNextChunk)
            {
                _Mem_Err_Bounds;
            }

            // 1、如果为空，直接放置
            if (mNextChunk == (uintptr_t)this)
            {
                mNextChunk = blk;
                *((void **)blk) = this;
                ++mCount;
                return;
            }
            // 2、如果存在可用就需要判断块的相对位置
            // 2.1、当前块在可用块之后
            if (mNextChunk < blk)
            {
                auto pre = mNextChunk;
                auto next = (uintptr_t) * ((void **)pre);
                while (next < blk && next != (uintptr_t)this)
                {
                    pre = next;
                    next = (uintptr_t) * ((void **)pre);
                }
                // 存在关系
                // pre < node < next
                *((void **)pre) = (void *)blk;
                *((void **)blk) = (void *)next;
                ++mCount;
                return;
            }
            // 2.2、当前块在可用块之前
            if (blk < mNextChunk)
            {
                *((void **)blk) = (void *)mNextChunk;
                mNextChunk = blk;
                ++mCount;
                return;
            }
            // 2.3、发生内存越界
            _Mem_Err_Bounds;
        }
    };

    // 主管理节点模板
    template <typename Node, const uintptr_t pagecol>
    struct MemHugePage : public IListNode<MemHugePage<Node, pagecol>>
    {
        uint32_t mTotal : 31;  // 分页/块总数量
        uint32_t mUsing : 1;   // 该节点是否在使用
        int32_t mCount;        // 可用分页/块数量
        uintptr_t mNextChunk;  // 下一个可用的池页/块
        Node mChunks[pagecol]; // 管理的页节点信息

        MemHugePage()
        {
            memset(this, 0, sizeof(*this));
            mCount = pagecol;
            mTotal = mCount;
            mUsing = 0;
            mNextChunk = (uintptr_t)mChunks;
            size_t i = 0;
            for (; i < mCount - 1; ++i)
            {
                mChunks[i].mNextChunk = (uintptr_t)&mChunks[i + 1];
            }
            mChunks[i].mNextChunk = (uintptr_t)this;
        }

        // 是否使用完成
        inline bool isempty() const { return mNextChunk == (uintptr_t)this; }
        // true:全部空闲,可以释放内存页
        inline bool canfree() const { return mCount == mTotal; }
        // 获取一个管理节点:无节点时返回nullptr
        inline Node *get()
        {
            Node *ret = nullptr;
            if (mNextChunk != (uintptr_t)this)
            {
                ret = (Node *)mNextChunk;
                // 更新下一个可用地址节点
                mNextChunk = ret->mNextChunk;
                --mCount; // 更新可用节点数量
            }

            return ret;
        }
        // 归还一个管理节点
        inline void back(Node *node)
        {
            // 是否发生内存越界
            if ((uintptr_t)node == mNextChunk)
            {
                _Mem_Err_Bounds;
            }

            // 1、如果为空，直接放置
            if (mNextChunk == (uintptr_t)this)
            {
                mNextChunk = (uintptr_t)node;
                node->mNextChunk = (uintptr_t)this;
                ++mCount; // 更新可用节点数量
                return;
            }
            // 2、如果存在可用就需要判断块的相对位置
            // 2.1、当前块在可用块之后
            if (mNextChunk < (uintptr_t)node)
            {
                auto pre = (Node *)mNextChunk;
                auto next = (Node *)(pre->mNextChunk);
                while (next < node && (uintptr_t)next != (uintptr_t)this)
                {
                    pre = next;
                    next = (Node *)(pre->mNextChunk);
                }
                // 存在关系
                // pre < node < next
                pre->mNextChunk = (uintptr_t)node;
                node->mNextChunk = (uintptr_t)next;
                ++mCount; // 更新可用节点数量
                return;
            }
            // 2.2、当前块在可用块之前
            if ((uintptr_t)node < mNextChunk)
            {
                node->mNextChunk = mNextChunk;
                mNextChunk = (uintptr_t)node;
                ++mCount; // 更新可用节点数量
                return;
            }
            // 2.3、发生内存越界
            _Mem_Err_Bounds;
        }
    };
    using MemHugeNode = MemHugePage<MemChunkNode, MemSysEnum::HugeNodeCount>; // 巨页管理节点
    using MemPageNode = MemHugePage<MemChunkNode, MemSysEnum::PageNodeCount>; // 池页管理节点
                                                                              // 大内存块管理节点
    struct MemBigNodeItem
    {
        uintptr_t mBlock;                            // 大内存块地址
        uintptr_t mSize : sizeof(uintptr_t) * 8 - 1; // 大内存块大小
        uintptr_t mUsing : 1;                        // 该管理项是否已使用
    };
    // 大内存块管理节点
    struct MemBigNode : public IListNode<MemBigNode>
    {
        uint32_t mTotal : 31;                             // 管理节点总数量
        uint32_t mUsing : 1;                              // 该节点是否在使用
        int32_t mCount;                                   // 可用管理节点数量
        uintptr_t mNextItem;                              // 下一个可用节点
        MemBigNodeItem mBlocks[MemSysEnum::BigNodeCount]; // 管理的页节点信息

        // 初始化管理节点
        MemBigNode()
        {
            memset(this, 0, sizeof(*this));
            // 按照PageChunkInfo规则初始化节点
            mCount = MemSysEnum::BigNodeCount;
            mTotal = mCount;
            mUsing = 0;
            mNextItem = (uintptr_t)mBlocks;
            size_t i = 0;
            for (; i < mCount - 1; ++i)
            {
                mBlocks[i].mBlock = (uintptr_t)&mBlocks[i + 1];
            }
            mBlocks[i].mBlock = (uintptr_t)this;
        }
        // 是否节点用完
        inline bool isempty() const { return mNextItem == (uintptr_t)this; }
        // 是否全部空闲,可以释放管理节点
        inline bool canfree() const { return mCount == mTotal; }

        // 获取一个管理节点:无节点时返回nullptr
        inline MemBigNodeItem *get()
        {
            MemBigNodeItem *ret = nullptr;
            if (mNextItem != (uintptr_t)this)
            {
                ret = (MemBigNodeItem *)mNextItem;
                // 更新下一个可用地址节点
                mNextItem = ret->mBlock;
                --mCount; // 更新可用节点数量
                ret->mUsing = 1;
            }

            return ret;
        }
        // 归还一个管理节点
        inline void back(MemBigNodeItem *item)
        {
            // 是否发生内存越界
            if ((uintptr_t)item == mNextItem)
            {
                _Mem_Err_Bounds;
            }

            // 1、如果为空，直接放置
            if (mNextItem == (uintptr_t)this)
            {
                mNextItem = (uintptr_t)item;
                item->mBlock = (uintptr_t)this;
                item->mSize = 0;
                item->mUsing = 0;
                ++mCount;
                return;
            }
            // 2、如果存在可用就需要判断块的相对位置
            // 2.1、当前块在可用块之后
            if (mNextItem < (uintptr_t)item)
            {
                auto pre = (MemBigNodeItem *)mNextItem;
                auto next = (MemBigNodeItem *)(pre->mBlock);
                while (next < item && (uintptr_t)next != (uintptr_t)this)
                {
                    pre = next;
                    next = (MemBigNodeItem *)(pre->mBlock);
                }
                // 存在关系
                // pre < node < next
                pre->mBlock = (uintptr_t)item;
                item->mBlock = (uintptr_t)next;
                item->mSize = 0;
                item->mUsing = 0;
                ++mCount;
                return;
            }
            // 2.2、当前块在可用块之前
            if ((uintptr_t)item < mNextItem)
            {
                item->mBlock = mNextItem;
                item->mSize = 0;
                item->mUsing = 0;
                mNextItem = (uintptr_t)item;
                ++mCount;
                return;
            }
            // 2.3、发生内存越界
            _Mem_Err_Bounds;
        }
    };

    // --------------------------------巨页分配管理中心-------------------------------
    struct MemHugeCenter
    {
        static MemHugeCenter Instance; // 唯一实例
        IList<MemHugeNode> mRoot;      // 节点管理中心
        Spinlock mLock;                // 多线程同步锁

        // 终止化管理中心
        void terminal();

        // 出借池页
        uintptr_t getPoolPage();
        // 归还池页
        void backPoolPage(uintptr_t page);
    };
    MemHugeCenter MemHugeCenter::Instance;

    void MemHugeCenter::terminal()
    {
        Spinlocker holder(mLock);
        // 先释放巨页，后释放管理节点
        auto iter = mRoot.entry();
        while (iter != mRoot.root())
        {
            // 如果从管理节点中查询使用的管理项
            for (int i = 0; i < iter->mTotal; ++i)
            {
                auto &item = iter->mChunks[i];
                // 该节点未使用
                if (0 == item.mUsing)
                    continue;
                free((void *)item.mPage);
                MemSys::constructor(&item);
                iter->back(&item);
            }
            auto node = iter;
            iter = iter->getNext();
            node->remove();
            free(node);
        }
    }

    uintptr_t MemHugeCenter::getPoolPage()
    {
        Spinlocker holder(mLock);
        uintptr_t retpage = 0;
        // 1、从管理节点中获取可用页
        for (auto iter = mRoot.entry();
             iter != mRoot.root();
             iter = iter->getNext())
        {
            // 如果从管理节点中查询
            for (int i = 0; i < iter->mTotal; ++i)
            {
                auto &item = iter->mChunks[i];
                // 该节点未使用
                if (0 == item.mUsing)
                    continue;
                // 存在可用页,获取并返回
                if (false == item.isempty())
                {
                    retpage = item.get();
                    if (0 == retpage)
                    {
                        _Mem_Err_Bounds;
                    }
                    return retpage;
                }
            }
        }
        // ---------------没有找到可用池页----------------
        // 2、先分配巨页，再获取页
        // 2.1 获取管理节点中的巨页管理项
        MemChunkNode *item = nullptr;
        for (auto iter = mRoot.entry();
             iter != mRoot.root();
             iter = iter->getNext())
        {
            if (true == iter->isempty())
            {
                item = iter->get();
                if (nullptr == item)
                {
                    _Mem_Err_Bounds;
                }
                break;
            }
        }
        // 2.2 如果没有获得巨页管理项，表明管理节点中的管理项已经用完了
        // 需要重新获取管理节点
        if (nullptr == item)
        {
            // 分配并初始化管理节点
            MemHugeNode *node = (MemHugeNode *)malloc(sizeof(MemHugeNode));
            MemSys::constructor<MemHugeNode>(node);
            // 插入管理链表中
            mRoot.insertEntry(node);
            node->mUsing = 1; // 标记该管理节点正在使用
            // 获取管理项
            item = node->get();
            // 再次验证一下
            if (0 == item)
            {
                _Mem_Err_Bounds;
            }
        }

        // 2.3 分配巨页并初始化
        uintptr_t huge = (uintptr_t)malloc(MemSysEnum::HugePageSize);
        item->paging(huge, MemSysEnum::HugePageSize, MemSysEnum::PoolPageSize);
        // 2.4 获取池页
        retpage = item->get();
        if (0 == retpage)
        {
            _Mem_Err_Bounds;
        }
        return retpage;
    }
    void MemHugeCenter::backPoolPage(uintptr_t page)
    {
        Spinlocker holder(mLock);
        uintptr_t pageAddr = page;
        // 1、找到Page所属的巨页
        for (auto it = mRoot.entry();
             it != mRoot.root();
             it = it->getNext())
        {
            for (int i = 0; i < it->mTotal; ++i)
            {
                // 巨页管理项
                auto &item = it->mChunks[i];
                // 该节点未使用
                if (0 == item.mUsing)
                    continue;
                // 该页在巨页内部
                uintptr_t start = (uintptr_t)item.mPage;
                uintptr_t end = start + MemSysEnum::HugePageSize;
                if (start <= pageAddr && pageAddr < end)
                {
                    // 清空一下池页
                    memset((void *)page, 0, MemSysEnum::PoolPageSize);
                    item.back(page);
                    // 2、判断巨页管理项中的巨页是否可以释放
                    if (true == item.canfree())
                    {
                        free((void *)item.mPage);
                        MemSys::constructor(&item);
                        it->back(&item);
                    }
                    // 3、判断巨页管理节点是否可以释放
                    if (true == it->canfree())
                    {
                        // 先从链中移除节点
                        it->remove();
                        // 再释放节点内存
                        free(it);
                    }
                    return;
                }
            }
        }
    }

    // --------------------------------池页分配管理中心-------------------------------

    struct MemPools : public IMemPools
    {

        IList<MemPageNode> mPools[MemSysEnum::PoolCount]; // 固定块内存池
        IList<MemBigNode> mBigs;                          // 大内存块链表
        Spinlock mLock[MemSysEnum::PoolCount + 1];        // 多线程同步锁

        // 在内存池上分配上分配内存块
        virtual void *alloc(size_t size) override;

        // 释放内存对象
        virtual void dealloc(void *blk, size_t size) override;

        inline void terminal();

        static MemPools Instance; // 全局实例
    };
    MemPools MemPools::Instance;

    void *MemPools::alloc(size_t size)
    {
        // 计数池的锁引索
        size_t total = size; // 需要分配的内存大小
        // 计数对齐大小
        total = Align::up<size_t>(total, (size_t)MemSysEnum::BlockAlignSize);
        const size_t index = total > MemSysEnum::BlockMaxSize ? MemSysEnum::PoolCount : ((total >> MemSysEnum::IndexShiftValue) - 1);

        // 锁定所关联的池
        Spinlocker hoder(mLock[index]);

        // 1、大内存分配
        if (MemSysEnum::PoolCount == index)
        {
            // 1.1、从管理节点获取可用管理项
            MemBigNodeItem *item = nullptr;
            for (auto it = mBigs.entry();
                 it != mBigs.root();
                 it = it->getNext())
            {
                if (true == it->isempty())
                {
                    item = it->get();
                    if (nullptr == item)
                    {
                        _Mem_Err_Bounds;
                    }
                    break;
                }
            }
            // 1.2、 如果没有获得巨页管理项，表明管理节点中的管理项已经用完了
            // 需要重新获取管理节点
            if (nullptr == item)
            {
                // 分配并初始化管理节点
                MemBigNode *node = (MemBigNode *)malloc(sizeof(MemBigNode));
                MemSys::constructor<MemBigNode>(node);
                //node->init();
                // 插入管理链表中
                mBigs.insertEntry(node);
                node->mUsing = 1; // 标记该管理节点正在使用
                // 获取管理项
                item = node->get();
                // 再次验证一下
                if (nullptr == item)
                {
                    _Mem_Err_Bounds;
                }
            }
            // 1.3 分配内存块并初始化
            auto retblk = malloc(total);
            item->mBlock = (uintptr_t)retblk;
            item->mUsing = 1;
            item->mSize = total;
            return retblk;
        }

        // 2、从池中分配

        auto &root = mPools[index];
        // 2.1、先从管理节点中获取可用块
        for (auto iter = root.entry();
             iter != root.root();
             iter = iter->getNext())
        {
            // 如果从管理节点中查询
            for (int i = 0; i < iter->mTotal; ++i)
            {
                auto &item = iter->mChunks[i];
                // 该节点未使用
                if (0 == item.mUsing)
                    continue;
                // 存在可用块,获取并跳转
                if (false == item.isempty())
                {
                    auto retblk = item.get();
                    return (void *)retblk;
                }
            }
        }
        // 2.2、没有获得可用块
        // 2.2.1 先分配池页，再获取块
        // 2.2.2 获取管理节点中的池页管理项
        MemChunkNode *item = nullptr;
        for (auto iter = root.entry();
             iter != root.root();
             iter = iter->getNext())
        {
            if (true == iter->isempty())
            {
                item = iter->get();
                if (nullptr == item)
                {
                    _Mem_Err_Bounds;
                }
                break;
            }
        }
        // 2.3 如果没有获得池页管理项，表明管理节点中的管理项已经用完了
        // 需要重新获取管理节点
        if (nullptr == item)
        {
            // 分配并初始化管理节点
            MemPageNode *node = (MemPageNode *)malloc(sizeof(MemPageNode));
            MemSys::constructor<MemPageNode>(node);
            //node->init();
            // 插入管理链表中
            root.insertEntry(node);
            node->mUsing = 1; // 标记该管理节点正在使用
            // 获取管理项
            item = node->get();
            // 再次验证一下
            if (nullptr == item)
            {
                _Mem_Err_Bounds;
            }
        }
        // 2.4分配池页并初始化
        uintptr_t page = MemHugeCenter::Instance.getPoolPage();
        item->paging(page, MemSysEnum::PoolPageSize, total);
        // 2.5 获取可用块并初始化
        auto retblk = item->get();
        return (void *)retblk;
    }

    void MemPools::dealloc(void *blk, size_t size)
    {
        // 计数池的锁引索
        size_t total = size; // 需要分配的内存大小
        // 计数对齐大小
        total = Align::up<size_t>(total, (size_t)MemSysEnum::BlockAlignSize);
        const size_t index = total > MemSysEnum::BlockMaxSize ? MemSysEnum::PoolCount : ((total >> MemSysEnum::IndexShiftValue) - 1);

        // 锁定所关联的池
        Spinlocker hoder(mLock[index]);

        // 分配的大内存
        if (MemSysEnum::PoolCount == index)
        {
            for (auto it = mBigs.entry();
                 it != mBigs.root();
                 it = it->getNext())
            {
                // 1.1、查找到管理项
                for (int i = 0; i < it->mTotal; ++i)
                {
                    // 大内存块管理项
                    auto &item = it->mBlocks[i];
                    // 该节点未使用
                    if (0 == item.mUsing)
                        continue;
                    // 判断是否是该页
                    if (item.mBlock == (uintptr_t)blk)
                    {
                        // 1.2、释放内存和退还管理项
                        free(blk);
                        it->back(&item);
                        // 1.3、判断管理节点是否可以释放
                        if (true == it->canfree())
                        {
                            // 先从链中移除节点
                            it->remove();
                            // 再释放节点内存
                            free(it);
                        }
                        return;
                    }
                }
            }
        }
        // 从池中分配
        auto &root = mPools[index];
        uintptr_t blkAddr = (uintptr_t)blk;
        // 2.1、查找在哪个管理项
        for (auto it = root.entry();
             it != root.root();
             it = it->getNext())
        {
            for (int i = 0; i < it->mTotal; ++i)
            {
                // 池页管理项
                auto &item = it->mChunks[i];
                // 该节点未使用
                if (0 == item.mUsing)
                    continue;
                // 该内存块在池页内部
                uintptr_t start = item.mPage;
                uintptr_t end = start + MemSysEnum::PoolPageSize;
                if (start <= blkAddr && blkAddr < end)
                {
                    // 2.2、清空一下内存块
                    memset(blk, 0, total);
                    item.back((uintptr_t)blk);
                    // 2.3、判断池页管理项中的池页是否可以释放
                    if (true == item.canfree())
                    {
                        MemHugeCenter::Instance.backPoolPage(item.mPage);
                        MemSys::constructor<MemChunkNode>(&item);
                        it->back(&item);
                    }
                    // 2.4、判断池页管理节点是否可以释放
                    if (true == it->canfree())
                    {
                        // 先从链中移除节点
                        it->remove();
                        // 再释放节点内存
                        free(it);
                    }
                    return;
                }
            }
        }
        // 没有找到内存
        Print::warning("错误释放非管属的内存块：%a\n", blk);
        exit(-1);
    }

    inline void MemPools::terminal()
    {
        uint32_t notFreeCnt = 1; // 为释放内存块计数

        // 1、查看内存池释放情况
        for (size_t index = 0; index < MemSysEnum::PoolCount; ++index)
        {
            // 计数池内的内存块大小
            size_t poolblk = (index + 1) << MemSysEnum::IndexShiftValue;
            // 1.1分别查看每个池
            auto &pool = mPools[index];
            for (auto it = pool.entry();
                 it != pool.root();)
            {
                // 1.2、查找管理项中未释放的内存块
                for (int i = 0; i < it->mTotal; ++i)
                {
                    // 池页管理项
                    auto &item = it->mChunks[i];
                    // 该节点未使用
                    if (0 == item.mUsing)
                        continue;
                    // 1.3、查看池页类的内存释放情况
                    auto start = item.mPage;
                    auto next = item.mNextChunk;
                    for (size_t i = 0; i < item.mTotal; ++i)
                    {
                        auto blk = (size_t)start + i * poolblk; // 当前块的地址
                        // 当前块未被使用
                        if (blk == (size_t)next)
                        {
                            next = *(size_t *)next;
                            continue;
                        }

                        ++notFreeCnt;
                    }

                    // 1.5、释放池页
                    MemHugeCenter::Instance.backPoolPage(item.mPage);
                    MemSys::constructor<MemChunkNode>(&item);
                    it->back(&item);
                }
                // 1.6、释放管理节点
                auto cit = it;
                it = it->getNext();
                cit->remove();
                free(cit);
            }
        }
        // 2、查看大内存释放情况
        for (auto it = mBigs.entry();
             it != mBigs.root();)
        {
            // 2.1、查找管理项中未释放的内存块
            for (int i = 0; i < it->mTotal; ++i)
            {
                // 大内存块管理项
                auto &item = it->mBlocks[i];
                // 该节点未使用
                if (0 == item.mUsing)
                    continue;
                // 2.3释放大内存块
                free((void *)item.mBlock);
                it->back(&item);
                ++notFreeCnt;
            }
            // 2.4、释放管理节点
            auto cit = it;
            it = it->getNext();
            cit->remove();
            free(cit);
        }
        // 3、总结一下释放情况
        --notFreeCnt;
        if (0 != notFreeCnt)
            Print::warning("Number of unreleased memory blocks：%u\n", notFreeCnt);
    }

    //------------------------------对外接口-----------------------------------

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

    void MemSys::init()
    {
        MemSys::constructor<MemHugeCenter>(&MemHugeCenter::Instance);
        MemSys::constructor<MemPools>(&MemPools::Instance);
    }

    void MemSys::terminal()
    {
        MemHugeCenter::Instance.terminal();
    }
    IMemPools &MemSys::GlobalPools(MemPools::Instance);

}