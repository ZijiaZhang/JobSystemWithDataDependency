//
// Created by Zijia Zhang on 2022-03-27.
//

#ifndef JOBSYSTEM_H
#define JOBSYSTEM_H


#include <unordered_map>
#include "Worker.h"



class JobSystem {
public:
    // Mutexes
    static std::mutex workers_mutex;
    static std::mutex jobs_mutex;
    static std::mutex data_dependency_mutex;

    // Init a number of workers should only be called once
    static void init(int workerCount);

    // Add job to the system
    static void add_job(const std::shared_ptr<Job>& job);

    // Claim the dependency
    static void mark_dependency(const std::shared_ptr<Job>& job);

    // Release the dependency
    static void release_dependency(const std::shared_ptr<Job>& job);

    // See if the job can be run
    static bool validate_job(const std::shared_ptr<Job>& job);

    // Try to execute a job from the queue
    static void try_trigger_workers();

    // Callback when a job is done
    static void work_done(const std::shared_ptr<Job>& job);

    // Wait for all work to finish
    static void join();

private:
    // Job id to worker
    static std::unordered_map<unsigned int, Worker*> working_workers;
    static std::vector<Worker*> free_workers;
    // DATA ID AND COUNTS
    static std::unordered_map<long long, int> data_reads;
    static std::unordered_map<long long, int> data_writes;

    // Job Queue
    static std::vector<std::shared_ptr<Job>> jobs;
};

void addWriteJob(int *i);

void addReadJob(const int *i);

void addComplexJob(int *i);


#endif //JOBSYSTEM_H
