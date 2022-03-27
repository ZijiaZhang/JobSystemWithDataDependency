cmake_minimum_required(VERSION 3.20)
project(JobSystem)

set(CMAKE_CXX_STANDARD 23)

add_executable(JobSystem main.cpp JobSystem.h JobSystem.cpp debug.h Worker.cpp Worker.h Job.cpp Job.h)
