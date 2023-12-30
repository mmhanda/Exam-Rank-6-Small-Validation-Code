# Exam-Rank-6-Small-64 lines + 10 lines from the subject 42 school
#### don't forget to give me a star :) ⭐
### here is an explanation of each line if you interested
This is a C program that implements a basic server for handling multiple clients. The server listens for incoming connections, manages connected clients, and allows them to send and receive messages. Below is an explanation of each part of the code:

#include <libc.h>
This line includes necessary header files for the C program.

typedef struct client {
    int id;
    char msg[424242];
} t_client;

Defines a t_client structure to represent client information. Each client has an ID and a message buffer.

t_client clients[1024];
Creates an array of t_client structures to store information for up to 1024 clients.

char buffRead[424242], buffWrite[424242];
Defines two character arrays to store incoming and outgoing messages.

int max = 0, next_id = 0;
Initializes max to 0, which keeps track of the maximum file descriptor used, and next_id to 0, which is used to assign unique IDs to clients.

fd_set active_fds, Read_fds, Write_fds;
Defines sets of file descriptors (fd_set) to keep track of active, read, and write file descriptors.

void exitError(char *str) {
    write(2, str, strlen(str));
    exit(1);
}
Defines a custom error handling function that writes an error message to the standard error output and exits the program with an error code.

void sendMsg(int sender_fd) {
    for (int fd = 0; fd <= max; fd++)
        if (FD_ISSET(fd, &Write_fds) && fd != sender_fd) write(fd, buffWrite, strlen(buffWrite));
}
A function to send messages to all clients except the sender. It iterates through the file descriptors, checking if they are in the Write_fds set and not equal to the sender's file descriptor. If so, it sends the message in buffWrite to that client.

int main(int ac, char **av) {
    if (ac != 2) exitError("Wrong number of arguments\n");
The main function starts here and checks if the correct number of command-line arguments is provided. If not, it exits with an error message.

    int sockfd = max = socket(AF_INET, SOCK_STREAM, 0);
Creates a socket (IPv4, TCP) and assigns its file descriptor to sockfd. Also, initializes max with the sockfd. This socket will be used for listening to incoming connections.

    if (sockfd == -1) exitError("Fatal error\n");
Checks if the socket creation was successful, and if not, exits with a fatal error message.

    FD_ZERO(&active_fds);
    FD_SET(sockfd, &active_fds);
Initializes the active_fds set and adds the server socket to it.

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
    servaddr.sin_port = htons(atoi(av[1]));
Sets up server address information, including the IP address (127.0.0.1) and the port number specified as a command-line argument.

    if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) exitError("Fatal error\n");
Binds the socket to the specified address. If binding fails, it exits with a fatal error message.

    if (listen(sockfd, 10) != 0) exitError("Fatal error\n");
Listens on the socket, allowing up to 10 pending connections. If listening fails, it exits with a fatal error message.

   while (1) {
Starts an infinite loop to handle incoming connections and client interactions.


        Read_fds = Write_fds = active_fds;
Copies the active_fds set to Read_fds and Write_fds sets, so we can track both read and write events on active file descriptors.


        if (select(max + 1, &Read_fds, &Write_fds, NULL, NULL) < 0) continue;
Uses the select function to monitor file descriptors for any activity. If select returns a negative value, it continues the loop.


        for (int fd = 0; fd <= max; fd++) {
Iterates through file descriptors from 0 to the maximum descriptor.


            if (FD_ISSET(fd, &Read_fds) && fd == sockfd) {
Checks if the current file descriptor is in the Read_fds set and is equal to the server socket. This condition is true when a new client is trying to connect.


                int ClientSocket = accept(sockfd, NULL, NULL);
Accepts the incoming connection and assigns the client's socket descriptor to ClientSocket.


                max = (ClientSocket > max) ? ClientSocket : max;
Updates max to the new maximum file descriptor if ClientSocket is greater.


                clients[ClientSocket].id = next_id++;
Assigns a unique ID to the client and increments next_id for the next client.


                bzero(clients[ClientSocket].msg, strlen(clients[ClientSocket].msg));
Clears the client's message buffer to ensure it's empty.


                FD_SET(ClientSocket, &active_fds);
Adds the client's socket to the active_fds set.


                sprintf(buffWrite, "server: client %d just arrived\n", clients[ClientSocket].id);
Formats a welcome message indicating that a new client has arrived and stores it in buffWrite.


                sendMsg(ClientSocket);
Sends the welcome message to all clients.


                break;
Breaks the loop to continue checking other file descriptors.


            if (FD_ISSET(fd, &Read_fds) && fd != sockfd) {
Checks if the current file descriptor is in the Read_fds set and is not the server socket. This condition is true when an existing client is sending a message.


                int read = recv(fd, buffRead, sizeof(buffRead), 0);
Receives a message from the client and stores it in buffRead.


                if (read <= 0) {
Checks if the received message length is less than or equal to 0, indicating that the client has disconnected.


                    sprintf(buffWrite, "server: client %d just left\n", clients[fd].id);
Formats a message indicating that the client has left and stores it in buffWrite.


                    sendMsg(fd);
Sends the leaving message to all clients.


                    FD_CLR(fd, &active_fds);
Removes the client's socket from the active_fds set.

                    close(fd);
Closes the client's socket.


                    break;
Breaks the loop to continue checking other file descriptors.


                } else {
If the received message length is greater than 0, it means the client sent a message.


                    for (int i = 0, j = strlen(clients[fd].msg); i < read; i++, j++) {
Iterates through the received message and copies it character by character into the client's message buffer.


                        clients[fd].msg[j] = buffRead[i];
Copies the current character into the message buffer.


                        if (clients[fd].msg[j] == '\n') {
Checks if the current character is a newline character, indicating the end of a message.


                            clients[fd].msg[j] = '\0';
Replaces the newline character with a null terminator to mark the end of the message.


                            sprintf(buffWrite, "client %d: %s\n", clients[fd].id, clients[fd].msg);
Formats a message with the client's ID and the received message and stores it in buffWrite.


                            sendMsg(fd);
Sends the message to all clients.


                            bzero(clients[fd].msg, strlen(clients[fd].msg));
Clears the client's message buffer to prepare for the next message.


                            j = -1;
Resets the index j to -1, so the next character will be copied to the start of the message buffer.


                    }

                    break;
Breaks the loop to continue checking other file descriptors.


                }

            }
End of the loop that iterates through file descriptors.


        }
End of the loop that handles file descriptor events.

The program continues to listen for incoming connections and process client messages in an infinite loop until it is terminated externally. Clients can connect, send messages, and disconnect, and the server relays messages to all connected clients.
