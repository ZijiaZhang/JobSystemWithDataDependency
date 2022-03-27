//
// Created by Zijia Zhang on 2022-03-27.
//
#include "JobSystem.h"

void JobSystem::init(int workerCount) {
    assert(working_workers.empty() && free_workers.empty());
    workers_mutex.lock();
    for (int i =0; i< workerCount; i++){
        free_workers.push_back(new Worker());
    }
    workers_mutex.unlock();
}

void JobSystem::add_job(const std::shared_ptr<Job> &job) {
    jobs_mutex.lock();
    jobs.push_back(job);
    jobs_mutex.unlock();
//        try_trigger_workers();
}

void JobSystem::mark_dependency(const std::shared_ptr<Job> &job) {
    data_dependency_mutex.lock();
    for (long long data: job->data_read){
        if (!data_reads.contains(data)){
            data_reads.insert(std::pair<long long, int>(data, 0));
        }
        data_reads[data] += 1;
    }
    for (long long data: job->data_write){
        if (!data_writes.contains(data)){
            data_writes.insert(std::pair<long long, int>(data, 0));
        }
        data_writes[data] += 1;
    }
    data_dependency_mutex.unlock();
}

void JobSystem::release_dependency(const std::shared_ptr<Job> &job) {
    data_dependency_mutex.lock();
    for (long long  data: job->data_read){
        if (!data_reads.contains(data)){
            data_reads.insert(std::pair<long long, int>(data, 0));
        }
        data_reads[data] -= 1;
    }
    for (long long  data: job->data_write){
        if (!data_writes.contains(data)){
            data_writes.insert(std::pair<long long, int>(data, 0));
        }
        data_writes[data] -= 1;
    }
    data_dependency_mutex.unlock();
}

bool JobSystem::validate_job(const std::shared_ptr<Job> &job) {
    data_dependency_mutex.lock();
    // Data reads can be executed with data reads, but not with data writes
    for (long long data_key: job-> data_read){
        if(data_writes.contains(data_key) && data_writes.at(data_key) > 0){
            data_dependency_mutex.unlock();
            return false;
        }
    }

    // Data writes can not be executed with either data reads or other data writes
    for (long long data_key: job->data_write){
        if (data_reads.contains(data_key) && data_reads.at(data_key) > 0) {
            data_dependency_mutex.unlock();
            return false;
        }

        if (data_writes.contains(data_key) && data_writes.at(data_key) > 0){
            data_dependency_mutex.unlock();
            return false;
        }
    }
    data_dependency_mutex.unlock();
    return true;
}

void JobSystem::try_trigger_workers() {
    jobs_mutex.lock();
    workers_mutex.lock();
    for (int i =0; i < jobs.size(); i++){
        std::shared_ptr<Job> &job = jobs[i];

        if (free_workers.empty()) break;
        bool valid = validate_job(job);

        // Make worker do the job
        if (valid){
            Worker* worker = free_workers.back();
#ifdef DEBUG
            std::cout << "Sending Job " << job -> id<< " to Worker " << worker->id << std::endl;
#endif
            free_workers.pop_back();
            worker->job = job;
            mark_dependency(job);
            working_workers.insert(std::pair<int, Worker*>(job->id, worker));
#ifdef DEBUG
            std::cout << "Starting Job " << job -> id<< " to Worker " << worker->id << std::endl;
#endif
            worker->start();
            jobs.erase (jobs.begin()+i);
            i--;
        }

    }
    // std::cout << working_workers.size() << "/" <<free_workers.size() << std::endl;
    workers_mutex.unlock();
    jobs_mutex.unlock();
}

void JobSystem::work_done(const std::shared_ptr<Job> &job) {
    workers_mutex.lock();
    int work_id = job->id;
    release_dependency(job);
#ifdef DEBUG
    std::cout<< "JobID" << work_id << std::endl;
#endif
    Worker* worker = working_workers[work_id];
#ifdef DEBUG
    std::cout << "Worker" << worker->id << "Done" <<std::endl;
#endif
    free_workers.push_back(worker);
    working_workers.erase(work_id);
    workers_mutex.unlock();
    try_trigger_workers();
}

void JobSystem::join() {
    while (true){
        jobs_mutex.lock();
        workers_mutex.lock();
        if (jobs.empty() && working_workers.empty()){
            jobs_mutex.unlock();
            return;
        }
        workers_mutex.unlock();

        jobs_mutex.unlock();
    }
}






std::mutex JobSystem::workers_mutex;
std::mutex JobSystem::jobs_mutex;
std::mutex JobSystem::data_dependency_mutex;
std::unordered_map<unsigned int, Worker*> JobSystem::working_workers;
std::vector<Worker*> JobSystem::free_workers;
std::unordered_map<long long, int> JobSystem::data_reads;
std::unordered_map<long long, int> JobSystem::data_writes;
std::vector<std::shared_ptr<Job>> JobSystem::jobs;