#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>

const int reptileNum = 5;   // 爬虫进程个数
const int analyzerNum = 5;  // 页面url分析进程个数
const int downloadNum = 5;  // 下载进程个数
const char Url[] = "http://xxx.com";

// URL，使用共享内存
#define IPC_KEY 0x12312345

int init();
int reptile();

void read_pipe (int file);
void write_pipe (int file);

int main() {
    init();
    int num = 0;
    int filedes[2];
    pid_t pids[15];
    //创建管道
    if (pipe (filedes))
    {
        fprintf(stderr, "Pipe failed. \n");
        return EXIT_FAILURE;
    }


    // 创建爬虫
    for (int i = 0;i < reptileNum;i++) {
        pid_t pid = fork();
        if (pid == 0) {
            close(filedes[1]);
            read_pipe(filedes[0]);
            reptile();
            return 0;
        } else {
            pids[num] = pid;
            num++;
            close(filedes[0]);
            write_pipe(filedes[1]);
        }
    }
    // 创建页面url分析进程
    for (int i = 0;i < analyzerNum;i++) {
        pid_t pid = fork();
        if (pid == 0) {
            return 0;
        } else {
            pids[num] = pid;
            num++;
        }
    }
    // 创建下载图片进程
    for (int i = 0;i < downloadNum;i++) {
        pid_t pid = fork();
        if (pid == 0) {
            return 0;
        } else {
            pids[num] = pid;
            num++;
        }
    }
    /*for (int i = 0;i < num;i++) {
        printf("pid: %d\n", pids[i]);
    }*/
}

int init() {
	int shmid = shmget(IPC_KEY, 32, IPC_CREAT | 0664);
	if(shmid < 0)
	{
		perror("shmget error");
		return -1;
	}
	void * shm_start = shmat(shmid,NULL,0);
	if(shm_start == (void *) -1)
	{
		perror("shmat error");
		return -1;
	}
}

void read_pipe (int file)
{
    FILE *stream;
    int c;
    stream = fdopen (file, "r");
    while ((c = fgetc (stream)) != EOF)
        putchar (c);
    fclose (stream);
}

void write_pipe (int file)
{
    FILE *stream;
    stream = fdopen (file, "w");
    fprintf (stream, Url);
    fclose (stream);
}

/**
 * 爬虫
 * @param
 * @return
 * int reptile;
 */
int reptile() {
    printf("爬虫\n");
    return 0;
}

/**
 * 页面url分析
 * @param
 * @return
 * int analyzer;
 */
int analyzer() {
    printf("分析\n");
    return 0;
}

/**
 * 下载图片
 * @param
 * @return
 * int download;
 */
int download() {
    printf("下载\n");
    return 0;
}