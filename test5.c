#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#define KEY 100
#define SIZE 10
struct msgform
{
    long mtype;
    int mnum[256];
} msg_pro, msg_con;
int main()
{
    int num[SIZE];
    int sum1 = 0;
    int i;
    int pid;
    int *p;
    // input data
    for (i = 0; i < SIZE; i++)
    {
        printf("Input number %d:", i + 1);
        scanf("%d", &num[i]);
        sum1 += num[i];
    }
    printf("father process SUM:sum1 = %d\n", sum1);
    if ((pid = fork()) == -1)
    {
        printf("fork error!\n");
    }
    if (pid == 0)
    {
        // child process
        int sum_power2 = 0;
        int mid = msgget(KEY, 0777);
        if (mid == -1)
        {
            printf("msgget error");
        }
        if (msgrcv(mid, &msg_con, 256, 1, 0) == -1)
        {
            printf("msgrcv error");
        }
        int *begin = msg_con.mnum;
        for (i = 0; i < SIZE; i++)
        {
            int temp = (int)begin[i];
            // printf("%d",temp);
            sum_power2 += temp * temp;
        }
        printf("child process SQUARE_SUM:sum2 = %d\n", sum_power2);
        // send data
        msg_con.mtype = 2;
        msg_con.mnum[0] = sum_power2;
        if (msgsnd(mid, &msg_con, 1 * sizeof(int), 0) == -1)
        {
            printf("msgsnd error!\n");
        }
    }
    else
    {
        // parent process
        // use message queue to deliver result
        int msgid = msgget(KEY, 0777 | IPC_CREAT);
        if (msgid == -1)
        {
            printf("msgget error!");
        }
        // father process send message to message queue
        msg_pro.mtype = 1;
        for (i = 0; i < SIZE; i++)
        {
            msg_pro.mnum[i] = num[i];
            // printf("%d",num[i]);
        }
        if (msgsnd(msgid, &msg_pro, 10 * sizeof(int), 0))
        {
            printf("msgsnd error!\n");
        }
        // wait child process
        wait(NULL);
        // receive data
        if (msgrcv(msgid, &msg_pro, 256, 2, 0) == -1)
        {
            printf("msgrcv error");
        }
        printf("receive square sum:%d\n", msg_pro.mnum[0]);
        printf("sum1 + sum2 = %d + %d = %d", sum1, msg_pro.mnum[0], sum1 + msg_pro.mnum[0]);
        // release resources
        if (msgctl(msgid, IPC_RMID, 0))
        {
            printf("msgctl error");
        }
    }
    return 0;
}
