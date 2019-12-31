#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *envp[]={"PATH=/tmp","USER=sunq",NULL};
        if(fork()==0){
                if(execle("/bin/env","env",NULL,envp)<0)
                        perror("execle error!");
        }
}

