
inline Spinlock::Spinlock(int32_t shared)
{
    pthread_spinlock_t *spin = (pthread_spinlock_t *)&mHandle;
    pthread_spin_init(spin, shared == true ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);
}

inline Spinlock::~Spinlock()
{
    pthread_spin_destroy((pthread_spinlock_t *)&mHandle);
    mHandle = 0;
}

inline void Spinlock::lock()
{
    pthread_spin_lock((pthread_spinlock_t *)&mHandle);
}

inline int Spinlock::trylock()
{
    return pthread_spin_trylock((pthread_spinlock_t *)&mHandle);
}

inline void Spinlock::unlock()
{
    pthread_spin_unlock((pthread_spinlock_t *)&mHandle);
}