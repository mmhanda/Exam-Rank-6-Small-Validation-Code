#include <libc.h>

typedef struct client {
    int id;
    char msg[100000];
} t_client;

t_client clients[1024];

int max = 0, next_id = 0;
fd_set active, readyRead, readyWrite;
char bufRead[100000], bufWrite[100000];

void exitError(char *str) {
    if (str)
        write(2, str, strlen(str));
    exit(1);
}

void sendAll(int es) {
    for(int i = 0; i <= max; i++)
        if (FD_ISSET(i, &readyWrite) && i != es)
            send(i, bufWrite, strlen(bufWrite), 0);
}

int main(int argc, char **argv) {
    if (argc != 2) exitError("Wrong number of arguments\n");
  	int sockfd = socket(AF_INET, SOCK_STREAM, 0); // from main Unitl line
	if (sockfd == -1) exitError("Fatal error\n");
    max = sockfd;
    FD_ZERO(&active);
    FD_SET(sockfd, &active);
	struct sockaddr_in servaddr; // from main Unitl line
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1]));
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) exitError("Fatal error\n");
	if (listen(sockfd, 128) != 0) exitError("Fatal error\n");  // it is 10 in the main replace it with 128
    while(1)
    {
        readyRead = readyWrite = active;
        if (select(max + 1, &readyRead, &readyWrite, NULL, NULL) < 0) continue;
        for(int fd = 0; fd <= max; fd++) {
            if (FD_ISSET(fd, &readyRead) && fd == sockfd) {
                int clientSocket = accept(sockfd, NULL, NULL);
                if (clientSocket < 0) continue;
                max = (clientSocket > max) ? clientSocket : max;
                clients[clientSocket].id = next_id++;
                FD_SET(clientSocket, &active);
                sprintf(bufWrite, "server: client %d just arrived\n", clients[clientSocket].id);
                sendAll(clientSocket);
                break;
            }
            else if (FD_ISSET(fd, &readyRead) && fd != sockfd) {
                int read = recv(fd, bufRead, 100000, 0);
                if (read <= 0) {
                    sprintf(bufWrite, "server: client %d just left\n", clients[fd].id);
                    sendAll(fd);
                    FD_CLR(fd, &active);
                    close(fd);
                    break;
                }
                else {
                    for (int i = 0, j = strlen(clients[fd].msg); i < read; i++, j++) {
                        clients[fd].msg[j] = bufRead[i];
                        if (clients[fd].msg[j] == '\n') {
                            clients[fd].msg[j] = '\0';
                            sprintf(bufWrite, "client %d: %s\n", clients[fd].id, clients[fd].msg);
                            sendAll(fd);
                            bzero(&clients[fd].msg, strlen(clients[fd].msg));
                            j = -1;
                        }
                    }
                    break;
                }
            }
        }
    }
}