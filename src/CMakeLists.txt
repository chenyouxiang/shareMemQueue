cmake_minimum_required(VERSION 2.6)
project(sharememqueue)

add_definitions("-g -Wall -std=c++11")

add_executable(sharememqueue
main.cpp
FileLock.cpp
FileLockGuard.cpp
Notifier.cpp
ShareMem.cpp
)

add_executable(test1
test/test1.cpp
FileLock.cpp
FileLockGuard.cpp
Notifier.cpp
ShareMem.cpp
)

add_executable(test2
test/test2.cpp
FileLock.cpp
FileLockGuard.cpp
Notifier.cpp
)

add_library(lsharememqueue
FileLock.cpp
FileLockGuard.cpp
)