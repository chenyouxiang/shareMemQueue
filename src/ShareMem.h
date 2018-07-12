#pragma once

#include <string>
#include "FileLock.h"
#include "Notifier.h"
#include <sys/types.h>

using namespace std;

class ShareMem {
public:
	ShareMem(string path, size_t size);
	~ShareMem();
	int write(const void *data, size_t size);
	int read(void **data);
private:
	struct MemHeader {
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
};