#pragma once
#include "FileLock.h"


class FileLockGuard {
public:
	FileLockGuard(FileLock &lock);
	~FileLockGuard();
private:
	FileLock &lock_;
};