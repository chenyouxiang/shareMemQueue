#include "FileLock.h"

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h> 
#include<unistd.h> 

FileLock::FileLock(string path):fd_(open(path.c_str(),O_RDWR|O_CREAT)) {
	init_lock();
}

FileLock::~FileLock(){
	close(fd_);
}

void FileLock::init_lock(){
	lock_.l_start = 0;
	lock_.l_whence = SEEK_SET;
	lock_.l_len = 1;
}

int FileLock::lock(){
	lock_.l_type = F_WRLCK;
	return fcntl(fd_, F_SETLKW,&lock_);
}

int FileLock::unlock(){
	lock_.l_type = F_UNLCK;
	return fcntl(fd_, F_SETLKW,&lock_);
}