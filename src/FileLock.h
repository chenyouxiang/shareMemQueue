#pragma once

#include <string>
#include <fcntl.h>

using namespace std;

class FileLock {
public:
    FileLock(string path);
    ~FileLock();
    int lock();
    int unlock();
private:
    void init_lock();
    int fd_;    
    struct flock lock_;
};