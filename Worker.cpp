//
// Created by Zijia Zhang on 2022-03-27.
//

#include "Worker.h"
#include "JobSystem.h"

void Worker::start() {
    assert(!working);
    assert(job);
    working = true;
    // Create a thread executing the job
    t = std::thread([=]() mutable {
        worker_mutex.lock();
        job->f();
        working = false;
        std::shared_ptr<Job> temp;
        temp = job;
        job = nullptr;
        // Trigger the work done callback
        JobSystem::work_done(temp);
        worker_mutex.unlock();
    });
    t.detach();
}