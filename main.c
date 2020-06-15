#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

const int reptileNum = 5;   // 爬虫进程个数
const int analyzerNum = 5;  // 页面url分析进程个数
const int downloadNum = 5;  // 下载进程个数
const char path[] = "./image/";
const char Url[] = "http://xxx.com";

const int PORT = 80;

int urlPipe[2];         // url管道
int imageUrlPipe[2];    // 图片url管道

char HTTPRequestHeader[] = "";

// URL，使用共享内存
#define IPC_KEY 0x12312345

int init();
int reptile();
int analyzer();
int download();
struct hostent * get_host_by_name(char hostname[]);

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
        exit(-1);
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

    int sockfd, ret, i;
    struct sockaddr_in serverAddr;

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    struct hostent *h = get_host_by_name("www.baidu.com");
    printf("HostName :%s/n",h->h_name);
    printf("IP Address :%s/n",inet_ntoa(*((struct in_addr *)h->h_addr)));
//    if (inet_pton(AF_INET, IPSTR, &serverAddr.sin_addr) <= 0) {
//        printf("创建网络连接失败,本线程即将终止--inet_pton error!\n");
//        exit(0);
//    }

    if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        printf("连接到服务器失败,connect error!\n");
        exit(0);
    }
    printf("与远端建立了连接\n");

    //发送数据
    ret = write(sockfd, requestHeader, strlen(requestHeader));
    if(ret < 0) {
        printf("发送失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
        exit(0);
    }else{
        printf("消息发送成功，共发送了%d个字节！\n\n", ret);
    }

    fd_set   t_set;
    FD_ZERO(&t_set);
    FD_SET(sockfd, &t_set);

//    struct timeval tv;
//    char buf[1024];
//    while(1){
//        sleep(2);
//        tv.tv_sec= 0;
//        tv.tv_usec= 0;
//        h= 0;
//        printf("--------------->1");
//        h= select(sockfd +1, &t_set, NULL, NULL, &tv);
//        printf("--------------->2");
//
//        if(h < 0) {
//            close(sockfd);
//            printf("在读取数据报文时SELECT检测到异常，该异常导致线程终止！\n");
//            return-1;
//        }
//
//        if(h > 0) {
//            memset(buf, 0, 4096);
//            i= read(sockfd, buf, 4095);
//            if(i==0){
//                close(sockfd);
//                printf("读取数据报文时发现远端关闭，该线程终止！\n");
//                return-1;
//            }
//            printf("%s\n", buf);
//        }
//    }
    close(sockfd);
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

/**
 * 域名解析
 * @param   char hostname[]
 * @return  he hostent
 * struct hostent * get_host_by_name(char hostname[]);
 */
struct hostent * get_host_by_name(char hostname[]) {
    struct hostent *h;
    if ((h = gethostbyname(hostname)) == NULL) {
         fprintf(stderr,"不能得到IP/n");
         exit(1);
     }
    return h;
}