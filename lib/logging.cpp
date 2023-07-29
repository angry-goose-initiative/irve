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

#include "tsqueue.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#if IRVE_INTERNAL_CONFIG_ASYNC_LOGGING
#include <string>
#include <thread>
#include <atomic>
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
            tsqueue::tsqueue_t<std::tuple<FILE*, uint64_t, uint8_t, std::string>> m_queue;
            std::atomic<bool> m_thread_should_keep_running;
            std::thread m_thread;
        public:
            AsyncLogger() : m_thread_should_keep_running(true), m_thread([&]() {
                //Main logging loop
                while (true) {
                    if (this->m_queue.empty()) {
                        if (this->m_thread_should_keep_running.load()) {
                            //Yield so as to not absolutely burn CPU time
                            std::this_thread::yield();
                        } else {//The queue is empty and will never be filled again (our backlog is empty forever)
                            return;//So exit the thread
                        }
                    } else {//The queue is not empty
                        //Pop the front element from the queue
                        auto [destination, inst_num, indent, str] = m_queue.front();
                        m_queue.pop();

                        //Log the popped element
                        actual_log_function(destination, inst_num, indent, str.c_str());
                    }
                }
            })
            {}

            ~AsyncLogger() {
                m_thread_should_keep_running.store(false);
                m_thread.join();//Wait for the thread to finish its backlog
            }

            void enqueue_log_request(FILE* destination, uint64_t inst_num, uint8_t indent, std::string str) {
                m_queue.emplace(destination, inst_num, indent, str);
            }
    };

    static AsyncLogger async_logger;//We make this static so the thread lasts for the entirety of IRVE's execution
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
