#pragma once

#include <sys/msg.h>

class Notifier {
public:
    Notifier(key_t key);
    void notify();
    void wait();
private:
    int msgid_;
};