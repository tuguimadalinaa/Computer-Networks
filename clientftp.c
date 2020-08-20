#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include<string.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>
extern int errno;
int port =2024;
void separateData(char msg[], char** file, char** command)
{
	*command=strtok(msg," ");
  char* token = strtok(NULL,"~");
  strcpy(*file,token);
}
void getAllFoldersData(char path[],int client)
{
	DIR* dir;
	char name[256];
	struct dirent* director;
	int whereToStop=0;
  while((dir=opendir(path))!=NULL)
  {	
    while((director=readdir(dir))!=NULL)
      {
        if(strcmp(director->d_name,".")!=0 && strcmp(director->d_name,"..")!=0 )
        {
          strcpy(name,path);
          strcat(name,"/");
          strcat(name,director->d_name);
          if (write (client, name, 256) <= 0)
          {
            perror ("[server]Eroare la write() catre client.\n");
            //continue;		
          }
          printf("Se scrie: %s\n",name);
          getAllFoldersData(name,client);
        }
      }
  }
}
void getFile(char** download,char* file,int sd,int** type,char FolderToUpdate[])
{
    struct stat path_stat;
    stat(file,&path_stat);
    if (S_ISDIR(path_stat.st_mode) == 0)
    {
      *type=2;
      FILE* fd;
      fd = fopen(file,"rb");
      if(fd==NULL)
      {
        printf("Can't open file!\n");
        return;
      }
      long int  size = path_stat.st_size;
      int chunck =0;
      fread(*download,size,1,fd);
      fclose(fd);
    }
    else
    {
      char send_mess[256]="uploadFolder~";
      strcat(send_mess,FolderToUpdate);
      strcat(send_mess,"~");
      strcat(send_mess,file);
      strcat(send_mess,"~");
      if (write (sd, send_mess,256) <= 0)
      {
        perror ("[server]Eroare la write() catre client.\n");
        //continue;		
      }
      getAllFoldersData(file,sd);
      char name[100];
      strcpy(name,"DoneTransfering");
      if (write (sd, name, 100) <= 0)
      {
        perror ("[server]Eroare la write() catre client.\n");
        //continue;		
      }
		  printf("S-a trimis: %s\n",name);
  }
    
}
void getInfo(char** fileData,int** size,char* info)
{
  *fileData = strtok(info,"~");
  char* token = strtok(NULL,"~");
  *size = atoi(token);
}
void downloadFile(char path[], char fileInfo[], int type,int sd,char* fileToGet)
{
  chdir(path);
  if (read (sd, fileInfo, 9999) < 0)
  {
    perror ("[client]Eroare la read() de la server.\n");
    return errno;
  }
  if(strcmp(fileInfo,"File/Folder does not exist!~~")==0)
  {
    printf("File/Folder does not exist!\n");
    return;
  }
  else if(type==1)
  {
    printf("Downloading..\n");
    struct stat path_status;
    int* size = NULL;
    char* fileData=(char*)malloc(9999);
    getInfo(&fileData,&size,fileInfo);
    FILE* fd;
    fd = fopen(path,"wb");
    if(fd==NULL)
    {
      printf("Can't open file!\n");
      return;
    }
    fwrite(fileData,size,1,fd);
    fclose(fd);
    printf("Download of a file complete!\n");
  }
  else if(type==2)
  {
    if(strcmp(fileToGet,"root")!=0)
    {
      //
      char aux_fileToGet[256];
      strcpy(aux_fileToGet,fileToGet);
      strcat(aux_fileToGet,"/");
      int j=0;
      char* token = strtok(aux_fileToGet,"/");
      while(token)
      {
          mkdir(token,0777);
          chdir(token);
          token = strtok(NULL,"/");
          j++;
      }
      while(j)
      {
        chdir("..");
        j--;
      }
      //
    }
    printf("Downloading..\n");
    while(strcmp(fileInfo,"DoneTransfering")!=0)
    {
      printf("File %s\n",fileInfo);
      if(strstr(fileInfo,".")==0)
      {
          mkdir(fileInfo,0777);
      }
      else
      {
        FILE* fd;
        fd=fopen(fileInfo,"w");
        fclose(fd);
      }
      if (read (sd, fileInfo, 9999) < 0)
      {
        perror ("[client]Eroare la read() de la server.\n");
        break;
      }
    }
    printf("Download of a directory complete!\n");
    chdir("..");
  } 
}
int main (int argc, char *argv[])
{
  int sd;
  struct sockaddr_in server;	 
  char msg[100];
  char send_mess[1024];
  char recv_mess[9999];
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = htons (port);
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  {
    perror ("[client]Eroare la connect().\n");
    return errno;
  }
  bool notConnected=true;
  int isExit = 0;
  while(isExit!=1)
  {
    bzero (msg, 100);
    char path[100];
    //login function
    if(notConnected==true)
    {
      while(notConnected==true)
      {
        printf ("Autentification \n");
        printf("Username:   ");
        fflush (stdout);
        gets(msg);
        if(strcmp(msg,"exit")==0)
        {
          printf("bye\n");
          return 1;
        }
        strcat(msg,"~");
        strcpy(send_mess,msg);
        printf("\n");
        fflush (stdout);
        bzero (msg, 100);
        char* password = getpass("Password:   ");
        strcpy(msg,password);
        free(password);
        strcat(msg,"~");
        strcat(send_mess,msg);
        if (write (sd, send_mess, 100) <= 0)
          {
            perror ("[client]Eroare la write() spre server.\n");
            return errno;
          }
          bzero(recv_mess,9999);
        if (read (sd, recv_mess, 9999) < 0)
          {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
          }
        printf ("%s", recv_mess);
        if(strcmp(recv_mess,"Acces denied. You can't acces this server.\n")!=0 && strcmp(recv_mess,"You have no account\n") !=0)
          {
            notConnected=false;
          }
      }
    }
    else
    {
      printf("\nGive coomand:\n");
      bzero(send_mess,1024);
      gets(send_mess);
      strcat(send_mess,"~");
      //exit function 
      if(strcmp("exit~",send_mess)==0)
      {
        if (write (sd, send_mess, 1024) <= 0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        }
        bzero(recv_mess,9999);
        if (read (sd, recv_mess, 9999) < 0)
        {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
        }
        printf("bye\n");
        isExit=1;
        exit(1);
      }
      //command function
      bool willGetfile=false;
      char* command = (char*)malloc(10);
      char* file=(char*)malloc(48);
      separateData(send_mess,&file,&command);
      if(strcmp(command,"get")==0)
      {
        willGetfile=true;
        printf("You recieved a file!\nWhere to save it?(+ FileName)\n");
        gets(path);
      }
      strcpy(send_mess,command);
      strcat(send_mess,"~"); //probleme la separare
      strcat(send_mess,file);
      strcat(send_mess,"~");
      //UPLOAD
      if(send_mess[0]=='u' && send_mess[1]=='p' && send_mess[2]=='l')
      {
        char* fileData=(char*)malloc(9999);
        int* type=NULL;
        char folderToUpdate[256];
        printf("Where yo upload?\n");
        gets(folderToUpdate);
        getFile(&fileData,file,sd,&type,folderToUpdate);
        if(type==2)
        {
          strcpy(send_mess,command);
          strcat(send_mess,folderToUpdate);
          strcat(send_mess,"~");
          strcat(send_mess,fileData);
          if (write (sd, send_mess, 1024) <= 0)
          {
            perror ("[client]Eroare la write() spre server.\n");
            return errno;
          }
        }
      }
      else
      {
        if (write (sd, send_mess, 1024) <= 0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        }
      }
      bzero(send_mess,1024);
      bzero(recv_mess,9999);
      if(willGetfile==true)
      {
          if(strstr(file,"."))
          {
              downloadFile(path,recv_mess,1,sd,file); //1 pentru fisier normal
          }
          else
          {
              downloadFile(path,recv_mess,2,sd,file); //2 pentru director
          }
      }
      else
      {
        if (read (sd, recv_mess, 9999) < 0)
        {
            perror ("[client]Eroare la read() de la server.\n");
            close(sd);
            exit(1);
        }
        printf("%s\n",recv_mess);        
      }
      
    }

  }
  close (sd);
}