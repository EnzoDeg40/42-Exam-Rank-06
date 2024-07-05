# include <unistd.h>
# include <stdio.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <string.h>
# include <stdlib.h>
# include <sys/types.h>
# include <netinet/in.h>

# define MAX_CLIENTS 1024
# define MAX_MSG_SIZE 120000
# define BACK_LOG 100

typedef struct s_client
{
    int     id;
    char    msg[MAX_MSG_SIZE];
}   t_client;

t_client    clients[MAX_CLIENTS];
fd_set      read_set;
fd_set      write_set;
fd_set      current;

char        send_buffer[MAX_MSG_SIZE];
char        recv_buffer[MAX_MSG_SIZE];

int         maxfd = 0;
int         curr_id = 0;

void    err(char  *msg)
{
    if (msg)
        write(2, msg, strlen(msg));
    else
        write(2, "Fatal error", 11);
    write(2, "\n", 1);
    exit(1);
}

void    broadcast_send(int except)
{
    for (int fd = 0; fd <= maxfd; fd++)
    {
        if  (FD_ISSET(fd, &write_set) && fd != except)
            if (send(fd, send_buffer, strlen(send_buffer), 0) == -1)
                err(NULL);
    }
}

int     main(int ac, char **av)
{
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
                        strcpy(clients[fd].msg, recv_buffer);
                        char *found;
                        if ((found = strstr(clients[fd].msg, "\n")) != NULL)
                        {
                            *found = '\0';
                            sprintf(send_buffer, "client %d: %s\n", clients[fd].id, clients[fd].msg);
                            broadcast_send(fd);
                            bzero(clients[fd].msg, strlen(clients[fd].msg));
                        }
                    }
                }
                break;
            }
        }
    }
    return (0);
}
