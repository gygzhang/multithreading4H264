#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	pid_t result;
	result = fork();
	if(result == -1){
		perror("fork");
		exit;
	}
	else if(result == 0){
		printf("The return value is %d\nIn child process!!\nMy PID is %d\n",result,getpid());
}
	else{
	printf("The return value is %d\nIn father process!!\nMy PID is %d\n",result,getpid());
	}
}

