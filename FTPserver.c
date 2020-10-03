/* 
FTPserver.c 
Zhelin Yu, Lab 1
*/ 

//Include the necessary libraries for FTPserver.c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <dirent.h>
#include <fcntl.h>

#define PORT 12345 // Define Port Number
#define BUFFER_SIZE 4096 // Define Buffer Size
#define SERVER_ADDRESS "127.0.0.1" // Define Server Address

int main(){
    int serverSocket, clientSocket;
    int retBind, retListen;
    struct sockaddr_in serverAddress, clientAddress;
    pid_t childPID;
    socklen_t addr_size;

    char commandBuffer[BUFFER_SIZE]; // command buffer from the client
    char dirTracker[BUFFER_SIZE];  //directory buffer on the server

    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr =inet_addr(SERVER_ADDRESS) ;
    
    // create server socket
    serverSocket = socket(AF_INET,SOCK_STREAM,0); //TCP stream socket on IPv4
    if (serverSocket < 0){
        perror("Error in socket creation.\n");
        return -1;
    }
    else{
        printf("Server socket is created.\n");
    }

    // bind server socket with a specific port
    retBind = bind(serverSocket,(struct sockaddr *) &serverAddress,sizeof(serverAddress));
    if (retBind < 0){
       perror("Error in Bind.");
       return -1; 
    } 

    // listen to the potential connection at server socket
    retListen = listen(serverSocket,10);
    if (retListen < 0){
       perror("Error in Listen.");
       return -1; 
    }  
    
    // run the server and accept the connection at server socket 
    while (1){
        clientSocket = accept(serverSocket,(struct sockaddr *) &clientAddress,&addr_size);
        if (clientSocket<0){
           perror("Error in Accept.");
           return -1;  
        }
        else{
            printf("Connection is accepeted from %s:%d\n",inet_ntoa(clientAddress.sin_addr),ntohs(clientAddress.sin_port));
        }

        // create child process to support the multithread connection to the server
        if ((childPID = fork()==0)){
            while(1){
                bzero(commandBuffer,sizeof(commandBuffer));// clear the buffer 
                
                // receive command from the client and invalid command will not be sent by the client
                recv(clientSocket,commandBuffer,sizeof(commandBuffer),0);
                
                // exit or bye command to exit from the client process
                if (!strncmp(commandBuffer,"exit",4)||!strncmp(commandBuffer,"bye",3))
                {
                    printf("Disconnected from  %s:%d\n",inet_ntoa(clientAddress.sin_addr),ntohs(clientAddress.sin_port));
                    close(clientSocket);
                }
                
                // lpwd commend as pwd to display the server's current directory
                else if (!strncmp(commandBuffer,"lpwd",4))
                {
                    getcwd(dirTracker,sizeof(dirTracker));
                    send(clientSocket,dirTracker,sizeof(dirTracker),0);
                }
                
                // lls command as ls to list files at the server's current directory
                else if (!strncmp(commandBuffer,"lls",3))
                {
                   getcwd(dirTracker,sizeof(dirTracker));
                   DIR *dp;
                   char dirList[BUFFER_SIZE];
                   bzero(dirList,sizeof(dirList));
                   struct dirent *entry;
                   dp = opendir(dirTracker);
                   entry = readdir(dp);
                   while(entry!=NULL){
                       strcat(dirList,entry->d_name);
                       strcat(dirList,"\n");
                       entry = readdir(dp);

                   }
                   send(clientSocket,dirList,sizeof(dirList),0);
                   free(dp);
                }
                
                //lcd command as cd to change the server's current directory
                else if (!strncmp(commandBuffer,"lcd",3))
                {
                    char targetDir[BUFFER_SIZE];
                    getcwd(dirTracker,sizeof(dirTracker));
                    strcpy(targetDir,dirTracker);
                    strcat(targetDir,"/");
                    strcat(targetDir,commandBuffer+4);
                    if (chdir(targetDir)!=0){
                        perror("chdir");
                        send(clientSocket,"No such dir",BUFFER_SIZE,0);
                    }

                    getcwd(dirTracker,sizeof(dirTracker));
                    send(clientSocket,"cd success",BUFFER_SIZE,0);
                }
                
                // get command to send the file to the client
                else if (!strncmp(commandBuffer,"get",3))
                {
                    getcwd(dirTracker,sizeof(dirTracker));
                    char fileName[BUFFER_SIZE];
                    strcpy(fileName,commandBuffer+4);

                    FILE *fp;
                    if ((fp = fopen (fileName,"rb"))==NULL){
                       send(clientSocket,"Can not find the file!",BUFFER_SIZE,0); 
                    }
                    else{
                        // scan the whole file
                        fseek(fp, 0, SEEK_END);
                        long fsize = ftell(fp);
                        rewind(fp);  
                        // save the file into a file content buffer
                        char *fileContent = malloc(fsize + 1);
                        fread(fileContent, 1, fsize, fp);
                        send(clientSocket,fileContent,fsize+1,0);
                        fclose(fp);
                        free(fileContent);
                    }
                }
            }
        }
    }
    return 0;
}



