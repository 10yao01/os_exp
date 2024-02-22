#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#define KEY 75
#define K 1024
int main()
{
    int mid, *q, *t, i;
    char *maddr;
    mid = shmget(KEY, K, 0777);
    if (mid == -1)
    {
        printf("shmget error");
    }
    maddr = shmat(mid, 0, 0);
    if (maddr == (void *)-1)
    {
        printf("shmat error");
    }
    q = (int *)maddr;
    sleep(12);
    for (i = 0; i < 10; i++)
    {
        printf("平方:%d\n", (*q) * (*q));
        printf("平方根:%f\n", sqrt(*q));
        sleep(1);
        q++;
    }
    if (shmdt(maddr) == -1)
    {
        printf("shmdt error");
    }
    if (shmctl(mid, IPC_RMID, 0) == -1)
    {
        printf("shmctl error");
    }
    return 0;
}
