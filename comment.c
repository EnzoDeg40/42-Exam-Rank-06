#include <unistd.h>  // Pour close(), write()
#include <stdlib.h>  // Pour exit(), atoi()
#include <netinet/in.h>  // Pour struct sockaddr_in
#include <string.h>  // Pour bzero(), strlen()
#include <sys/select.h>  // Pour select()
#include <stdio.h>  // Pour sprintf(), printf()
#include <arpa/inet.h>  // Pour htonl(), htons(), inet_ntoa()

#define MAX_MSG_SIZE 1000000  // Taille maximale des messages
#define MAX_CLIENTS 1024  // Nombre maximum de clients

typedef struct s_client
{
    int id;  // ID du client
    char msg[MAX_MSG_SIZE];  // Buffer de message du client
} t_client;

t_client clients[MAX_CLIENTS];  // Tableau de clients

int current_id = 0;  // ID courant des clients
int maxfd = 0;  // Descripteur de fichier maximum

fd_set read_set;  // Ensemble des fichiers en lecture
fd_set write_set;  // Ensemble des fichiers en écriture
fd_set current;  // Ensemble courant des fichiers

char send_buffer[MAX_MSG_SIZE];  // Buffer pour les messages envoyés
char recv_buffer[MAX_MSG_SIZE];  // Buffer pour les messages reçus

// Fonction pour afficher une chaîne sur un fd donné
void putstr(int fd, char *str)
{
    write(fd, str, strlen(str));
}

// Fonction pour afficher une erreur et quitter
void err(char *str)
{
    if (!str)
        putstr(2, "Fatal error\n");
    else
        putstr(2, str);
    exit(1);
}

// Fonction pour envoyer un message à tous les clients sauf à l'expéditeur
void send_broadcast(int accepted)
{
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

    // Création du socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        err(NULL);
    
    bzero(&servaddr, sizeof(servaddr));  // Initialisation de la structure d'adresse

    FD_ZERO(&current);  // Initialisation de l'ensemble des descripteurs
    FD_SET(sockfd, &current);  // Ajout du socket principal

    servaddr.sin_family = AF_INET;  // Famille d'adresse IPv4
    servaddr.sin_addr.s_addr = htonl(2130706433);  // Adresse 127.0.0.1
    servaddr.sin_port = htons(atoi(av[1]));  // Port fourni en argument

    // Liaison du socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
        err(NULL);

    // Passage en mode écoute
    if (listen(sockfd, 10) != 0)
        err(NULL);

    while (1)
    {
        read_set = write_set = current;  // Copie de l'ensemble actuel
        
        // Sélection des fichiers prêts
        if (select(maxfd + 1, &read_set, &write_set, NULL, NULL) == -1)
            err(NULL);
        
        for (int fd = 0; fd <= maxfd; fd++)
        {
            if (FD_ISSET(fd, &read_set))
            {
                if (fd == sockfd)  // Nouveau client
                {
                    struct sockaddr_in cli;
                    bzero(&cli, sizeof(cli));
                    socklen_t len = sizeof(cli);
                    connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
                    
                    if (connfd < 0)
                        err(NULL);
                    if (connfd > maxfd)
                        maxfd = connfd;

                    clients[connfd].id = current_id;  // Attribution d'un ID
                    current_id++;
                    FD_SET(connfd, &current);  // Ajout du client à l'ensemble
                    sprintf(send_buffer, "server: client %d just arrived\n", clients[connfd].id);
                    send_broadcast(connfd);
                }
                else  // Client existant envoie un message
                {
                    int ret = recv(fd, recv_buffer, MAX_MSG_SIZE, 0);
                    if (ret <= 0)  // Déconnexion
                    {
                        sprintf(send_buffer, "server: client %d just left\n", clients[fd]);
                        send_broadcast(fd);
                        FD_CLR(fd, &current);
                        close(fd);
                    }
                    else  // Réception de message
                    {
                        for (int i = 0, j = strlen(clients[fd].msg); i < ret; i++, j++)
                        {
                            clients[fd].msg[j] = recv_buffer[i];
                            if (clients[fd].msg[j] == '\n')  // Fin de message
                            {
                                clients[fd].msg[j] = '\0';
                                sprintf(send_buffer, "client %d: %s\n", clients[fd].id, clients[fd].msg);
                                send_broadcast(fd);
                                bzero(clients[fd].msg, strlen(clients[fd].msg));
                                j = -1;
                            }
                        }
                    }
                }
            }
        }
    }
}
