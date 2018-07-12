#include "../Notifier.h"
#include<unistd.h> 
#include <stdio.h>

int main(){
	Notifier notifier(100);
	while(1){
		notifier.wait();
		printf("%s\n", "wait end");
	}
}