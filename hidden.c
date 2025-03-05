
// Inclusion des bibliothèques nécessaires

// Définition des constantes pour la taille des messages et le nombre maximal de clients

// Définition de la structure client contenant un ID et un buffer de message

// Déclaration des variables globales : tableau de clients, ID actuel, maximum des descripteurs de fichiers

// Déclaration des ensembles de descripteurs pour select()

// Déclaration des buffers pour l'envoi et la réception de messages

// Fonction pour écrire une chaîne sur un descripteur donné

// Fonction pour afficher un message d'erreur et quitter le programme

// Fonction pour envoyer un message à tous les clients sauf à l'expéditeur

// Fonction principale

// Vérification du nombre d'arguments

// Initiation des variables

// Création du socket

// Initialisation de la structure d'adresse

// Initialisation des ensembles de descripteurs

// Configuration de l'adresse du serveur (127.0.0.1 et port fourni en argument)

// Liaison du socket au port

// Mise en écoute des connexions

// Boucle infinie pour gérer les connexions et les messages des clients

// Copie des ensembles de descripteurs pour select()

// Attente d'un événement sur les sockets avec select()

// Parcours des descripteurs actifs

// Vérification si un nouveau client tente de se connecter

// Acceptation de la connexion et stockage du client

// Attribution d'un ID au client

// Ajout du client à l'ensemble des descripteurs

// Envoi d'un message de bienvenue aux autres clients

// Vérification si un client envoie un message

// Réception des données du client

// Vérification si le client s'est déconnecté

// Notification aux autres clients du départ

// Suppression du client des ensembles et fermeture du socket

// Traitement des messages reçus et envoi aux autres clients

// Vérification des retours à la ligne pour détecter la fin d'un message

// Nettoyage du buffer du client après l'envoi du message
