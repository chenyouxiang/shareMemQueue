#pragma once

#include <string>
#include "FileLock.h"
#include "Notifier.h"
#include <sys/types.h>

using namespace std;

class ShareMemQueue {
public:
    ShareMemQueue(string path, size_t size);
    ~ShareMemQueue();
    int write(const void *data, size_t size);
    int read(void **data);
private:
    struct MemHeader {
        int magic;
        size_t readIndex;
        size_t writeIndex;
        size_t size; 
        char *addr;
    };
    struct BlockHeader {
        size_t size;
        int    hasData;
    };
    MemHeader* memHeader_;  
    int shmId_;
    FileLock lock_;
    key_t key_;
    Notifier notifier_;
    int doWrite(const void *data, size_t size);
    int doRead(void **data);
    const size_t BLOCK_HEADER_SIZE = sizeof(BlockHeader);
    const size_t MEM_HEADER_SIZE = sizeof(MemHeader);
    const int MAGIC = 123456;
};