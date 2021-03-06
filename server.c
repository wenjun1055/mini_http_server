#include "server.h"

int send_header()
{
    
}

/**
 * show help
 */
static int show_help()
{
    fprintf(stderr, "\n********* Http Mini Server **********\n\n");
    fprintf(stderr, "Usage: ./server [OPTION] ...\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "-D, --is_debig      Is open debug mode, default No\n");
    fprintf(stderr, "-p, --port=PORT     Server listen port, default 80\n");
    fprintf(stderr, "-l, --is_log        Is write access log, default No\n");
    fprintf(stderr, "\n*************************************\n");
}

/**
 * parse options
 */
static int parse_options(int argc, char **argv)
{
    int opt;
    struct option longopts[] = {
        {"port",        1,  NULL,   'p'},
        {"is_log",      0,  NULL,   'l'},
        {NULL,          0,  NULL,     0}
    };
    
    while (-1 != (opt = getopt_long(argc, argv, "p:l", longopts, NULL))) {
        switch (opt) {
            case 'p':
                g_port = atoi(optarg);
                if (g_port < 1 || g_port > 65535) {
                    fprintf(stderr, "Error:Options -p,--port error: input port number %d invalid, must between 1 - 65535.\n\n", g_port);
                    return -1;
                }
                break;
            case 'l':
                g_is_log = 1;
                break;
            default:
                show_help();
        }
    }

    return 0;
}

static int response_header(int client_sock, int http_code)
{
    time_t now;
    char buf[BUFFER_SIZE], buf_all[BUFFER_SIZE], timebuf[100];

    memset(buf_all, 0, strlen(buf_all));

    memset(buf, 0, strlen(buf));
    sprintf(buf, "HTTP/1.1 %d OK\r\n", http_code);
    strcat(buf_all, buf);

    memset(buf, 0, strlen(buf));
    sprintf(buf, "Server: %s\r\n", SERVER_NAME);
    strcat(buf_all, buf);

    now = time((time_t *)0);
    strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
    memset(buf, 0, strlen(buf));
    sprintf(buf, "Date: %s\r\n", timebuf);
    strcat(buf_all, buf);

    memset(buf, 0, strlen(buf));
    sprintf(buf, "Cache-Control: private, max-age=0\r\n");
    strcat(buf_all, buf);

    memset(buf, 0, strlen(buf));
    sprintf(buf, "Content-Type: text/html; charset=UTF-8\r\n");
    strcat(buf_all, buf);

    // memset(buf, 0, strlen(buf));
    // sprintf(buf, "Content-Length: %d\r\n", 100);        
    // strcat(buf_all, buf);

    memset(buf, 0, strlen(buf));
    sprintf(buf, "Connection: close\r\n\r\n");
    strcat(buf_all, buf);

    printf("\n%s\n", buf_all);

    write(client_sock, buf_all, strlen(buf_all));

    return 0;
}

parse_request(int client_sock)
{
    char request_buf[REQUEST_MAX_SIZE];

    if (read(client_sock, request_buf, REQUEST_MAX_SIZE) < 0) {
        //TODO 接收参数错误
        printf("Read sock error\n");
    }

    if (IS_DEBUG) {
        printf("Receive request:\n%s\n", request_buf);        
    }

    


}


static void handle_client(int client_sock, struct sockaddr_in client_addr)
{
    parse_request(client_sock);



    // response_header(client_sock, 200);

    // close(client_sock);
}


int main(int argc, char **argv)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;

    if (argc > 1) {
        if (0 != parse_options(argc, argv)) {
            exit(-1);
        }
    }
    // printf("port=%d\tlog=%d\n", g_port, g_is_log);

    if ((server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Create socket failed\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(g_port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind server socket failed\n");
        return -1;
    }

    if (listen(server_sock, 2) < 0) {
        printf("Listen server socket failed\n");
        return -1;
    }

    while(1) {
        unsigned int client_len = sizeof(client_addr);

        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            printf("Accept client connection failed\n");
            return -1;
        }

        handle_client(client_sock, client_addr);
        close(client_sock);
            // printf("client_sock close\n");

        // if (0 == fork()) {
        //     handle_client(client_sock, client_addr);

        //     close(client_sock);
        //     printf("client_sock close\n");
        //     exit(0);
        // } else {
        //     wait(NULL);
        // }
    }

    return 0;
}
