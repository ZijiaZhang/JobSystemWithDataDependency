//
// Created by Zijia Zhang on 2022-03-27.
//

#ifndef JOB_H
#define JOB_H


#include <functional>
#include <vector>

class Job {
public:
    unsigned int id;
    std::function<void()> f;

    // Data it reads
    std::vector<long long> data_read{};

    // Data it writes
    std::vector<long long> data_write{};

    Job(){
        id = next_id();
    }
private:
    static unsigned int next_id() {
        static unsigned int id_count = 1;
        return id_count++;
    }
};


#endif //JOB_H
