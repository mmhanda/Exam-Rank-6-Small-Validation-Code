#include <libc.h>

typedef struct client {
    int id;
    char msg[424242];
} t_client;

t_client clients[1024];
char buffRead[424242], buffWrite[424242];
int max = 0, next_id = 0;
fd_set active_fds, Read_fds, Write_fds;

void exitError(char *str) {
    write(2, str, strlen(str));
    exit(1);
}

void sendMsg(int sender_fd) {
    for (int fd = 0; fd <= max; fd++)
        if (FD_ISSET(fd, &Write_fds) && fd != sender_fd) write(fd, buffWrite, strlen(buffWrite));
}

int main(int ac, char **av) {
    if (ac != 2) exitError("Wrong number of arguments\n");
  	int sockfd = max = socket(AF_INET, SOCK_STREAM, 0); // copy from subject/main.c Unitl line 27
	if (sockfd == -1) exitError("Fatal error\n"); // copy from subject/main.c
    FD_ZERO(&active_fds);
    FD_SET(sockfd, &active_fds);
	struct sockaddr_in servaddr; // copy from subject/main.c from subject/main.c Unitl line 35
	servaddr.sin_family = AF_INET; // copy from subject/main.c
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1 // copy from subject/main.c
	servaddr.sin_port = htons(atoi(av[1])); // copy from subject/main.c change the PORT
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) exitError("Fatal error\n"); // copy from subject/main.c
	if (listen(sockfd, 10) != 0) exitError("Fatal error\n"); // copy from subject/main.c
    while(1) {
        Read_fds = Write_fds = active_fds;
        if (select(max + 1, &Read_fds, &Write_fds, NULL, NULL) < 0) continue;
        for (int fd = 0; fd <= max; fd++) {
            if (FD_ISSET(fd, &Read_fds) && fd == sockfd) { // hnaya sorry in english here join new client
                int ClientSocket = accept(sockfd, NULL, NULL);
                max = (ClientSocket > max) ? ClientSocket : max;
                clients[ClientSocket].id = next_id++;
                bzero(clients[ClientSocket].msg, strlen(clients[ClientSocket].msg)); // protection
                FD_SET(ClientSocket, &active_fds);
                sprintf(buffWrite, "server: client %d just arrived\n", clients[ClientSocket].id);
                sendMsg(ClientSocket);
                break;
            }
            if (FD_ISSET(fd, &Read_fds) && fd != sockfd) { // recive msgs from clients if its empty than the client is quit
                int read = recv(fd, buffRead, sizeof(buffRead), 0);
                if (read <= 0) { // handle quit
                    sprintf(buffWrite, "server: client %d just left\n", clients[fd].id);
                    sendMsg(fd);
                    FD_CLR(fd, &active_fds);
                    close(fd);
                    break;
                }
                else { // handel msg
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