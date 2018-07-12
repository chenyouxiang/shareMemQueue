#include "FileLockGuard.h"
#include <stdio.h>

FileLockGuard::FileLockGuard(FileLock &lock):lock_(lock){
    if(lock_.lock() <0){
        throw string("get file lock error");
    }
}

FileLockGuard::~FileLockGuard(){
    lock_.unlock();
}
