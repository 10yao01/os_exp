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
        int pid2 = fork();
        if (pid2 == -1)
        {
            printf("Fork Failed");
            exit(0);
        }
        else if (pid2 == 0)
        {
            for (int i = 0; i < 5; i++)
            {
                count += 2;
                printf("子进程2 %d count = %d \n", getpid(), count);
            }
        }
        else
        {
            wait(NULL);
            for (int i = 0; i < 5; i++)
            {
                count += 3;
                printf("子进程1 %d count = %d \n", getpid(), count);
            }
        }
    }
    else
    {
        wait(NULL);
        count = count + 5;
        printf("父进程 %d 学号:20213210101112 姓名:姚攀峰 count = %d \n", getpid(), count);
    }
    return 0;
}
