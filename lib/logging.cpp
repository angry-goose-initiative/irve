/**
 * @file    logging.cpp
 * @brief   Logging facilities for irve
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
 * Based on code from rv32esim
*/

/* ------------------------------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------------------------- */

#include "config.h"

#define INST_COUNT this does not actually need to be defined with anything important before including logging.h in this case
#include "logging.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#if IRVE_INTERNAL_CONFIG_ASYNC_LOGGING
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <tuple>
#endif

using namespace irve::internal;

/* ------------------------------------------------------------------------------------------------
 * Static Function Declarations
 * --------------------------------------------------------------------------------------------- */

static void actual_log_function(FILE* destination, uint64_t inst_num, uint8_t indent, const char* str);

/* ------------------------------------------------------------------------------------------------
 * Function Implementations
 * --------------------------------------------------------------------------------------------- */

void logging::irvelog_internal_variadic_function_dont_use_this_directly(FILE* destination, uint64_t inst_num, uint8_t indent, const char* str, ...) {
    //TODO in async logging try to do this on the logging thread
    va_list list_copy_1;
    va_start(list_copy_1, str);
    va_list list_copy_2;
    va_copy(list_copy_2, list_copy_1);

    //Perform variable argument string formatting into a buffer
    std::size_t buffer_size = std::vsnprintf(nullptr, 0, str, list_copy_1) + 1;//vsnprintf doesn't count the null terminator
    va_end(list_copy_1);
    char* buffer = (char*)std::malloc(buffer_size);
    std::vsnprintf(buffer, buffer_size, str, list_copy_2);
    va_end(list_copy_2);

    //Actually log the string
    irvelog_internal_function_dont_use_this_directly(destination, inst_num, indent, buffer);

    //Free the buffer
    std::free(buffer);
}

void logging::irvelog_internal_function_dont_use_this_directly(FILE* destination, uint64_t inst_num, uint8_t indent, const char* str) {
    assert(destination && "Attempt to log to null destination file");
    assert(str && "Attempt to log with null string");

#if IRVE_INTERNAL_CONFIG_ASYNC_LOGGING
    class AsyncLogger {
        private:
            std::queue<std::tuple<FILE*, uint64_t, uint8_t, std::string>> m_queue;
            std::atomic<bool> m_queue_busy;
            std::atomic<bool> m_thread_running;
            std::thread m_thread;
        public:
            AsyncLogger() : m_queue_busy(false), m_thread_running(true), m_thread([&]() {
                std::setbuf(stdout, NULL);//Disable stdout buffering since we're using multiple threads anyways//TODO should we really be doing this?
                //Main logging loop
                while (true) {
                    //Continually try to reserve the queue
                    while (m_queue_busy.exchange(true)) {
                        //Okay to yield here since in testing, the logging thread can't keep up with the main thread
                        //anyways (so might as well be more efficient)
                        std::this_thread::yield();
                    }
                    //At this point, the queue is reserved

                    //If the queue is empty, release it and check if the thread should exit
                    if (m_queue.empty()) {
                        assert(m_queue_busy.load() && "Queue was not reserved when it should have been");
                        m_queue_busy.store(false);

                        if (!m_thread_running.load()) {//The queue is empty and will never be filled again
                            return;//So exit the thread
                        }
                    } else {//Otherwise, if the queue is not empty
                        //Pop the front element and release the queue
                        auto [destination, inst_num, indent, str] = m_queue.front();
                        m_queue.pop();
                        assert(m_queue_busy.load() && "Queue was not reserved when it should have been");
                        m_queue_busy.store(false);

                        //Log the popped element
                        actual_log_function(destination, inst_num, indent, str.c_str());
                    }
                }
            })
            {}

            ~AsyncLogger() {
                m_thread_running.store(false);
                m_thread.join();
            }

            void enqueue_log_request(FILE* destination, uint64_t inst_num, uint8_t indent, std::string str) {
                while (m_queue_busy.exchange(true));//Busy wait until the queue is free (NOT yeilding since this needs to be very fast)
                m_queue.emplace(destination, inst_num, indent, str);
                assert(m_queue_busy.load() && "Queue was not reserved when it should have been");
                m_queue_busy.store(false);//Release the queue
            }
    };

    static AsyncLogger async_logger;
    async_logger.enqueue_log_request(destination, inst_num, indent, str);
#else//Non-async logging
    actual_log_function(destination, inst_num, indent, str);
#endif
}

/* ------------------------------------------------------------------------------------------------
 * Static Function Implementations
 * --------------------------------------------------------------------------------------------- */

static void actual_log_function(FILE* destination, uint64_t inst_num, uint8_t indent, const char* str) {
    assert(destination && "Attempt to log to null destination file");
    assert(str && "Attempt to log with null string");

    if (inst_num) {
        std::fprintf(destination, "\x1b[94m%llu\x1b[1;90m>\x1b[0m ", (long long unsigned int) inst_num);
    } else {
        std::fprintf(destination, "\x1b[94mIRVE\x1b[1;90m>\x1b[0m ");
    }

    while (indent--) {
        std::fprintf(destination, "  ");
    }

    std::fprintf(destination, "%s\n", str);
}
