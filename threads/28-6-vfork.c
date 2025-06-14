#include "stdlib.h"
#include <unistd.h>
#include <sys/wait.h>

int main(){
	for(int i=0; i<100000;i++){
		pid_t pid = vfork();
		if (pid == 0){
			exit(0);
		} else {
			waitpid(pid, NULL, 0);
		}
	}
	return 0;
}
