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
        Mutex(int32_t shared = false);
        ~Mutex();

        // 锁定
        void lock();
        // 时间段内锁定互斥量
        bool timedlock(size_t ns);
        bool trylock();
        void unlock();

    protected:
        friend struct Condition;
        uintptr_t mHandle; // 互斥量句柄
    };
    struct MutexLock
    {
    public:
        inline MutexLock(Mutex &handle) : mHandle(handle) { mHandle.lock(); }
        inline ~MutexLock() { mHandle.unlock(); }

    protected:
        Mutex &mHandle; // 互斥量句柄
    };

    // 条件变量
    struct Condition
    {
        Condition(int32_t shared = false);
        ~Condition();

        // 锁定
        inline void lock() { mMutex.lock(); }
        // 解锁
        inline void unlock() { mMutex.unlock(); }
        // 等待
        void wait();
        bool timedwait(size_t ns);
        // 通知
        void signal();
        void broadcast();

    protected:
        uintptr_t mHandle; // 条件变量句柄
        Mutex mMutex;      // 对应的互斥量
    };

    // 线程定义
    struct Thread
    {
        // 线程ID
        using ID = uintptr_t;
        // 静态工作函数
        using WorkerFunc = void *(*)(void *);

        inline Thread() : mHandle(0) {}
        inline ~Thread() { mHandle = 0; }

        bool join(uintptr_t &retval);
        bool detch();

        bool setName(cstring name);
        bool getName(char *name, size_t len);

        bool cancel();
        bool signal(int sig);

        void run(WorkerFunc task, void *userptr);

        static bool setName(ID thd, cstring name);
        static bool getName(ID thd, char *name, size_t len);

        // 获取当前线程ID
        static ID getSelf();
        // 初始化主线程ID
        static inline void InitMainID() { mMainID = getSelf(); }
        // 是否是主线程
        static inline bool isMainID() { return mMainID == getSelf(); }
        // 获取主线程ID
        static inline ID getMainID() { return mMainID; }

        static void exit(uintptr_t code);
        static bool cancel(ID thd);
        static bool signal(ID thd, int sig);

    protected:
        uintptr_t mHandle; // 线程句柄
        static ID mMainID; // 组线程
    };

// 实现
#ifdef Air_Plat_Pthread
#include <Airkit/Core/Plat/airPThread.inl>
#else
#error "线程等同步类未实现！"
#endif

}

#endif // __AIRTHREAD_INC__