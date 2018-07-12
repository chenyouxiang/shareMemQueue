#include "../Notifier.h"
#include<unistd.h> 
#include <stdio.h>

int main(){
	Notifier notifier(100);
	sleep(10);
	notifier.notify();
}