#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>
#include "talk.h"
#define BLACK_LOG 4096
#define MAXLEN 1000
#define OKAY 3
#define LOCAL 0
#define REMOTE (LOCAL + 1)

void chat_handler(int sockfd) {
    int done, len, mlen;
    char buff[MAXLEN];
    struct pollfd fds[REMOTE + 1];
    const char *disconnect = "\n\nConnection closed. ^C to terminate.\n";

    fds[LOCAL].fd = STDIN_FILENO;
    fds[LOCAL].events = POLLIN;
    fds[LOCAL].revents = 0;
    fds[REMOTE] = fds[LOCAL];
    fds[REMOTE].fd = sockfd;

    done = 0;

    do {
        poll(fds, sizeof(fds)/sizeof(struct pollfd), -1);
        if (fds[LOCAL].revents & POLLIN) {
            update_input_buffer();
            if (has_whole_line()) {
                len = read_from_input(buff, MAXLEN);
                mlen = send(sockfd, buff, len, 0);
            }
            if (has_hit_eof()) {
                    done = 1;
            }
        }
        if (fds[REMOTE].revents & POLLIN) {
            mlen = recv(sockfd, buff, sizeof(buff), 0);
                if (mlen == 0){
                    write_to_output(disconnect, strlen(disconnect));
                    pause();
                }
            len = write_to_output(buff, mlen);
            /* write(STDOUT_FILENO, buff, mlen); */
        }
        if (!strncmp(buff, "bye", 3)) {
            done = 1;
        }
    } while (!done);
}

void server(int port_num) {
    struct sockaddr_in sa;
    struct sockaddr_in mysockinfo;
    struct sockaddr_in peerinfo;
    char peeraddr[INET_ADDRSTRLEN];
    char buff[MAXLEN + 1];
    char c[3];
    int newsock;
    int sockfd;
    socklen_t len;

    /* Clear out mem blocks for buffer and structs */
    memset(buff, '\0', MAXLEN + 1);
    memset(&sa, 0, sizeof(sa));

    /* Initilize a socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* Setup port/address info for sock struct */
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port_num);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    /* bind the address to the socket */
    if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
        perror("bind() failed");
        close(sockfd);
        exit(-1);
    }

    /* tells socket to listen for pending connections */
    listen(sockfd, BLACK_LOG);

    /* accept the connection to the client, through a new socket */
    len = sizeof(mysockinfo);
    newsock = accept(sockfd, (struct sockaddr *)&peerinfo, &len);

    /* listens for incoming packets through newsock */
    if (recv(newsock, buff, sizeof(buff) , 0) == -1) {
        perror("recv failed");
    }
    
    /* converts the client's network address into a character string */
    inet_ntop(AF_INET, &peerinfo.sin_addr.s_addr, peeraddr, sizeof(peeraddr));
    
    /* prompt the user, and get input */
    printf("Mytalk request from %s@%s  Accept (y/n)? ", buff, peeraddr);
    fgets(c, 3, stdin);
    
    /* check the option collected from user */
    if (strncmp(c, "y", 1) == 0 || strncmp(c, "yes", 3) == 0) {

        /* server sends packet "ok" to client */
        if (send(newsock, "ok", 3, 0) == -1) {
            perror("Send Failed here");
        }
        /* printf("open ncurses here\n"); */
        start_windowing();
        chat_handler(newsock);
    } 
    else {
        exit(1);
    }
    close(sockfd);
    close(newsock);
}
void client(char *hostname, int port_num) {
    struct sockaddr_in sa;
    struct hostent *hostent;
    struct passwd *pwd;
    char buff[MAXLEN + 1];
    char ans[OKAY];
    int sockfd;

    /* clear out memory for the buffer */
    memset(buff, '\0', MAXLEN + 1);
    memset(ans, '\0', OKAY);

    /* create a socket on the specified port */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* retrieve the hostname */
    hostent = gethostbyname(hostname);

    /* Setup port/address info for sock struct */
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port_num);
    sa.sin_addr.s_addr = *(uint32_t*)hostent->h_addr_list[0]; 

    /* connect to sockfd at the sock address specified */
    connect(sockfd, (struct sockaddr *)&sa, sizeof(sa));

    /* make passwrd struct then pull the user ID */
    pwd = getpwuid(getuid());

    /* send a packet to server containing clients user ID info */
    if (send(sockfd, pwd->pw_name, strlen(pwd->pw_name), 0) == -1){
        perror("Send failed.");
    }
    printf("Waiting on response from %s.\n", hostname);

    /* recieve a packet containing the servers response "ok" if (y) */
    if (recv(sockfd, ans, 3, 0) != -1) {

        if ((strcmp(ans, "ok")) == 0) {
            /* printf("open ncurses here\n"); */
            start_windowing();
            chat_handler(sockfd);
        }
        else {
            printf("%s has declined connection.\n", hostname);
            exit(1);
        }
    }
    else {
        perror("recv failed here");
    }

    close(sockfd);
}
int main(int argc, char *argv[]) {

    if (argc > 3 || argc < 2) {
        printf("Usage: mytalk [ hostname ] port\n");
    }

    if (argc == 2) {
        /*  Acts as a SERVER, opening a listening socket on 
            local machine at the given port. */
        int port; 
        port = strtol(argv[1], NULL, 10);
        server(port); 
    } 
    if (argc == 3) {
        /*  Acts as a CLIENT. Hostname is the name of the remote 
            host to which to connect at the given port. */
        int port; 
        port = strtol(argv[2], NULL, 10);
        client(argv[1], port);
    }
    return 0;
}
