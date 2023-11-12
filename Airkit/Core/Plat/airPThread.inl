
inline Spinlock::Spinlock(int32_t shared)
{
    pthread_spinlock_t *spin = (pthread_spinlock_t *)&mHandle;
    auto res = pthread_spin_init(spin, shared == true ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);
}

inline Spinlock::~Spinlock()
{
    auto res = pthread_spin_destroy((pthread_spinlock_t *)&mHandle);
    mHandle = 0;
}

inline void Spinlock::lock()
{
    auto res = pthread_spin_lock((pthread_spinlock_t *)&mHandle);
}

inline int Spinlock::trylock()
{
    return pthread_spin_trylock((pthread_spinlock_t *)&mHandle) == 0;
}

inline void Spinlock::unlock()
{
    auto res = pthread_spin_unlock((pthread_spinlock_t *)&mHandle);
}
inline Mutex::Mutex(int32_t shared)
{
    mHandle = 0;
    pthread_mutex_t *mutex = (pthread_mutex_t *)&mHandle;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutexattr_setpshared(&attr, shared == false ? PTHREAD_PROCESS_PRIVATE : PTHREAD_PROCESS_SHARED);
    auto res = pthread_mutex_init(mutex, &attr);
}
inline Mutex::~Mutex()
{
    // 释放互斥量
    pthread_mutex_t *mutex = (pthread_mutex_t *)&mHandle;
    auto res = pthread_mutex_destroy(mutex);
    mHandle = 0;
}
inline void Mutex::lock()
{
    auto res = pthread_mutex_lock((pthread_mutex_t *)&mHandle);
}
inline bool Mutex::timedlock(size_t ns)
{
    // 获取当前时间
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    // 计算超时时间
    timeout.tv_nsec += ns;

    return pthread_mutex_timedlock((pthread_mutex_t *)&mHandle, &timeout) == 0;
}
inline bool Mutex::trylock()
{
    return pthread_spin_trylock((pthread_mutex_t *)&mHandle) == 0;
}
inline void Mutex::unlock()
{
    auto res = pthread_spin_unlock((pthread_mutex_t *)&mHandle);
}

inline Condition::Condition(int32_t shared)
    : mHandle(0), mMutex(shared)
{
    pthread_cond_t *cond = (pthread_cond_t *)&mHandle;
    auto res = pthread_cond_init(cond, nullptr);
}
inline Condition::~Condition()
{
    pthread_cond_t *cond = (pthread_cond_t *)&mHandle;
    auto res = pthread_cond_destroy(cond);
}

// 等待
inline void Condition::wait()
{
    pthread_cond_t *cond = (pthread_cond_t *)&mHandle;
    pthread_mutex_t *mutex = (pthread_mutex_t *)&mMutex.mHandle;
    auto res = pthread_cond_wait(cond, mutex);
}
inline bool Condition::timedwait(size_t ns)
{
    pthread_cond_t *cond = (pthread_cond_t *)&mHandle;
    pthread_mutex_t *mutex = (pthread_mutex_t *)&mMutex.mHandle;
    // 获取当前时间
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    // 计算超时时间
    timeout.tv_nsec += ns;
    return pthread_cond_timedwait(cond, mutex, &timeout) == 0;
}
// 通知
inline void Condition::signal()
{
    pthread_cond_t *cond = (pthread_cond_t *)&mHandle;
    auto res = pthread_cond_signal(cond);
}
inline void Condition::broadcast()
{
    pthread_cond_t *cond = (pthread_cond_t *)&mHandle;
    auto res = pthread_cond_broadcast(cond);
}

inline bool Thread::join(uintptr_t &retval)
{
    auto thd = (pthread_t)&mHandle;
    auto res = pthread_join(thd, (void **)&retval);
    return res == 0;
}
inline bool Thread::detch()
{
    auto thd = (pthread_t)&mHandle;
    auto res = pthread_detach(thd);
    return res == 0;
}
inline bool Thread::setName(cstring name)
{
    auto thd = (pthread_t)&mHandle;
    auto res = pthread_setname_np(thd, name);
    return res == 0;
}
inline bool Thread::getName(char *name, size_t len)
{
    auto thd = (pthread_t)&mHandle;
    auto res = pthread_getname_np(thd, name, len);
    return res == 0;
}

inline bool Thread::cancel()
{
    auto thd = (pthread_t)&mHandle;
    auto res = pthread_cancel(thd);
    return res == 0;
}
inline bool Thread::signal(int sig)
{
    auto thd = (pthread_t)&mHandle;
    auto res = pthread_kill(thd, sig);
    return res == 0;
}

inline bool Thread::setName(Thread::ID thd, cstring name)
{
    auto res = pthread_setname_np(thd, name);
    return res == 0;
}
inline bool Thread::getName(Thread::ID thd, char *name, size_t len)
{
    auto res = pthread_getname_np(thd, name, len);
    return res == 0;
}
inline Thread::ID Thread::getSelf() { return (Thread::ID)pthread_self(); }
inline void Thread::exit(uintptr_t code) { pthread_exit((void *)code); }

inline bool Thread::cancel(Thread::ID thd)
{
    auto res = pthread_cancel(thd);
    return res == 0;
}
inline bool Thread::signal(Thread::ID thd, int sig)
{
    auto res = pthread_kill(thd, sig);
    return res == 0;
}

inline void Thread::run(WorkerFunc task, void *userptr)
{
    auto thdptr = (pthread_t *)&mHandle;

    pthread_attr_t type;
    pthread_attr_init(&type);
    pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);
    auto res = pthread_create(thdptr, &type, task, userptr);
}