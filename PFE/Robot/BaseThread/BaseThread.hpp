#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <iostream>
#include <string>
#include <atomic>
#include <array>
#include <condition_variable>
#include <thread>
#include <mutex>

#if OS == OS_WINDOWS
#include <thread>
#else
#include <pthread.h>
#endif

/*

                     BaseThread class

    SOUDIER Jean  (jean.soudier@insa-strasbourg.fr)


    Provides a class to manage threads

    Features :

    • Create, start, stop and destroy threads
    • Provide global variable for the communication between threads

    Functions :

    • BaseThread                     | Constructor with argument the name of the thread
    • StartThread()                  | Function to start the thread
    • StopThread()                   | Function to stop the thread
    • ReturnFunction()               | Function to add at the end for properlly closing the thread
    • GetStartValue()                | Function to test if the thread stop order is given
    • ThreadRun()                    | Function to overwrite with our code ThreadRun() {
                                                                                while(GetStartValue()) {code_to_do();}
                                                                                ReturnFunction();
                                                                            }

*/

#define LENGTH_PID_CHAIN 2 //number of controllers

extern std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_setpoint;
extern std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_feedback;
extern std::array<std::atomic<bool>, LENGTH_PID_CHAIN> g_event;
extern std::array<std::mutex, LENGTH_PID_CHAIN> g_cv_mutex;
extern std::array<std::condition_variable, LENGTH_PID_CHAIN> g_cv;

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
