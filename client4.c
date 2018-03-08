
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include  <signal.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;


void  SIGQUIT_handler(int sig)
{
  printf("SIGQUIT PRIMIT");
  exit(10);
}
int main (int argc, char *argv[])
{
  int sd;     // descriptorul de socket
  struct sockaddr_in server;  // structura folosita pentru conectare 
  char msg[100];    // mesajul trimis

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
  {
    printf ("[client] Sintaxa: %s <adresa_server> <port>\n", argv[0]);
    return -1;
  }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror ("[client] Eroare la socket().\n");
    return errno;
  }
  

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  {
    perror ("[client]Eroare la connect().\n");
    return errno;
  }

  /////////////* citirea mesajului */
  char nume[100];
  bzero (nume, 100);
  printf ("[client]Introduceti un nume: ");
  fflush (stdout);
  read (0, nume, 100);
  nume[strlen(nume)-1]='\0';
///
  if (write (sd, nume, 100) <= 0)
  {
    perror ("[client]Eroare la write() spre server.\n");
    return errno;
  }

  
  char destinatar[100];
  bzero (destinatar, 100);
  

// introducerea destinatarului

  printf ("[client]Introduceti un destinatar: ");
  fflush (stdout);
  read (0, destinatar, 100);
  destinatar[strlen(destinatar)-1]='\0';
  if (write (sd, destinatar, 100) <= 0)
  {
    perror ("[client]Eroare la write() spre server.\n");
    return errno;
  }

  
  int fiu;
  if(fiu=fork())
  {
    printf("\nIncepeti discutia cu user-ul:%s\n",destinatar);
    while((strstr(msg,"/quit\n")==NULL))
      {  char aux[100];
        char mesaj[100];
        bzero(mesaj,100);
        bzero (msg, 100);
        fflush (stdout);
        read (0, msg, 100);
        strcat(mesaj,nume);
        strcat(mesaj,":");
        strcat(mesaj,msg);
  /* trimiterea mesajului la server */
        if (write (sd, mesaj, 100) <= 0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        }
        if(strstr(mesaj,"/quit\n")!=NULL)
        {
      kill(fiu, SIGQUIT);
          exit(1);
        }
      }
      close(sd);
      exit(1);
    }
    else
    {

      while(1)
      {
  /* citirea raspunsului dat de server 
     (apel blocant pina cind serverul raspunde) */
        if (read (sd, msg, 100) < 0)
        {

          perror ("[client]Eroare la read() de la server.\n");
          return errno;

        }

  /* afisam mesajul primit */
        printf ("%s", msg);
      }
      printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<\nn");
      if (write (sd,"/quit\n", 100) <= 0)
      {
        perror ("[client]Eroare la write() spre server.\n");
        return errno;
      }
      close(sd);
      exit(1);


    }
  /* inchidem conexiunea, am terminat */
    close (sd);
  }

