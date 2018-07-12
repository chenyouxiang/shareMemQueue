cmake_minimum_required(VERSION 2.6)
project(sharememqueue)

add_definitions("-g -Wall -std=c++11")

add_executable(sharememqueue
src/main.cpp
src/FileLock.cpp
src/FileLockGuard.cpp
src/Notifier.cpp
src/ShareMem.cpp
)

add_executable(test1
test/test1.cpp
src/FileLock.cpp
src/FileLockGuard.cpp
src/Notifier.cpp
src/ShareMem.cpp
)

add_executable(test2
test/test2.cpp
src/FileLock.cpp
src/FileLockGuard.cpp
src/Notifier.cpp
src/ShareMem.cpp
)

add_library(lsharememqueue
src/FileLock.cpp
src/FileLockGuard.cpp
src/Notifier.cpp
src/ShareMem.cpp
)