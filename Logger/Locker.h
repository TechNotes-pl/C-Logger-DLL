#pragma once
class Locker
{
public:

    static void init(void);
    static bool isInitialized() { return m_is_mutex_initialized; }
    static void lock(void);
    static void unlock(void);

private:
    inline static bool m_is_mutex_initialized;
#if defined(_WIN32)
    inline static CRITICAL_SECTION s_mutex;
#else
    inline static pthread_mutex_t s_mutex;
#endif
};

