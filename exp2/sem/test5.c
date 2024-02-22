#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define KEY 100
#define SIZE 10

#include <sys/sem.h>

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
int sem_id1; // 数据信号量 初始值为0需要父进程进程V操作
int sem_id2; // 和信号量 初始值为0，需要子进程进行V操作
int set_semvalue(int sem_id)
{
    union semun sem_union;
    sem_union.val = 0;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return 0;
    return 1;
}
int semaphore_p(int sem_id)
{
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_p failed\n");
        return 0;
    }
    return 1;
}
int semaphore_v(int sem_id)
{
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_v failed\n");
        return 0;
    }
    return 1;
}
void del_semvalue(int sem_id)
{
    // 删除信号量
    union semun sem_union;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

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

    sem_id1 = semget((key_t)1234, 1, 0777 | IPC_CREAT);
    sem_id2 = semget((key_t)4321, 1, 0777 | IPC_CREAT);
    // 信号量初始值设定
    if (!set_semvalue(sem_id1))
    {
        fprintf(stderr, "init failed\n");
        exit(0);
    }
    if (!set_semvalue(sem_id2))
    {
        fprintf(stderr, "init failed\n");
        exit(0);
    }

    if ((pid = fork()) == -1)
    {
        printf("fork error!\n");
    }

    if (pid == 0)
    {
        // 申请信号量1  数据是否ready
        if (semaphore_p(sem_id1))
        {
            printf("P sem1 \n");
        }
        else
        {
            exit(0);
        }
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
        if (msgsnd(mid, &msg_con, 2 * sizeof(int), 0) == -1)
        {
            printf("msgsnd error!\n");
        }
        // 释放信号量2 子进程和ready
        if (semaphore_v(sem_id2))
        {
            printf("V sem2 \n");
        }
        else
        {
            exit(0);
        }
    }
    else
    {
        // input data
        for (i = 0; i < SIZE; i++)
        {
            printf("Input number %d:", i + 1);
            scanf("%d", &num[i]);
            sum1 += num[i];
        }
        printf("\nfather process SUM:sum1 = %d\n", sum1);
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
        // 释放信号量1 数据ready
        if (semaphore_v(sem_id1))
        {
            printf("V sem1 \n");
        }
        else
        {
            exit(0);
        }
        // receive data
        // 申请信号量1
        if (semaphore_p(sem_id2))
        {
            printf("P sem2 \n");
        }
        else
        {
            exit(0);
        }
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
