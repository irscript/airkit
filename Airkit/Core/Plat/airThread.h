#ifndef __AIRTHREAD_INC__
#define __AIRTHREAD_INC__

#include <Airkit/Precompiled.h>

namespace airkit
{
    // 自旋锁
    struct Spinlock
    {
        Spinlock(int32_t shared = false);
        ~Spinlock();

        // 锁定
        void lock();
        int trylock();
        void unlock();

    protected:
        uintptr_t mHandle; // 互斥量句柄
    };

    // 自旋锁持有者
    struct Spinlocker
    {
        inline Spinlocker(Spinlock &handle) : mHandle(handle) { mHandle.lock(); }
        inline ~Spinlocker() { mHandle.unlock(); }

    protected:
        Spinlock &mHandle; // 互斥量句柄
    };

    // 互斥量
    struct Mutex
    {
    public:
        Mutex();
        ~Mutex();

        // 锁定
        void lock();
        bool trylock();
        void unlock();

    protected:
        uintptr_t mHandle; // 互斥量句柄
    };
    struct MutexLock
    {
    public:
        inline MutexLock() { mMutex.lock(); }
        inline ~MutexLock() { mMutex.unlock(); }

    protected:
        Mutex mMutex; // 互斥量句柄
    };

    // 条件变量
    struct Condition
    {
        Condition();
        ~Condition();

        void emit();
        void wait();

    protected:
        uintptr_t mMutex;
        uintptr_t mSemp;
    };

    using ThreadID = uintptr_t;
    // 线程定义
    struct Thread
    {
    public:
        Thread();
        ~Thread();
    };

// 实现
#ifdef Air_Plat_Pthread
#include <Airkit/Core/Plat/airPThread.inl>
#else
#error "线程等同步类未实现！"
#endif

}

#endif // __AIRTHREAD_INC__