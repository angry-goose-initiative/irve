/**
 * @brief   A dead simple thread safe queue based purely on things in the C++ standard library
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO make this lock-free?
 * FIXME this isn't really usable if we have multiple readers since we don't provide a way to check if the queue is empty
 *     at the same time as popping
 *
*/

#pragma once

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include <cassert>
#include <cstddef>
#include <deque>
#include <mutex>
#include <queue>
#include <thread>

/* ------------------------------------------------------------------------------------------------
 * Type/Class Declarations
 * --------------------------------------------------------------------------------------------- */

namespace irve::internal::tsqueue {

template<typename T, typename C = std::deque<T>>//Same as std::queue
class tsqueue_t {
public:
    tsqueue_t() {}

    //Note: These return a copy of the value, not a reference since that simplifies thread safety
    T front() const;
    T back() const;

    bool empty() const;
    std::size_t size() const;

    void push(const T& value);
    template<typename... A> void emplace(A&&... constructor_arguments);
    void pop();

    //TODO add other std::queue functions (ex. swap, operator overloads, etc.)

private:
    std::queue<T, C> m_queue;
    mutable std::mutex m_mutex;//Locking/unlocking is "logically" const since it doesn't change the queue

    //Locking/unlocking is "logically" const since it doesn't change the queue
    void lock() const;
    void unlock() const;
};

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */
//NOTE: Must be in header file because this is templated

template<typename T, typename C>
T tsqueue_t<T, C>::front() const {
    this->lock();
    T value = this->m_queue.front();
    this->unlock();
    return value;
}

template<typename T, typename C>
T tsqueue_t<T, C>::back() const {
    this->lock();
    T value = this->m_queue.back();
    this->unlock();
    return value;
}

template<typename T, typename C>
bool tsqueue_t<T, C>::empty() const {
    this->lock();
    bool value = this->m_queue.empty();
    this->unlock();
    return value;
}

template<typename T, typename C>
std::size_t tsqueue_t<T, C>::size() const {
    this->lock();
    bool value = this->m_queue.size();
    this->unlock();
    return value;
}

template<typename T, typename C>
void tsqueue_t<T, C>::push(const T& value) {
    this->lock();
    this->m_queue.push(value);
    this->unlock();
}

template<typename T, typename C> template<typename... A>
void tsqueue_t<T, C>::emplace(A&&... constructor_arguments) {
    this->lock();
    this->m_queue.emplace(std::forward<A>(constructor_arguments)...);
    this->unlock();
}

template<typename T, typename C>
void tsqueue_t<T, C>::pop() {
    this->lock();
    this->m_queue.pop();
    this->unlock();
}

template<typename T, typename C>
void tsqueue_t<T, C>::lock() const {
    this->m_mutex.lock();
}

template<typename T, typename C>
void tsqueue_t<T, C>::unlock() const {
    this->m_mutex.unlock();
}

}
