//
// Created by zenikolas on 08.06.2020.
//

#ifndef RWMUTEX_RWMUTEX_H
#define RWMUTEX_RWMUTEX_H

#include <mutex>
#include <condition_variable>

class RWMutex {
    std::mutex m_mut;
    std::condition_variable m_readWait;
    std::condition_variable m_writeWait;
    int64_t m_waiting_writers = {};
    int64_t m_active_readers = {};
    bool m_write_active = false;
public:
    void lock();
    void unlock();

    void lock_shared();
    void unlock_shared();
};

inline
void RWMutex::lock() {
    std::unique_lock lock(m_mut);
    ++m_waiting_writers;
    if (m_active_readers > 0 || m_write_active) {
        m_writeWait.wait(lock, [this]{  return m_active_readers == 0 && !m_write_active; });
    }
    --m_waiting_writers;
    m_write_active = true;
}

inline
void RWMutex::unlock() {
    std::lock_guard lock(m_mut);
    m_write_active = false;
    if (m_waiting_writers > 0) {
        m_writeWait.notify_one();
    } else {
        m_readWait.notify_all();
    }
}

inline
void RWMutex::lock_shared() {
    std::unique_lock lock(m_mut);
    if (m_waiting_writers > 0 || m_write_active) {
        m_readWait.wait(lock, [this]{ return m_waiting_writers == 0 && !m_write_active; });
    }
    ++m_active_readers;
}

inline
void RWMutex::unlock_shared() {
    std::lock_guard lock(m_mut);
    --m_active_readers;
    if (m_waiting_writers > 0 && m_active_readers == 0) {
        m_writeWait.notify_one();
    }
}

#endif //RWMUTEX_RWMUTEX_H
