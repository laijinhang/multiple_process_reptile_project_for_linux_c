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
    httpGet("/vivoportal/files/image/navi/20190315/a58140a62b1189f49ea4f3a0abedb664.png", "wwwstatic.vivo.com.cn", "");
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
    strcat(requestHeader, " HTTP/1.1\r\n");
    strcat(requestHeader, "Host: ");
    strcat(requestHeader, host);
    strcat(requestHeader, "\r\n");

    strcat(requestHeader, "Content-Type:application/x-www-form-urlencoded;\r\n");
    strcat(requestHeader, "Connection: keep-alive\r\n");
    strcat(requestHeader, "Cache-Control: max-age=0\r\n");
    strcat(requestHeader, "Upgrade-Insecure-Requests: 1\r\n");
    strcat(requestHeader, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\n");
    strcat(requestHeader, "Accept-Encoding: gzip, deflate\r\n");
    strcat(requestHeader, "Accept-Language: zh-CN,zh;q=0.9\r\n");
    strcat(requestHeader, "\r\n");
    printf("%s", requestHeader);

    int sockfd, ret, i;
    struct sockaddr_in serverAddr;

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    struct hostent *h = get_host_by_name(host);
    printf("HostName :%s\n",h->h_name);
    printf("IP Address :%s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));
    int numbytes;
    char buf[BUFSIZ];
    struct sockaddr_in their_addr;
    printf("break!");
    while((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1);
    printf("We get the sockfd~\n");
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(443);
    their_addr.sin_addr.s_addr=inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr)));
    bzero(&(their_addr.sin_zero), 8);

    for(int r = connect(sockfd, (struct sockaddr*)&their_addr, sizeof(struct sockaddr));r == -1;r = connect(sockfd, (struct sockaddr*)&their_addr, sizeof(struct sockaddr)));
    printf("Get the Server~Cheers!%d\n", BUFSIZ);
    numbytes = send(sockfd, requestHeader, strlen(requestHeader), 0);
    numbytes=recv(sockfd,buf,BUFSIZ,0);
    buf[numbytes]='\0';
    printf("received:%s\n",buf);
    close(sockfd);


    fd_set t_set;
    FD_ZERO(&t_set);
    FD_SET(sockfd, &t_set);

    struct timeval tv;
    while(1){
        sleep(2);
        tv.tv_sec= 0;
        tv.tv_usec= 0;
        h = 0;
        printf("--------------->1");
        int d;
        d = select(sockfd +1, &t_set, NULL, NULL, &tv);
        printf("--------------->2");

        if(d < 0) {
            close(sockfd);
            printf("在读取数据报文时SELECT检测到异常，该异常导致线程终止！\n");
            return-1;
        }

        if(h > 0) {
            memset(buf, 0, 4096);
            i= read(sockfd, buf, 4095);
            if(i==0){
                close(sockfd);
                printf("读取数据报文时发现远端关闭，该线程终止！\n");
                return-1;
            }
            printf("%s\n", buf);
        }
    }
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