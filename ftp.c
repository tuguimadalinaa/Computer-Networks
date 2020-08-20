#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdio.h>
#include<ftw.h>
#define PORT 2024
extern int errno;
char* getFile(char* file,char* username,int client);
char* commands[8] ={"mkdir","rmdir","delete","nano","uploadFolder","uploadFile","get"};
void getData(char msg[], char** password, char** username)
{
	*username = strtok(msg,"~");
	*password = strtok(NULL,"~");
}
bool check(char* password, char* username, char* file)
{
	char path[100];
	strcpy(path,"ClientiServer/AccesAutentificare/");
	strcat(path,file);
	FILE* fd = fopen(path,"r");
	int read;
	char* us=(char*)malloc(45);
	char* pas=(char*)malloc(30);
	int len_us=0;
	int len_pas=0;
	if(fd<0)
	{
		printf("Eroare la deschidere fisier");
	}
	while((read=getline(&us,&len_us,fd))!=-1)
	{
		read= getline(&pas,&len_pas,fd);
		pas[strlen(pas)-1]='\0';
		us[strlen(us)-1]='\0';
		if(strcmp(password,pas)==0 && strcmp(username,us)==0)
			{
				return true;
			}
	}
	return false;
}
void separateData(char msg[], char** file, char** command,char** fileSend)
{
	*command=strtok(msg,"~");
	*file = strtok(NULL,"~");
	if(strcmp(*command,"uploadFolder")==0)
	{
		*fileSend=strtok(NULL,"~");
	}
	else
	{
		*fileSend="";
	}
	
}
bool checkCommand(char* command)
{
	for(int i=0;i<7;i++)
	{
		if(strcmp(command,commands[i]))
			return true;
	}
	return false;
}
int getAllPaths(char*** all_Paths,char* file)
{
	char aux[60];
	strcpy(aux,file);
	int i=0;
	char* token = strtok(aux, "/~");
	do
	{
		strcpy(all_Paths[i],token);
		i++;
		token=strtok(NULL,"/~");
	}while(token);
	return i;
}
char* makeFolder(char* file,char username[])
{
	DIR* dir;
	struct dirent* director;
	char path[100] ="ClientiServer/";
	strcat(path,username);
	int i=0;
	int j=0;
	char* allPaths[10];
	for(int j=0;j<10;j++)
	{
		allPaths[j] = (char*)malloc(10);
	}
	j= getAllPaths(&allPaths,file);
	int whereToStop=0;
	int auxWhereToStop=whereToStop+1;
	while(whereToStop!=auxWhereToStop)
	{
		auxWhereToStop=whereToStop;
		if((dir=opendir(path))!=NULL)
		{
			bool notFound=true;
			while((director=readdir(dir))!=NULL && notFound==true)
			{
				if(strcmp(director->d_name,".")!=0 && strcmp(director->d_name,"..")!=0 )
				{
					{
						if(strcmp(director->d_name,allPaths[whereToStop])==0)
						{
							whereToStop++;
							strcat(path,"/");
							strcat(path,director->d_name);
							notFound=false;
						}
					}
				}
			}
		}
	}
	if(whereToStop==j)
	{
		char* response = "Folder already exists!\n";
		return response;
	}
	else
	{
		chdir(path);
		printf("Pathul din else: %s\n",path);
		for(int i=whereToStop;i<j;i++)
		{
			mkdir(allPaths[i],0777);
			chdir(allPaths[i]);
		}
		for(int i=whereToStop;i<j+whereToStop;i++) 
		{
			chdir("..");
		}
		chdir("..");
		chdir("..");
		char cwd[100];
		getcwd(cwd,100);
		printf("Dupa sunt in: %s\n",cwd);
		char* response = "Folder created successfully\n";
		return response;

	}
	
}
void deleteRecursively(char* path, char username[])
{
	int path_len;
	DIR* dir;
	struct dirent* director;
	struct stat stat_path, stat_director;
	stat(path, &stat_path);
	char* full_path; 
	if (S_ISDIR(stat_path.st_mode) == 0) {
		printf("Not a directory!\n");
		exit(-1);
	}
	if ((dir = opendir(path)) == NULL) {
		//return "Can't open directory!";
		printf("Directory doesn't exist");
		exit(-1);
	}
	path_len = strlen(path);
	while ((director = readdir(dir)) != NULL) {
		if (!strcmp(director->d_name, ".") || !strcmp(director->d_name, ".."))
			continue;
		full_path = malloc(100);
		strcpy(full_path, path);
		strcat(full_path, "/");
		strcat(full_path, director->d_name);
		stat(full_path, &stat_director);

		if (S_ISDIR(stat_director.st_mode) != 0) {
			deleteRecursively(full_path,username);
			continue;
		}

		if (unlink(full_path) == 0)
			printf("Removed a file: %s from user %s\n", full_path,username);
	}

	if (rmdir(path) == 0)
		printf("Removed a directory: %s from user %s\n", path,username);
	closedir(dir);
}
char* deleteFolder(char* file,char username[])
{
	DIR* dir;
	struct dirent* director;
	char path[100] ="ClientiServer/";
	strcat(path,username);
	int i=0;
	int j=0;
	char* allPaths[10];
	for(int j=0;j<10;j++)
	{
		allPaths[j] = (char*)malloc(10);
	}
	j= getAllPaths(&allPaths,file);
	int whereToStop=0;
	int auxWhereToStop=whereToStop+1;
	while(auxWhereToStop!=whereToStop)
	{
		auxWhereToStop=whereToStop;
		bool notFound=true;
		if((dir=opendir(path))!=NULL && notFound==true)
		{
			while((director=readdir(dir))!=NULL && notFound==true)
			{
				if(strcmp(director->d_name,".")!=0 && strcmp(director->d_name,"..")!=0 )
				{
					{
						if(strcmp(director->d_name,allPaths[whereToStop])==0)
						{
							whereToStop++;
							strcat(path,"/");
							strcat(path,director->d_name);
							notFound=false;
						}
					}
				}
			}
		}
	}
	if((whereToStop==j && whereToStop!=0) || whereToStop==1)
	{
		chdir(path);
		chdir("..");
		deleteRecursively(allPaths[whereToStop-1],username);
		if(whereToStop==1)
		{
			rmdir(allPaths[0]);
		}
		else
		{
			rmdir(allPaths[j]);
		}
		
		for(int i=0;i<whereToStop+1;i++) //-1
		{
			chdir("..");
		}
		char* reponse = "Folder deleted completly\n";
		return reponse;
	}
	else
	{
		char* response = "Path is incorect\n";
		return response;
	}
	
}
void getFolderAndFileData(char** path, char** fileData,char msg[])
{
	char* token = strtok(msg,"~");
	token = strtok(NULL,"~");
	*path = strtok(NULL,"~");
	*fileData = strtok(NULL,"~");
}
char* uploadFile(char* file, char* username, char msg[],int client)
{
	strcat(msg,"~");
	char* command = strtok(msg,"~");
	char* fileName = strtok(NULL,"~");
	char* pathGiven = strtok(NULL,"~");
	char* fileData =strtok(NULL,"~");
	printf("FileData: %s\n",fileData);
	char path[100] ="ClientiServer/";
	strcat(path,username);
	chdir(path);
	if(strcmp(pathGiven,"root")!=0)
		chdir(pathGiven);
	//
	char auxfile[200];
	strcpy(auxfile,fileName);
	char* token = strtok(auxfile,"/~");
	while(token)
	{	
		strcpy(fileName,token);
		token = strtok(NULL,"~/");
	}
	FILE* fd;
	fd = fopen(fileName,"wb");
	fwrite(fileData,strlen(fileData),1,fd);
	fclose(fd);
	chdir("..");
	chdir("..");
	chdir("..");
	chdir("..");
	/*char cwd[100];
	getcwd(cwd,100);
	printf("CWD: %s\n",cwd);*/
	return "Upload successfully\n";
}
void getAllFoldersData(char path[],int client,char* username)
{
	DIR* dir;
	char name[256];
	struct dirent* director;
	int whereToStop=0;
	while((dir=opendir(path))!=NULL)
	{	while((director=readdir(dir))!=NULL)
		{
			if(strcmp(director->d_name,".")!=0 && strcmp(director->d_name,"..")!=0 )
			{
				strcpy(name,path);
				strcat(name,"/");
				strcat(name,director->d_name);
				strcat(name,"~");
				char* token =strtok(name,"/");
				token=strtok(NULL,"/");
				token = strtok(NULL,"~");
				strcpy(name,token);
				if (write (client, name, 9999) <= 0)
				{
					perror ("[server]Eroare la write() catre client.\n");
					//continue;		
				}
				strcpy(name,path);
				strcat(name,"/");
				strcat(name,director->d_name);
				printf("Path: %s\n",name);
				getAllFoldersData(name,client,username);
			}
		}
	}
	closedir(dir);
}
char* getFile(char* file,char* username,int client)
{
	struct stat path_status;
	char*  fileData=(char*)malloc(9999);
	char path[100] = "ClientiServer/";
	strcat(path,username);
	if(strcmp(file,"root")!=0)
		{
			printf("File: %s\n",file);
			printf("Nu e root\n");
			strcat(path,"/");
			strcat(path,file);
		}
	int err = stat(path,&path_status);
	if(err==-1 && ENOENT==errno)
	{
		strcpy(fileData,"File/Folder does not exist!~~");
		return fileData;
	}
	else if (S_ISDIR(path_status.st_mode) == 0)
	{
		FILE* fd;
		fd = fopen(path,"rb");
		if(fd==NULL)
		{
			printf("Can't open file!\n");
			return;
		}
		long int  size = path_status.st_size;
		char c[10];
		char size_in_char[10];
		int i=0; 
		int j=0;
		int aux_size=size;
		while(size)
		{
			c[i]=(size%10) + '0';
			size=size/10;
			i++;
		}
		i--;
		while(i>-1)
		{
			size_in_char[j]=c[i];
			i--;
			j++;
		}
		char* content = (char*)malloc(9999);
		fread(content,aux_size,1,fd);
		strcpy(fileData,content);
		strcat(fileData,"~");
		strcat(fileData,size_in_char);
		strcat(fileData,"~");
		fclose(fd);
		return fileData;
	}
	else if(S_ISDIR(path_status.st_mode) != 0)
	{	
		getAllFoldersData(path,client,username);
		char name[100];
		strcpy(name,"DoneTransfering");
		if (write (client, name, 100) <= 0)
		{
			perror ("[server]Eroare la write() catre client.\n");
			//continue;		
		}
		printf("S-a trimis: %s\n",name);
		strcpy(fileData,"File found!~~");
		return fileData;
	}
}
char* manipulateFiles(bool delete_or_create,char* file,char* username) //delete=0 create1
{
	char* allPaths[10];
	DIR* dir;
	char* response = (char*)malloc(40);
	struct dirent* director;
	char path[100] ="ClientiServer/";
	strcat(path,username);
	chdir(path);
	int countChdir=0;
	for(int j=0;j<10;j++)
	{
		allPaths[j] = (char*)malloc(10);
	}
	int j= getAllPaths(&allPaths,file);
	printf("Number of paths: %d\n",j);
	for(int i=0;i<j-1;i++)
	{
		printf("Path: %s\n",allPaths[i]);
		if((dir=opendir(allPaths[i]))==NULL)
		{
			for(int i=0;i<countChdir;i++)
				chdir("..");
			chdir(".."); chdir("..");
			strcpy(response,"Path is not corect!\n");
			return response;
		}
		else
		{
			chdir(allPaths[i]);
			countChdir++;
			printf("Aici\n");
		}	
	}
	if(delete_or_create==0)
	{
		if(remove(allPaths[j-1])==0)
		{
			for(int i=0;i<countChdir;i++)
				chdir("..");
			countChdir=0;
			chdir(".."); chdir("..");
			strcpy(response,"Deleted successfuly!\n");
			return response;
		}
		else
		{
			for(int i=0;i<countChdir;i++)
				chdir("..");
			countChdir=0;
			chdir(".."); chdir("..");
			strcpy(response,"Delete failed\n");
			return response;
		}
		
	}
	else if(delete_or_create==1)
	{
		FILE* fd;
		fd = fopen(allPaths[j-1],"wb");
		if(fd<0)
		{
			for(int i=0;i<countChdir;i++)
				chdir("..");
			countChdir=0;
			chdir(".."); chdir("..");
			strcpy(response,"File failed to create\n");
			return response;
		}
		chdir(".."); chdir("..");
		strcpy(response,"File created successfully!\n");
		return response;
		
	}

}
char* uploadFolder(char* file,char* username,char aux_msj[],int client,char* fileSend)
{
	char path[256] = "ClientiServer/";
	strcat(path,username);
	chdir(path);
	int j=1;
	int k=1;
	char aux_file[256];
	char aux_file_send[256];
	strcpy(aux_file,file);
	strcat(aux_file,"/");
	strcpy(aux_file_send,fileSend);
	strcat(aux_file_send,"/");
	char* token = strtok(aux_file,"/");
	char cwd[100];
	if(strcmp(token,"root")!=0)
	{
		while(token)
		{
			chdir(token);
			j++;
			token = strtok(NULL,"/");
		}
	}
	token = strtok(aux_file_send,"/");
	char folder_to_create[100];
	while(token)
	{
		strcpy(folder_to_create,token);
		mkdir(folder_to_create,0777);
		k++;
		token = strtok(NULL,"/");
	}
	 //Read,Write,Owner,ExecutePermission,Group and Word
	char fileInfo[256]="";
	while(strcmp(fileInfo,"DoneTransfering")!=0)
	{
		if(strstr(fileInfo,".")==0)
		{
			mkdir(fileInfo,0777);
		}
		else if(strstr(fileInfo,"."))
		{
			FILE* fd;
			fd=fopen(fileInfo,"wb"); //transformat binar
			fclose(fd);
		}
		if (read (client, fileInfo, 256) < 0)
		{
			perror ("[client]Eroare la read() de la server.\n");
			return errno;
		}
		printf("File %s\n",fileInfo);
	}
	while(j)
	{
		chdir("..");
		j--;
	}
	chdir("..");
	return "Upload successfully\n";
}
void process_command(char msg[], char** send_back,char* username,int client)
{
	char aux_msj[9999];
	strcpy(aux_msj,msg);
	char* command=(char*)malloc(10);
	char* file = (char*)malloc(9999);
	char* fileSend = (char*)malloc(25);
	separateData(msg,&file,&command,&fileSend);
	if(checkCommand(command)==true)
	{
		if(strcmp(command,commands[0])==0)
		{
			*send_back = makeFolder(file,username);
		}
		if(strcmp(command,commands[1])==0)
		{
			*send_back = deleteFolder(file,username);
		}
		if(strcmp(command,commands[5])==0)
		{
			printf("Aici\n");
			*send_back = uploadFile(file,username,aux_msj,client);
		}
		if(strcmp(command,commands[6])==0)
		{
			*send_back = getFile(file,username,client);
		}
		if(strcmp(command,commands[3])==0)
		{
			*send_back =manipulateFiles(1,file,username);
		}
		if(strcmp(command,commands[2])==0)
		{
			*send_back = manipulateFiles(0,file,username);
		}
		if(strcmp(command,commands[4])==0)
		{
			*send_back=uploadFolder(file,username,aux_msj,client,fileSend);
		}
	}
	else
	{
		//strcpy(*send_back,"No valid command");
		*send_back="No valid command";
	}
	
}
int main ()
{
    struct sockaddr_in server;	
    struct sockaddr_in from;
    char auttentification[200]; 
	volatile int isAutt=0;
	volatile int isExit=0;    
    int sd;			
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	perror ("[server]Eroare la socket().\n");
    	return errno;
    }
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
    	perror ("[server]Eroare la bind().\n");
    	return errno;
    }

    if (listen (sd, 1) == -1)
    {
    	perror ("[server]Eroare la listen().\n");
    	return errno;
    }
	printf("Server started..\n");
    while (1)
    {
    	int client;
    	int length = sizeof (from);
    	fflush (stdout);
    	client = accept (sd, (struct sockaddr *) &from, &length);

    	if (client < 0)
    	{
    		perror ("[server]Eroare la accept().\n");
    		continue;
    	}

    	int pid;
    	if ((pid = fork()) == -1) {
    		close(client);
    		continue;
    	} 
		else if (pid > 0) {
    		close(client);
    		while(waitpid(-1,NULL,WNOHANG));
    		continue;
    	} 
		else if (pid == 0) 
		{
    		close(sd);
    		fflush (stdout);
			char msg[9999];
			char user[48];
			char* username =(char*)malloc(48);
			char* password = (char*)malloc(15);
			while(isExit==0)
			{
				bzero (msg, 9999);
				if (read (client, msg, 9999) <= 0)
				{
					perror ("[server]Eroare la read() de la client.\n");
					close (client);
						//continue;		
				}
				if(isAutt==0)
				{
					bzero(auttentification,200);
					getData(msg,&password,&username);
					if(check(password,username,"WhiteList.txt")==true)
					{
						strcpy(user,username);
						isAutt=1;
						printf("User %s is online\n",username);
						strcat(auttentification,"Welcome, ");
						strcat(auttentification,username);
						strcat(auttentification,"\n");
						if (write (client, auttentification, 200) <= 0)
						{
							perror ("[server]Eroare la write() catre client.\n");
							//continue;		
						}
					}
					else if(check(password,username,"BlackList.txt")==true)
					{
						printf("Denied acces to user %s\n",username);
						strcat(auttentification,"Acces denied. You can't acces this server.\n");
						if (write (client, auttentification, 1024) <= 0)
						{
							perror ("[server]Eroare la write() catre client.\n");
							//continue;		
						}
					}
					else
					{
						strcat(auttentification,"You have no account\n");
						if (write (client, auttentification, 200) <= 0)
						{
							perror ("[server]Eroare la write() catre client.\n");
							//continue;		
						}
					}
				}
				else if(isAutt==1 && strlen(msg)!=0)
				{
					printf("msg: %s\n",msg);
					printf("len: %ld\n",strlen(msg));
					if(strcmp(msg,"exit~")==0)
					{
						isExit=1;
						printf("User %s left server\n",user);
						char exit[5];
						strcpy(exit,"exit");
						if (write (client, exit, 5) <= 0)
						{
							perror ("[server]Eroare la write() catre client.\n");
							//continue;		
						}
						return;
					}
					else if(isExit==0)
					{
						char* send_back =(char*)malloc(9999);
						process_command(msg,&send_back,user,client);
						printf("Se trimite %s\n",send_back);
						if (write (client, send_back, 9999) <= 0)
						{
							perror ("[server]Eroare la write() catre client.\n");
							//continue;		
						}
					}

				}
			}
			close (client);
    		exit(0);
    	}
    }				
}	