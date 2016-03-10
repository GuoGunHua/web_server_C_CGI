

/* J. David's webserver */
/* This is a simple webserver.
 * Created November 1999 by J. David Blackstone.
 * CSE 4344 (Network concepts), Prof. Zeigler
 * University of Texas at Arlington
 */
/* This program compiles for Sparc Solaris 2.6.
 * To compile for Linux:
 *  1) Comment out the #include <pthread.h> line.
 *  2) Comment out the line that defines the variable newthread.
 *  3) Comment out the two lines that run pthread_create().
 *  4) Uncomment the line that runs accept_request().
 *  5) Remove -lsocket from the Makefile.
 */


#include "http_sever.h"
#include "standard_c_interface.h"
#include "save_data.h"


#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"

void *accept_request(void *);
void bad_request(int);
void cat(int, FILE *);
void cannot_execute(int);
void error_die(const char *);
void execute_cgi(int, const char *, const char *, const char *);
int get_line(int, char *, int);
void headers(int, const char *);
void not_found(int);
void serve_file(int, const char *);
int startup(u_short *);
void unimplemented(int);
int print_all(int socket);
void *sig_pipe(int sign);
/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 
 返回值和参数
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
void *accept_request(void *client_pin)
{
    DEBUG("进入多线程 处理连接！");

    char buf[1024]={0};
    int numchars;
    char method[255]={0};
    char url[255];
    char path[512];
    size_t i, j;
//    struct stat st;
    int cgi = 0;
    int client = *(int *)client_pin;


    /* becomes true if server decides this is a CGI * program */
    char *query_string = NULL;
    
    //读取一行
    numchars = get_line(client, buf, sizeof(buf));
    i = 0; j = 0;
    while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
    {
        method[i] = buf[j];
        i++; j++;
    }
    method[i] = '\0';
    
    DEBUG("buf=%s\n method=%s\n",buf,method);
//    if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
//    {
//	DEBUG("********unimplemented client*******************\n");
//        unimplemented(client);
//        return NULL;
//    }
    
    if (strcasecmp(method, "POST") == 0)
        cgi = 1;
    
    i = 0;
    while (ISspace(buf[j]) && (j < sizeof(buf)))
        j++;
    
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
    {
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';
    
    
    //----根据 url 获取数据 保存到 数据库--
    
   //print_all(client);
    save_data(url);
    
    //-----------
    DEBUG("存储完毕，返回请求信息 %s \n",url);
    
/*
    if (strcasecmp(method, "GET") == 0)// GET请求参数 转换成 post请求
    {
        query_string = url;
        while ((*query_string != '?') && (*query_string != '\0'))
            query_string++;
        
        if (*query_string == '?')
        {
            cgi = 1;
            *query_string = '\0';
            query_string++;
        }
    }
    
    sprintf(path,"%s", url);
    if (path[strlen(path) - 1] == '/')
        strcat(path, "index.html");
    DEBUG("%s",path);
 */   
    
    if (strstr(url,"line") != NULL)
    {
        serve_file(client,"./index.html");
    }
    else
    {
        //没有找到请求的资源 ， 废弃的数据 读出来 丢掉
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
                numchars = get_line(client, buf, sizeof(buf));

        not_found(client);
        
        //serve_file(client,"/Users/admin/Desktop/C_test/C_test/index.html");
    }
    
    
    
//    if (stat(path, &st) == -1)
//    {
//        //没有找到请求的资源 ， 废弃的数据 读出来 丢掉
//        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
//            numchars = get_line(client, buf, sizeof(buf));
//        
//        not_found(client);
//    }
//    else
//    {// 返回 请求结果
////        if ((st.st_mode & S_IFMT) == S_IFDIR)
////            strcat(path, "/index.html");
////        
////        if ((st.st_mode & S_IXUSR) ||
////            (st.st_mode & S_IXGRP) ||
////            (st.st_mode & S_IXOTH)    )
////            cgi = 1;
////        
////        DEBUG("client=%d,path=%s,method=%s,query_string=%s",client,path,method,query_string);
////        
////        if (!cgi)
////            serve_file(client, path);//GET 请求
////        else
////            execute_cgi(client, path, method, query_string); //post 请求
//        
//        
//        
//        serve_file(client,"/Users/admin/Desktop/C_test/C_test/index.html");
//    }
    
    close(client);
    //pthread_exit(NULL);
    
    return NULL;
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 
 请求错误 400 错误码
 
 * Parameters: client socket */
/**********************************************************************/
void bad_request(int client)
{
    char buf[1024];
    
    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client, buf, sizeof(buf), 0);
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat(int client, FILE *resource)
{
    unsigned char buf[1024] = {0};
    
    fgets((char*)buf, sizeof(buf), resource);
    while (!feof(resource))
    {
        send(client, buf, strlen((char*)buf), 0);
        fgets((char*)buf, sizeof(buf), resource);
    }
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 
 无法执行请求的脚本 错误码 500
 
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannot_execute(int client)
{
    char buf[1024];
    
    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 
 出错退出
 
 * program indicating an error. */
/**********************************************************************/
void error_die(const char *sc)
{
    perror(sc);
    if (errno == EINTR)
        return;
    
    
    exit(1);
}

/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 
 执行一个 CGI 脚本
 
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
void execute_cgi(int client, const char *path,const char *method, const char *query_string)
{
    char buf[1024];
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;
    int i;
    char c;
    int numchars = 1;
    int content_length = -1;
    
    buf[0] = 'A'; buf[1] = '\0';
    if (strcasecmp(method, "GET") == 0)
    {
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers   读取并丢弃请求头部*/
            numchars = get_line(client, buf, sizeof(buf));
    }
    else /* POST */
    {
        numchars = get_line(client, buf, sizeof(buf));
        while ((numchars > 0) && strcmp("\n", buf))
        {
            buf[15] = '\0';
            if (strcasecmp(buf, "Content-Length:") == 0)
                content_length = atoi(&(buf[16]));
            numchars = get_line(client, buf, sizeof(buf));
        }
        if (content_length == -1)
        {//请求 参数 出错
            bad_request(client);
            return;
        }
    }
    
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    
    if (pipe(cgi_output) < 0)//创建 管道 。。fd[0]:读管道，fd[1]:写管道
    {
        cannot_execute(client);
        return;
    }
    if (pipe(cgi_input) < 0)
    {
        cannot_execute(client);
        return;
    }
    
    if ( (pid = fork()) < 0 )
    {
        cannot_execute(client);
        return;
    }
    if (pid == 0)  /* child: CGI script  子进程  */
    {
        char meth_env[255];
        char query_env[255];
        char length_env[255];
        
        dup2(cgi_output[1], 1);
        dup2(cgi_input[0], 0);
        close(cgi_output[0]);
        close(cgi_input[1]);
        sprintf(meth_env, "REQUEST_METHOD=%s", method);
        putenv(meth_env);
        if (strcasecmp(method, "GET") == 0)
        {
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            putenv(query_env);
        }
        else
        {   /* POST */
            sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
            putenv(length_env);
        }
        execl(path, path, NULL);
        exit(0);
    }
    else
    {    /* parent    父进程 */
        close(cgi_output[1]);
        close(cgi_input[0]);
        if (strcasecmp(method, "POST") == 0)
            for (i = 0; i < content_length; i++)
            {
                recv(client, &c, 1, 0);
                write(cgi_input[1], &c, 1);
            }
        
        while (read(cgi_output[0], &c, 1) > 0)
            send(client, &c, 1, 0);
        
        close(cgi_output[0]);
        close(cgi_input[1]);
        waitpid(pid, &status, 0);
    }
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 
 从套接字读取一行，换行符，回车符，或一个CRLF组 结束行。以空字符结尾的字符串读取。
 如果没有新的指标是缓冲结束前发现，字符串是以空字符结束。

 参数和返回值
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) 返回值为读取的长度，不包括null
 */
/**********************************************************************/
int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    
    //------- '\n'换行符，'\r'回车符
    while ((i < size - 1) && (c != '\n'))
    {
//	DEBUG("recv 阻塞");
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        
        if (n > 0)
        {
            if (c == '\r') // 判断是否为行结尾
            {
                n = recv(sock, &c, 1, MSG_PEEK);   //MSG_PEEK,窥看外来消息,并不读取出来，只是复制出来，数据还在协议中
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            
            // 拷贝读取的数据 ，继续
            buf[i] = c;
            i++;
        }
        else
        {
            c = '\n';
        }
	
//	printf("%c",c);
    }
    
    buf[i] = '\0';

//    DEBUG("get_line buf=%s\n",buf);    
    return(i);
}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 
 返回请求     数据的头部
 
 *             the name of the file */
/**********************************************************************/
void headers(int client, const char *filename)
{
    char buf[1024];
    (void)filename;  /* could use filename to determine file type */
    
    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found(int client)
{
    char buf[1024];
    
    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html;charset=utf-8\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>服务器说它没找到你要的资源</p>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>别瞎鸡巴乱点！</P>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>你还真以为有片看啊</P>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 
 发送普通文件，如：GET 请求发送静态文件；
 
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_file(int client, const char *filename)
{
    FILE *resource = NULL;
    int numchars = 1;
    char buf[1024];
    
    buf[0] = 'A'; buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers 读取请求 头部的其他数据，并丢弃 */
        numchars = get_line(client, buf, sizeof(buf));
    
    resource = fopen(filename, "r");
    if (resource == NULL)
    {// 打开文件失败  返回404 错误
        not_found(client);
    }
    else
    {
        headers(client, filename);
        cat(client, resource);
    }
    fclose(resource);
}

/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 
 参数和返回值
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
int startup(u_short *port)
{
    int httpd = 0;
    struct sockaddr_in name;
    
    //创建套接字
    httpd = socket(PF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
        error_die("socket");
    
    //端口重用
    int reuse = 0;
    if (setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopet error\n");
        return -1;
    }
    
    
    //设置地址  本地
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //绑定地址和端口
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
        error_die("bind");
    
    //如果端口号  不可用，则动态分配端口
    if (*port == 0)  /* if dynamically allocating a port */
    {
        int namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name,(socklen_t*)&namelen) == -1)
            error_die("getsockname");
        *port = ntohs(name.sin_port);
    }
    
    DEBUG("bind = ip:%s     port:%d\n",inet_ntoa(name.sin_addr),*port);
    
    //监听
    if (listen(httpd, 3) < 0)
        error_die("listen");
    
    return(httpd);
}

/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(int client)
{
    char buf[1024];
    
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/*****************************************************************/
/*
 
 测试
 
 */
/*****************************************************************/
int print_all(int socket)
{
    char buf[4096] = {0};
    int n = 0;
    
    n = (int)recv(socket, buf, 4096, MSG_PEEK);
    save_data(buf);
    
    
//    while (n>=0)
//    {
//        n = (int)recv(socket, buf, 4096, MSG_PEEK);
//       // printf("%s",buf);
//        
//        save_data(buf);
//        
//       // if (strstr(buf,"\r\n\r\n") != NULL)
//        break;
//    }
    
    return 0;
}


/**********************************************************************/
int main_http_sever(void)
{
    int server_sock = -1;
    u_short port = 8888;
    int client_sock = -1;
    struct sockaddr_in client_name;
    int client_name_len = sizeof(client_name);
    int newthread;
    
    //注册信号处理函数  http://blog.csdn.net/xsckernel/article/details/8548393
    signal(SIGPIPE,sig_pipe(SIGPIPE));

    
    server_sock = startup(&port);
    printf("httpd sever running on port %d\n", port);
    
    while (1)
    {
        //等待连接
	DEBUG("处理完毕 accept 等待 新的  连接 ");
        client_sock = accept(server_sock,(struct sockaddr *)&client_name,(socklen_t*)&client_name_len);
        
        if (errno == EINTR)
            continue;
        
        if (client_sock == -1)
            error_die("accept 失败");
	else
	    DEBUG("accpet 成功!\n");
        
        //多线程  处理连接
         accept_request((void*)&client_sock);
        /*if (pthread_create((pthread_t*)&newthread , NULL, accept_request, (void*)&client_sock) != 0)
            perror("pthread_create");
        */
    }
    
    //close(server_sock);
    return(0);
}

//信号处理函数
void *sig_pipe(int sign)
{
    DEBUG("Catch a SIGPIPE signal\n");
    
    
    //    struct sigaction sa;
    //    sa.sa_handler = SIG_IGN;
    //    sigaction( SIGPIPE, &sa, 0 );
    
    return NULL;
}


