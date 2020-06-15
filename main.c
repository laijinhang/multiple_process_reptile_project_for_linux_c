#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

const int reptileNum = 5;   // 爬虫进程个数
const int analyzerNum = 5;  // 页面url分析进程个数
const int downloadNum = 5;  // 下载进程个数
const char Url[] = "http://xxx.com";
int urlPipe[2];         // url管道
int imageUrlPipe[2];    // 图片url管道

char HTTPRequestHeader[] = "";

// URL，使用共享内存
#define IPC_KEY 0x12312345

int init();
int reptile();

int httpGet(char uri[], char host[], char resp[]);
void read_pipe (int file);
void write_pipe (int file);

int main() {
    init();
    int num = 0;
    pid_t pids[15];

    // 创建爬虫
    for (int i = 0;i < reptileNum;i++) {
        pid_t pid = fork();
        if (pid == 0) {
            reptile();
            return 0;
        } else {
            pids[num] = pid;
            num++;
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
//	int shmid = shmget(IPC_KEY, 32, IPC_CREAT | 0664);
//	if(shmid < 0)
//	{
//		perror("shmget error");
//		return -1;
//	}
//	void * shm_start = shmat(shmid,NULL,0);
//	if(shm_start == (void *) -1)
//	{
//		perror("shmat error");
//		return -1;
//	}
    // 创建管道
    if (pipe(urlPipe)) {
        fprintf(stderr, "Pipe failed. \n");
        return EXIT_FAILURE;
    }
    // 写入第一条url
    printf("%ld\n", write(urlPipe[1], Url, strlen(Url)));
}

/**
 * 爬虫
 * @param
 * @return
 * int reptile;
 */
int reptile() {
    printf("爬虫\n");
    char url[100];
    for (;;) {
        if (read(urlPipe[0], url, 30) < 0) {
            printf("error\n");
            return 0;
        }
        printf("读取url: %s\n", url);
    }
    return 0;
}

/**
 * 发起url请求
 * @param char uri[], char host[], char resp[]
 * @return
 * int httpGet(char uri[], char host[], char resp[]);
 */
int httpGet(char uri[], char host[], char resp[]) {
    char requestHeader[4096];
    memset(requestHeader, 0, 4096);
    strcat(requestHeader, "GET ");
    strcat(requestHeader, uri);
    strcat(requestHeader, " HTTP/1.1\n");
    strcat(requestHeader, "Host: ");
    strcat(requestHeader, host);
    strcat(requestHeader, "\n");
    strcat(requestHeader, "Content-Type: text/html\n");
    strcat(requestHeader, "\n\n");

    printf("%s", requestHeader);
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