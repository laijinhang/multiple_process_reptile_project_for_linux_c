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
int analyzer();
int download();

int run(int num, int (*a)()) {
    for (int i = 0;i < num;i++) {
        pid_t pid = fork();
        if (pid == 0) {
            a();
            exit(0);
        }
    }
    return 0;
}
int httpGet(char uri[], char host[], char resp[]);
void read_pipe (int file);
void write_pipe (int file);

int main() {
    signal(SIGCHLD, SIG_IGN);
    init();
    // 创建爬虫
    run(reptileNum, reptile);
    // 创建页面url分析进程
    run(analyzerNum, analyzer);
    // 创建下载图片进程
    run(downloadNum, download);
    wait(NULL);
    printf("主进程结束!\n");
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

    int client;
    return 0;
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