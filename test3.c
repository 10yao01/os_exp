#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main()
{
    int i;
    pid_t pid, ppid;
    printf("begin\n");
    pid = fork();
    if (pid == -1)
    {
        printf("fork error");
        exit(-1);
    }
    if (pid == 0) // child process
    {
        for (i = 0; i < 10; i++)
        {
            ppid = getppid();
            printf("当前子进程的父进程ID%d\n", ppid);
            printf("子进程的进程ID%d\n", getpid());
            sleep(2);
        }
    }
    else
    {
        sleep(3);
        printf("当前进程(父进程)ID%d\n", getpid());
        printf("The parent process will stop running\n");
    }
    printf("end");
    return 0;
}
