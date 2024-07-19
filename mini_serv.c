#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define MAX_MSG_SIZE 1000000
#define MAX_CLIENTS 1024

typedef struct s_client
{
	int	id;
	char	msg[MAX_MSG_SIZE];
}t_client;

t_client clients[MAX_CLIENTS];

int current_id = 0;
int maxfd = 0;

fd_set read_set;
fd_set write_set;
fd_set current;

char	send_buffer[MAX_MSG_SIZE + 20];
char	recv_buffer[MAX_MSG_SIZE + 20];

void putstr(int fd, char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		write(fd, &str[i], 1);
		i++;
	}
}

void err(char *msg)
{
	if (!msg)
		putstr(2, "Fatal error\n");
	else
		putstr(2, msg);
	exit(1);
}

void	send_broadcast(int accepted)
{
<<<<<<< HEAD
	for (int fd = 0; fd <= maxfd; fd++)
	{
		if (FD_ISSET(fd, &write_set) && fd != accepted)
			if (send(fd, send_buffer, strlen(send_buffer), 0) == -1)
				err(NULL);
	}
}

int main(int ac, char **av)
{
	if (ac != 2)
		err("Wrong number of arguments\n");

	int sockfd, connfd;
	struct sockaddr_in servaddr; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1)
	       err(NULL);	
	bzero(&servaddr, sizeof(servaddr)); 
	FD_ZERO(&current);
	FD_ZERO(&write_set);
	FD_ZERO(&read_set);
	FD_SET(sockfd, &current);
	maxfd = sockfd;
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(av[1])); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		err(NULL);
	if (listen(sockfd, 10) != 0)
		err(NULL);
	while (1)
	{
		read_set = write_set = current;
		if (select(maxfd + 1, &read_set, &write_set, 0, 0) == -1)
			continue ;
		for (int fd = 0; fd <= maxfd; fd++)
		{
			if (FD_ISSET(fd, &read_set))
			{
				if (fd == sockfd)
				{
				
					struct sockaddr_in cliaddr;
					socklen_t len = sizeof(cliaddr);
					connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
					if (connfd < 0)
						err(NULL);
					if (maxfd < connfd)
						maxfd = connfd;
					FD_SET(connfd, &current);
					clients[connfd].id = current_id;
					current_id++;
					sprintf(send_buffer, "server: client %d just arrived\n", clients[connfd].id);
					send_broadcast(connfd);

				}
				else
				{
					int ac = recv(fd, recv_buffer, sizeof(recv_buffer), 0);
					if (ac <= 0)
					{
						sprintf(send_buffer, "server: client %d just left\n", clients[fd].id);
						send_broadcast(fd);
						FD_CLR(fd, &current);
						close(fd);
					}
					else
					{
						for (int i = 0, j = strlen(clients[fd].msg); i < ac; i++, j++)
						{
							clients[fd].msg[j] = recv_buffer[i];
							if (clients[fd].msg[j] == '\n')
							{
								clients[fd].msg[j] = '\0';
								sprintf(send_buffer, "client %d: %s\n", clients[fd].id, clients[fd].msg);
								send_broadcast(fd);
								bzero(clients[fd].msg, strlen(send_buffer));
								j = -1;
							}
						}
					}
				}
			}
		}
	}
=======
    if (ac != 2)
        err("Wrong number of arguments");

    bzero(clients, sizeof(clients));
    FD_ZERO(&current);

    int serverfd;
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        err(NULL);
    setsockopt(serverfd, SO_REUSEPORT, 0, 0 , 0);

    maxfd = serverfd;
    FD_SET(serverfd, &current);

    struct sockaddr_in  serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(av[1]));

    if (bind(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
         err(NULL);
    if (listen(serverfd, BACK_LOG) == -1)
        err(NULL);

    while (1)
    {
        read_set = write_set = current;
        if (select(maxfd + 1, &read_set, &write_set, NULL, NULL) == -1)
			continue;

        for (int fd = 0; fd <= maxfd; fd++)
        {
            if (FD_ISSET(fd, &read_set))
            {
                if (fd == serverfd)
                {
                    socklen_t   len;
                    int clientfd = accept(serverfd, (struct sockaddr *)&serveraddr, &len);
                    if (clientfd == -1)
						continue;
                    if (clientfd > maxfd)
						maxfd = clientfd;
                    clients[clientfd].id = curr_id++;
                    FD_SET(clientfd, &current);
                    sprintf(send_buffer, "server: client %d just arrived\n", clients[clientfd].id);
                    broadcast_send(clientfd);
                }
                else
                {
                    int ret; 
                    if ((ret = recv(fd, recv_buffer, sizeof(recv_buffer), 0)) <= 0)
                    {
                        sprintf(send_buffer, "server: client %d just left\n", clients[fd].id);
                        broadcast_send(fd);
                        FD_CLR(fd, &current);
                        close(fd);
                    }
                    else
                    {
                      for (int i = 0, j = strlen(clients[fd].msg); i < ret; i++, j++)
                      {
                            clients[fd].msg[j] = recv_buffer[i];
                            if (clients[fd].msg[j] == '\n')
                            {
                                clients[fd].msg[j] = '\0';
                                sprintf(send_buffer, "client %d: %s\n", clients[fd].id, clients[fd].msg);
                                broadcast_send(fd);
                                bzero(clients[fd].msg, sizeof(clients[fd].msg));
                                j = -1;
                            }
                        }
                    }
                }
            }
        }
    }
    return (0);
>>>>>>> 8545165 (Passed the exam)
}
