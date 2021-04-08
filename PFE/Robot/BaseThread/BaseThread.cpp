#include "BaseThread.hpp"

BaseThread::BaseThread(std::string name, bool start) {
    m_name = name;
    if(start) {StartThread();}
}

BaseThread::~BaseThread() {
	m_start.store(false, std::memory_order_release);
    #if OS != OS_WINDOWS
        delete inc_x_thread;
    #endif
}

void BaseThread::StartThread() {
	m_start.store(true,std::memory_order_release);
    #if OS == OS_WINDOWS
        inc_x_thread = new std::thread(&BaseThread::ThreadRun, this);
    #else
        inc_x_thread = new pthread_t();
        const int rc = pthread_create(inc_x_thread, NULL, &BaseThread::Helper, this);
        if (rc) {
            std::cout << "Error:unable to create thread " << m_name << ", " << rc << std::endl;
        }
    #endif
}

void BaseThread::StopThread() {
	m_start.store(false,std::memory_order_release);
}

#if OS != OS_WINDOWS
	void* BaseThread::ThreadHelper(void *context) {
	    return static_cast<BaseThread*>(context)->ThreadRun;
	}
#endif

void* BaseThread::ReturnFunction() {
    StopThread();
    #if OS == OS_WINDOWS
        return NULL;
    #else
        pthread_exit(NULL);
    #endif
}

void* BaseThread::ThreadRun() {
    std::cout << "No thread function set !" << std::endl;
    return NULL;
}

bool BaseThread::GetStartValue() {
    return m_start.load(std::memory_order_acquire);
}
