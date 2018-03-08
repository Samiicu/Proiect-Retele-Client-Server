

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

/* portul folosit */

#define PORT 2728

extern int errno;   /* eroarea returnata de unele apeluri */
/* functie de convertire a adresei IP a clientului in sir de caractere */
char * conv_addr (struct sockaddr_in address)
{
  static char str[25];
  char port[7];

  /* adresa IP a clientului */
  strcpy (str, inet_ntoa (address.sin_addr)); 
  /* portul utilizat de client */
  bzero (port, 7);
  sprintf (port, ":%d", ntohs (address.sin_port));  
  strcat (str, port);
  return (str);
}


/* programul */
int main ()
{
  struct sockaddr_in server;  /* structurile pentru server si clienti */
  struct sockaddr_in from;
  fd_set readfds;   /* multimea descriptorilor de citire */
  fd_set actfds;    /* multimea descriptorilor activi */
  struct timeval tv;    /* structura de timp pentru select() */
  int sd, client;   /* descriptori de socket */
  int optval=1;       /* optiune folosita pentru setsockopt()*/ 
  int fd;     /* descriptor folosit pentru 
           parcurgerea listelor de descriptori */
  int nfds;     /* numarul maxim de descriptori */
  int len;
       /* lungimea structurii sockaddr_in */

  /* creare socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror ("[server] Eroare la socket().\n");
    return errno;
  }

  /*setam pentru socket optiunea SO_REUSEADDR */ 
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

  /* pregatim structurile de date */
  bzero (&server, sizeof (server));

  /* umplem structura folosita de server */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (PORT);

  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
  {
    perror ("[server] Eroare la bind().\n");
    return errno;
  }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
  {
    perror ("[server] Eroare la listen().\n");
    return errno;
  }
  
  /* completam multimea de descriptori de citire */
  FD_ZERO (&actfds);    /* initial, multimea este vida */
  FD_SET (sd, &actfds);   /* includem in multime socketul creat */

  tv.tv_sec = 1;    /* se va astepta un timp de 1 sec. */
  tv.tv_usec = 0;
  
  /* valoarea maxima a descriptorilor folositi */
  nfds = sd;

  printf ("[server] Asteptam la portul %d...\n", PORT);
  fflush (stdout);
  
  static int okx=0;
  
  int bytes;
  char useri[10][100];

  int log[10];
  int dest[10][100];

  int flag_logat;
  int flag_destinatar=0;
  int it=0;
  char utilizator[100];
  for(int i=0;i<=10;i++)
    bzero(useri[i],100);
  for(int i=0;i<=10;i++)
    bzero(dest[i],100);

  /* servim in mod concurent clientii... */
  while (1)
  {  


    char destinatar[100];

      /* ajustam multimea descriptorilor activi (efectiv utilizati) */
    bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

      /* apelul select() */
    if (select (nfds+1, &readfds, NULL, NULL, &tv) < 0)
    {
      perror ("[server] Eroare la select().\n");
      return errno;
    }
      /* vedem daca e pregatit socketul pentru a-i accepta pe clienti */
    if (FD_ISSET (sd, &readfds))
    {
    /* pregatirea structurii client */
      len = sizeof (from);
      bzero (&from, sizeof (from));

    /* a venit un client, acceptam conexiunea */
      client = accept (sd, (struct sockaddr *) &from, &len);


    /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
      {
        perror ("[server] Eroare la accept().\n");
        continue;
      }

          if (nfds < client) /* ajusteaza valoarea maximului */
      nfds = client;

    /* includem in lista de descriptori activi si acest socket */
      FD_SET (client, &actfds);

      printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n",client, conv_addr (from));
      fflush (stdout);
    }
    static int ok=0;
    flag_logat=0;
    flag_destinatar=0;
      /* vedem daca e pregatit vreun socket client pentru a trimite raspunsul */
      for (fd = 0; fd <= nfds; fd++)  /* parcurgem multimea de descriptori */
    {   
      int ffd;
      int poz_it;
      char aux[100];
      int gasit=0;
      FILE *ff;
      FILE *gg;

    /* este un socket de citire pregatit? */
      if (fd != sd && FD_ISSET (fd, &readfds))
      { 

        
        for(int l=0;l<it;l++)
        {
          if(fd==log[l])
          {
            bzero(utilizator,100);
            strcpy(utilizator,useri[l]);
            bzero(destinatar,100);
            strcpy(destinatar,dest[l]);
            flag_logat=1;
            flag_destinatar=1;
            poz_it=l;
          }
        } 
    
        if(flag_logat==0)
        {
          bytes = read (fd, utilizator, 100);
          if (bytes <= 0)
          {
            perror ("{INT MAIN}[1]Eroare la read() de la client.\n");
            return errno;
          }

          printf ("[server]Utilizator...%s\n", utilizator);

          ffd=open("utilizatori.txt",O_CREAT,0777);
          close(ffd);
          ff=fopen("utilizatori.txt","r+");
          if (ff == NULL) 
          {
           perror("fopen");
         }
         while(!feof(ff)&& gasit==0)
         {
          if ( fgets (aux , 100 , ff) != NULL )
            aux[strlen(aux)-1]='\0';
          printf("VERIFICAT:|%s|\n",aux);

          if(strcmp(aux,utilizator)==0)
          {
            printf("Te-am gasit frumosule!!!\n");
            gasit=1;
          }
        }
        if(gasit==0)
        {
          char aux_ch[100];
          bzero(aux_ch,100);
          utilizator[strlen(utilizator)]='\0';
          strcpy(aux_ch,utilizator);
          strcat(aux_ch,"\n");
          fputs(aux_ch,ff);
          gasit=1;

        }
        strcat(useri[it],utilizator);
        log[it]=fd;
        it++;

        bytes = read (fd,destinatar, 100);
        if (bytes <= 0)
        {
          perror ("{INT MAIN}[2]Eroare la read() de la client.\n");

        }
        bzero(dest[it-1],100);
        strcpy(dest[it-1],destinatar);

        fclose (ff); 
      }
      
      ffd=open(utilizator,O_CREAT,0777);
      close(ffd);
      gg=fopen(utilizator,"r+");
          if (gg== NULL) 
          {
           perror("fopen");
         }
         while(!feof(gg))
         {
          if ( fgets (aux , 100 , gg) != NULL )
            if (write (log[it-1], aux, 100) < 0)
                {
                  perror ("[server] Eroare la write() catre client.\n");
                  return 0;
                }
        }
        ffd=open(utilizator,O_TRUNC);


      ok++;

      if (sayHello(fd,nfds,sd,utilizator,destinatar,log,it,useri)==0)
      {
        fflush(stdout); 
        printf("\nAICI AJUNGE!\n");
        for(int l=0;l<it;l++)
        {
          if(fd==log[l])
          {
            bzero(useri[l],100);
            bzero(destinatar,100);
            bzero(dest[l],100);
            log[l]=NULL;
          }
        } 
            close (fd);   /* inchidem conexiunea cu clientul */
            FD_CLR (fd, &actfds);/* scoatem si din multime */

      }



    }
  }     /* for */
    }       /* while */
}       /* main */

/*verificare clientului*/


/* realizeaza primirea si retrimiterea unui mesaj unui client */
    int sayHello(int fd,int nfds,int sd,char utilizator[100],char destinatar[100],int log[10],int it,char useri[10][100])
    {


          char buffer[100];   /* mesajul */
          int bytes;      /* numarul de octeti cititi/scrisi */
          char msg[100];    //mesajul primit de la client 
          char msgrasp[100]=" ";        //mesaj de raspuns pentru client

          FILE *ff;
          FILE *gg;
          int ffd;



          bytes = read (fd, msg, sizeof (buffer));
          if (bytes <= 0)
          {
            perror ("Eroare la read() de la client.\n");
            return 0;
          }
          printf ("[server]Mesajul a fost receptionat...%s", msg);

          if(strcmp(msg,"/quit\n")==0)
          { 
            return 0;
          } 
          else
          {



  /*pregatim mesajul de raspuns */
            bzero(msgrasp,100);
            strcat(msgrasp,msg);
          }


          printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
          int fda;   
          int v=0; 
          int on=0; 
          char nume_fisier[200];
          char aux_comparare[100];

          bzero(nume_fisier,200);
          bzero(aux_comparare,100);




          strcat(aux_comparare,utilizator);
          strcat(aux_comparare,":");
          strcat(aux_comparare,"/quit\n");


          bzero(nume_fisier,200);
          strcat(nume_fisier,utilizator);
          strcat(nume_fisier,"|");
          printf("\nNUME%s\n",nume_fisier);
          strcat(nume_fisier,destinatar);

          ffd=open(nume_fisier, O_CREAT ,0777 );
          close(ffd);
          ff=fopen(nume_fisier,"r+");
          if (ff == NULL) 
          {
           perror("fopen");
         }

         while (fgetc(ff) != EOF) {};

         if(strcmp(aux_comparare,msgrasp)!=0)
           fputs(msgrasp,ff) ;

         fclose(ff);
        

          bzero(nume_fisier,200);
          strcat(nume_fisier,destinatar);
          strcat(nume_fisier,"|");
          strcat(nume_fisier,utilizator);

          ffd=open(nume_fisier, O_CREAT ,0777 );
          close(ffd);
          gg=fopen(nume_fisier,"r+");
          if (gg == NULL) 
          {
           perror("fopen");
         }

         while (fgetc(gg) != EOF) {};

         if(strcmp(aux_comparare,msgrasp)!=0)
           fputs(msgrasp,gg) ;

         fclose(gg);




         for(fda=0;fda<=nfds;fda++)
         {
          if ( fda!=fd && fda!=sd )
            for(int k=0;k<it;k++)
            { 
              printf("\nuser|%s|<---==--->|%s|destinatar",useri[k],destinatar);
              if(strcmp(useri[k],destinatar)==0 && log[k]==fda)
              {

                if (bytes && write (log[k], msgrasp, bytes) < 0)
                {
                  perror ("[server] Eroare la write() catre client.\n");
                  return 0;
                }
                printf("\n[%d]AM trimis mesajul:%s!!!\n",v,msgrasp);
                v++;
              }
            }


          }

          if(v==0)
          {  









            printf("\nAUX:%s",aux_comparare);
            bzero(nume_fisier,200);
            strcat(nume_fisier,destinatar);
            printf("\n<<<<<[%s]>>>>>>>>>>>>>\n",nume_fisier);


            ffd=open(nume_fisier, O_CREAT ,0777 );
            close(ffd);

            ff=fopen(nume_fisier,"r+");
            if (ff == NULL) 
            {
             perror("fopen");
           }
           printf("\nMSGGRAP:%s||\n",msgrasp);
           while (fgetc(ff) != EOF) {};

           if(strcmp(aux_comparare,msgrasp)!=0)
             fputs(msgrasp,ff) ;
           else
            fputs("----------------------------------\n",ff);

          fclose(ff);


          if (bytes && write (fd,"[!]Utilizator offline\n", 100) < 0)
          { 

           perror ("[server] Eroare la write() catre client.\n");
           return 0;
         }

       }


       return bytes;
     }