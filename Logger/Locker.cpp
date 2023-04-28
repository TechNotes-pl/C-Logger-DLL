#include "pch.h"
#include "Locker.h"

void Locker::init(void)
{
    if (!isInitialized()) {
#if defined(_WIN32)
        InitializeCriticalSection(&s_mutex);
#else
        pthread_mutex_init(&s_mutex, NULL);
#endif
        m_is_mutex_initialized = true;
    }
}

void Locker::lock(void)
{
#if defined(_WIN32)
    EnterCriticalSection(&s_mutex);
#else
    pthread_mutex_lock(&s_mutex);
#endif
}

void Locker::unlock(void)
{
#if defined(_WIN32)
    LeaveCriticalSection(&s_mutex);
#else
    pthread_mutex_unlock(&s_mutex);
#endif
}