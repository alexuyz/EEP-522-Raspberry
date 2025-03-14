#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <list>
#include <exception>
#include <cstdio>
#include <semaphore.h>

// Thread pool class, defined as a template class for code reuse
template<typename T>
class ThreadPool {
public:
    // thread_number is the number of threads in the thread pool
    // max_requests is the maximum number of requests allowed in the request queue waiting for processing
    ThreadPool(int thread_number = 8, int max_requests = 10000);
    ~ThreadPool();
    
    // Add a task to the request queue
    bool append(T* request);

private:
    // Function run by worker threads, continuously taking tasks from the work queue and executing them
    static void* worker(void* arg);
    void run();

private:
    // Number of threads
    int m_thread_number;
    
    // Array describing the thread pool, size is m_thread_number
    pthread_t* m_threads;
    
    // Maximum number of requests allowed in the request queue waiting for processing
    int m_max_requests;
    
    // Request queue
    std::list<T*> m_workqueue;
    
    // Mutex lock protecting the request queue
    pthread_mutex_t m_queuelocker;
    
    // Semaphore to indicate if there are tasks to process
    sem_t m_queuestat;
    
    // Flag to end threads
    bool m_stop;
};

template<typename T>
ThreadPool<T>::ThreadPool(int thread_number, int max_requests) : 
    m_thread_number(thread_number), m_max_requests(max_requests), 
    m_stop(false), m_threads(NULL) {
    
    if((thread_number <= 0) || (max_requests <= 0)) {
        throw std::exception();
    }
    
    m_threads = new pthread_t[m_thread_number];
    if(!m_threads) {
        throw std::exception();
    }
    
    // Create thread_number threads and set them as detached threads
    for(int i = 0; i < thread_number; ++i) {
        printf("create the %dth thread\n", i);
        
        if(pthread_create(m_threads + i, NULL, worker, this) != 0) {
            delete [] m_threads;
            throw std::exception();
        }
        
        if(pthread_detach(m_threads[i])) {
            delete [] m_threads;
            throw std::exception();
        }
    }
    
    // Initialize mutex and semaphore
    if(pthread_mutex_init(&m_queuelocker, NULL) != 0) {
        delete [] m_threads;
        throw std::exception();
    }
    
    if(sem_init(&m_queuestat, 0, 0) != 0) {
        delete [] m_threads;
        pthread_mutex_destroy(&m_queuelocker);
        throw std::exception();
    }
}

template<typename T>
ThreadPool<T>::~ThreadPool() {
    m_stop = true;
    delete [] m_threads;
    pthread_mutex_destroy(&m_queuelocker);
    sem_destroy(&m_queuestat);
}

template<typename T>
bool ThreadPool<T>::append(T* request) {
    // Always lock when operating on the work queue, as it's shared by all threads
    pthread_mutex_lock(&m_queuelocker);
    if(m_workqueue.size() > m_max_requests) {
        pthread_mutex_unlock(&m_queuelocker);
        return false;
    }
    
    m_workqueue.push_back(request);
    pthread_mutex_unlock(&m_queuelocker);
    
    // Increase semaphore, notify worker threads that a new task has arrived
    sem_post(&m_queuestat);
    return true;
}

template<typename T>
void* ThreadPool<T>::worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    pool->run();
    return pool;
}

template<typename T>
void ThreadPool<T>::run() {
    while(!m_stop) {
        // Wait for semaphore, wake up when there's a task
        sem_wait(&m_queuestat);
        
        // Acquire mutex lock
        pthread_mutex_lock(&m_queuelocker);
        if(m_workqueue.empty()) {
            pthread_mutex_unlock(&m_queuelocker);
            continue;
        }
        
        // Take the first task from the queue
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        pthread_mutex_unlock(&m_queuelocker);
        
        if(!request) {
            continue;
        }
        
        // Execute the task
        request->process();
    }
}

#endif
