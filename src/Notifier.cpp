#include "Notifier.h"
#include <string>
#include <memory.h>

using namespace std;

const int MSGLEN = 512;

struct msg_st
{
    long int msg_type;
    char text[MSGLEN];
};

Notifier::Notifier(key_t key){
	msgid_ = msgget(key, 0666 | IPC_CREAT);
	if(msgid_ == -1){
	 	throw string("msgget error");
	}
}

void Notifier::notify(){
	struct msg_st data;
    memset(&data, sizeof(data),0);
	data.msg_type = 1;
    if (msgsnd(msgid_, (void *)&data, MSGLEN, IPC_NOWAIT) == -1){
        if(errno == EAGAIN){
            
        }else {
        	char err[100];
            sprintf(err, "msgsnd failed, msgid_:%d\n", msgid_);
            throw string(err);
        }
    }
}

void Notifier::wait(){
	struct msg_st data;
    memset(&data, sizeof(data),0);
	data.msg_type = 1;
	long int msgtype = 0; 
	if (msgrcv(msgid_, (void *)&data, MSGLEN, msgtype, 0) == -1)
    {	
    	char err[100];
        sprintf(err, "msgrcv failed width erro: %d", errno);
        throw  string(err);
    }
}