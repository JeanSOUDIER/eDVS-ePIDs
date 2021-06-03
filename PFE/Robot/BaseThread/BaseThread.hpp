#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <iostream>
#include <string>
#include <atomic>
#include <array>
#include <condition_variable>
#include <thread>
#include <mutex>

#define LENGTH_PID_CHAIN 2

extern std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_setpoint;
extern std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_feedback;
extern std::array<std::atomic<bool>, LENGTH_PID_CHAIN> g_event;
extern std::array<std::mutex, LENGTH_PID_CHAIN> g_cv_mutex;
extern std::array<std::condition_variable, LENGTH_PID_CHAIN> g_cv;

#if OS == OS_WINDOWS
#include <thread>
#else
#include <pthread.h>
#endif

class BaseThread {
    public:
        BaseThread(std::string name, bool start = false);
        ~BaseThread();

        #if OS != OS_WINDOWS
        	static void* ThreadHelper(void *context);
        #endif

        void StartThread();
        void StopThread();
        void* ReturnFunction();
        bool GetStartValue();

    protected:
        virtual void* ThreadRun();

    private:
    	std::string m_name;
        std::atomic<bool> m_start;

    	#if OS == OS_WINDOWS
            std::thread *inc_x_thread;
        #else
            pthread_t *inc_x_thread;
        #endif
};

#endif //BASE_THREAD_H
