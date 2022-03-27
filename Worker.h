//
// Created by Zijia Zhang on 2022-03-27.
//

#ifndef WORKER_H
#define WORKER_H
#include <vector>
#include <thread>
#include "debug.h"
#include "Job.h"


class Worker{
public:
    // This is to prevent new thread created and executed with previous one not exited,
    // causing excess amount of threads. Maybe there is a better way
    std::mutex worker_mutex{};
    // Debug Use
    unsigned int id;
    bool working{false};
    // The current job
    std::shared_ptr<Job> job;

    void start();
    Worker(){
        id = next_id();
    }
private:
    std::thread t;
    static unsigned int next_id() {
        static unsigned int id_count = 1;
        return id_count++;
    }

};


#endif //WORKER_H
