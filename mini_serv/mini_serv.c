#include <libc.h>

typedef struct client {
    int id;
    char msg[100000];
} t_client;

t_client clients[1024];

int max = 0, next_id = 0;
fd_set active_fds, fds_Read, fds_Write;
char buffRead[100000], buffWrite[100000];

void exitError(char *str) {
    write(2, str, strlen(str));
    exit(1);
}

void sendMsg(int sender_fd) {
    for(int fd = 0; fd <= max; fd++)
        if (FD_ISSET(fd, &fds_Write) && fd != sender_fd) write(fd, buffWrite, strlen(buffWrite));
}

int main(int ac, char **av) {
    if (ac != 2) exitError("Wrong number of arguments\n");
  	int sockfd = max = socket(AF_INET, SOCK_STREAM, 0); // copy from subject/main.c Unitl line 27
	if (sockfd == -1) exitError("Fatal error\n");      // copy
    FD_ZERO(&active_fds);
    FD_SET(sockfd, &active_fds);
	struct sockaddr_in servaddr; // copy from subject/main.c Unitl line 35
	servaddr.sin_family = AF_INET; // copy
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1 // copy
	servaddr.sin_port = htons(atoi(av[1]));                  // copy
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) exitError("Fatal error\n"); // copy
	if (listen(sockfd, 128) != 0) exitError("Fatal error\n");  // replace 10 with 128 // copy
    while(1) {
        fds_Read = fds_Write = active_fds;
        if (select(max + 1, &fds_Read, &fds_Write, NULL, NULL) < 0) continue;
        for(int fd = 0; fd <= max; fd++) {
            if (FD_ISSET(fd, &fds_Read) && fd == sockfd) { // hnaya sorry in english join new client
                int newfd = accept(sockfd, NULL, NULL);
                max = (newfd > max) ? newfd : max;
                clients[newfd].id = next_id++;
                FD_SET(newfd, &active_fds);
                sprintf(buffWrite, "server: client %d just arrived\n", clients[newfd].id);
                sendMsg(newfd);
                break;
            }
            if (FD_ISSET(fd, &fds_Read) && fd != sockfd) { // recive msgs from clients if its empty than the client is quit
                int read = recv(fd, buffRead, 100000, 0);
                if (read <= 0) {
                    sprintf(buffWrite, "server: client %d just left\n", clients[fd].id);
                    sendMsg(fd);
                    bzero(clients[fd].msg, strlen(clients[fd].msg));
                    FD_CLR(fd, &active_fds);
                    close(fd);
                    break;
                }
                else {
                    for (int i = 0, j = strlen(clients[fd].msg); i < read; i++, j++) {
                        clients[fd].msg[j] = buffRead[i];
                        if (clients[fd].msg[j] == '\n') {
                            clients[fd].msg[j] = '\0';
                            sprintf(buffWrite, "client %d: %s\n", clients[fd].id, clients[fd].msg);
                            sendMsg(fd);
                            bzero(clients[fd].msg, strlen(clients[fd].msg));
                            j = -1;
                        }
                    }
                    break;
                }
            }
        }
    }
}