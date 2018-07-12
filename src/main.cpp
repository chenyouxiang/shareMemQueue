#include "FileLock.h"
#include "FileLockGuard.h"
#include <unistd.h> 
#include "ShareMemQueue.h"
#include <string.h>
#include "malloc.h"

int main(){
	pid_t pid = fork();
	string path("/data/cmd.txt");
	if(pid == -1) {
		printf("%s\n", "fork error");
	} else if(pid == 0){
		try{
			ShareMemQueue queue(path, 1024);
			for(int i=0; i<1000000; i++){
				string s = "testasfdasgas";
				
				char buf[100];
				sprintf(buf, "%d", i+1);

				s.append(buf);

				queue.write(s.c_str(), s.length());
				sleep(1);
			}
		}catch(string& s){
			printf("%s\n", s.c_str());
		}
	} else if(pid > 0){
		try{
			ShareMemQueue queue(path, 1024);
			while(true) {
				void **data =reinterpret_cast<void**>(malloc(sizeof(void*)));
				int size = queue.read(data);
				if(size > 0){
					string s;
					s.assign(reinterpret_cast<char*>(*data),size);
					printf("%s\n", s.c_str());
				}
			}
		}catch(string &s){
			printf("%s\n", s.c_str());
		}
	}
}