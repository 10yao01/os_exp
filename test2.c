#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main()
{
    int count = 0;
    int pid1 = fork();
    if (pid1 == -1)
    {
        printf("Fork Failed");
        exit(-1);
    }
    else if (pid1 == 0)
    {
        printf("子进程前 \n");
        execlp("./child1", "./child1", NULL);
        printf("子进程后");
    }
    else
    {
        wait(NULL);
        count = count + 5;
        printf("父进程 %d 学号:20213210101112 姓名:姚攀峰 count = %d \n", getpid(), count);
    }
    return 0;
}
