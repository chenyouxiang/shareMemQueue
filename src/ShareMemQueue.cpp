
#include <sys/ipc.h>
#include <sys/shm.h>
#include <memory.h>
#include <malloc.h>
#include "ShareMemQueue.h"
#include "FileLockGuard.h"


ShareMemQueue::ShareMemQueue(string path, size_t size):lock_(path),key_(ftok(path.c_str(),1)), notifier_(key_) {
    shmId_ = shmget(key_, sizeof(MemHeader) + size, 0666|IPC_CREAT);
    if(shmId_ == -1){
        if(errno == ENOENT) {
            throw string("no such shm");
        }
        char err[100];
        sprintf(err, "shmget error:%d", errno);
        throw string(err);
    }
    char* addr =reinterpret_cast<char*>(shmat(shmId_,NULL,0));
    if(addr ==  reinterpret_cast<void*>(-1)) {
        throw string("shmat error");
    }
    memHeader_ = reinterpret_cast<MemHeader*>(addr);
    memset(memHeader_, 0, sizeof(MemHeader) + size);
    memHeader_->size = size;
    memHeader_->addr = addr + MEM_HEADER_SIZE;
}


ShareMemQueue::~ShareMemQueue(){
    auto ret = shmdt(memHeader_);
    if(ret == -1) {
        char err[100];
        sprintf(err, "shmdt error: %d",errno);
        throw string(err);
    }
}

int ShareMemQueue::write(const void *data, size_t size){
    int ret = -1;
    {
        FileLockGuard gurad(lock_);
        ret = doWrite(data, size);
        if(ret == 0){
            ret = doWrite(data, size);
        }
    }
    notifier_.notify();
    return ret;
}

int ShareMemQueue::read(void **data){
    int ret = -1;
    {
        FileLockGuard gurad(lock_);
        ret = doRead(data);
        if(ret == 0){
            ret = doRead(data);
        }
    }
    if(ret ==0 || ret == -1){
        notifier_.wait();
    }
    return ret;
}

int ShareMemQueue::doRead(void **data){
    char* mem = memHeader_->addr + memHeader_->readIndex;

    // printf("doRead->readIndex:%lu\n", memHeader_->readIndex);
    // printf("doRead->writeIndex:%lu\n", memHeader_->writeIndex);
    // printf("doRead->memHeader_->size:%lu\n", memHeader_->size);

    if(memHeader_->readIndex == memHeader_->writeIndex){
        return 0;
    }else if(memHeader_->readIndex > memHeader_->writeIndex){
        auto leftMemSize =memHeader_->size - memHeader_->readIndex;
        if(leftMemSize <= BLOCK_HEADER_SIZE){
            memHeader_->readIndex = 0;
            return 0;
        }
        auto blockHeader = reinterpret_cast<BlockHeader*>(mem);
        if(blockHeader->hasData == 0){
            memHeader_->readIndex = 0;
            return 0;
        }
        *data = malloc(blockHeader->size);
        memcpy(*data, mem + BLOCK_HEADER_SIZE, blockHeader->size);
        memHeader_->readIndex += BLOCK_HEADER_SIZE + blockHeader->size;
        return blockHeader->size;
    } else {
        auto leftMemSize =memHeader_->writeIndex - memHeader_->readIndex;
        if(leftMemSize <= BLOCK_HEADER_SIZE){
            return -1;
        }
        auto blockHeader = reinterpret_cast<BlockHeader*>(mem);
        if(blockHeader->size + BLOCK_HEADER_SIZE > leftMemSize) {
            return -1;
        }
        *data = malloc(blockHeader->size);
        memcpy(*data, mem + BLOCK_HEADER_SIZE, blockHeader->size);
        memHeader_->readIndex += BLOCK_HEADER_SIZE + blockHeader->size;
        return blockHeader->size;
    }

}



/**
 * 执行写操作
 * @param  data [description]
 * @param  size [description]
 * @return      [如果返回0，可以再次调用一次，返回-1代表失败]
 */
int ShareMemQueue::doWrite(const void *data, size_t size) {
    // printf("doWrite->readIndex:%lu\n", memHeader_->readIndex);
    // printf("doWrite->writeIndex:%lu\n", memHeader_->writeIndex);
    char* mem = memHeader_->addr + memHeader_->writeIndex;
    if(memHeader_->writeIndex >= memHeader_->readIndex) {
        size_t leftMemSize =memHeader_->size -  memHeader_->writeIndex;

        // printf("doWrite->leftMemSize:%lu\n", leftMemSize);
        // printf("doWrite->memHeader_->size:%lu\n", memHeader_->size);
        
        //剩下足够的空间存放数据
        if(leftMemSize >= size + BLOCK_HEADER_SIZE) {
            auto blockHeader = BlockHeader{size, 1};
            memcpy(mem, &blockHeader, BLOCK_HEADER_SIZE);
            mem = mem + BLOCK_HEADER_SIZE;
            memHeader_->writeIndex += BLOCK_HEADER_SIZE + size; 
            memcpy(mem, data, size);
            return size;
        } 
        //剩下的空间可以放下一个BlockHeader
        else if(leftMemSize >= BLOCK_HEADER_SIZE){
            auto blockHeader = BlockHeader{leftMemSize - BLOCK_HEADER_SIZE, 0};
            memcpy(mem, &blockHeader, BLOCK_HEADER_SIZE);
            //没任何空间了
            if(memHeader_->readIndex == 0){
                return -1;
            }
            //再试一次
            memHeader_->writeIndex = 0; 
            return 0;
        } 
        //连一个BlockHeader也放不下了
        else {
            //没任何空间了
            if(memHeader_->readIndex == 0){
                return -1;
            }
            //再试一次
            memHeader_->writeIndex = 0; 
            return 0;
        }
    } 
    //readerIndex > writeIndex的时候
    else {
        size_t leftMemSize = memHeader_->readIndex -  memHeader_->writeIndex;
        //有足够空间的时候，一定要大于，不然readerIndex == writeIndex，会认为没有任何数据了
        if(leftMemSize > size + BLOCK_HEADER_SIZE) {
            auto blockHeader = BlockHeader{size, 1};
            memcpy(mem, &blockHeader, BLOCK_HEADER_SIZE);
            mem = mem + BLOCK_HEADER_SIZE;
            memHeader_->writeIndex += BLOCK_HEADER_SIZE + size; 
            memcpy(mem, data, size);
            return size;
        } 
        //空间不够了
        else {
            return -1;
        }
    }
    
}