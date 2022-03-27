#include <iostream>
#include <vector>
#include "JobSystem.h"

#define ARRSIZE 1000
int main() {
    // Demo
    JobSystem::init(std::max((int)std::thread::hardware_concurrency(), 1));
    int* i = new int[ARRSIZE];
    for (int j = 0; j < ARRSIZE; j ++) {
        i[j] = 0;
    }
    //

    for (int x = 0; x < 10000; x++){
        addComplexJob(i);
    }


    std::cout << "All Job Created" << std::endl;
    JobSystem::try_trigger_workers();
    JobSystem::join();
    int sum = 0;
    for (int j = 0 ; j < ARRSIZE; j++){
        sum += i[j];
    }
    std::cout << "Final Val " << sum << std::endl;
    return 0;
}

void addReadJob(const int *i) {
    std::__1::shared_ptr<Job> work = std::__1::make_shared<Job>();
    int index = rand() % (ARRSIZE);
    work -> f = [=]() {
        std::cout << i[index] << std::endl;
    };
    work -> data_read.push_back((long long)(i + index));
    JobSystem::add_job(work);
}

void addWriteJob(int *i) {
    std::__1::shared_ptr<Job> work = std::__1::make_shared<Job>();
    int index = rand() % (ARRSIZE);
    work -> f = [=]() {
        for (int j =0; j < 100000; j++)
            i[index] += 1;
        // std::cout << "Done" << std::endl;

    };
    work -> data_write.push_back((long long)(i + index));
    JobSystem::add_job(work);
}

void addComplexJob(int *i) {
    std::__1::shared_ptr<Job> work = std::__1::make_shared<Job>();
    int index = rand() % (ARRSIZE);
    int index2 = rand() % (ARRSIZE);
    work -> f = [=]() {
        i[index2] += 1;
        std::cout << i[index] << std::endl;
    };
    work -> data_read.push_back((long long)(i + index));
    work -> data_write.push_back((long long)(i + index2));
    JobSystem::add_job(work);
}


