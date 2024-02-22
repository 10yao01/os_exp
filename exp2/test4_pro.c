#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define KEY 75
#define K 1024
int main()
{
    int shmid, *p, i;
    char *shmaddr;
    shmid = shmget(KEY, K, 0777 | IPC_CREAT);
    if (shmid == -1)
    {
        printf("shmget error");
    }
    shmaddr = shmat(shmid, 0, 0);
    if (shmaddr == (void *)-1)
    {
        printf("shmat error");
    }
    p = (int *)shmaddr;
    for (i = 0; i < 10; i++)
    {
        *p = rand() % 100;
        sleep(1);
        printf("%d\n", *p);
        p++;
    }
    sleep(5);
    if (shmdt(shmaddr) == -1)
    {
        printf("shmdt error");
    }
    printf("end\n");
    return 0;
}
