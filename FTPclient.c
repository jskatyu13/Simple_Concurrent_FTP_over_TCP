/*
FTPclient.c
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
#include <fcntl.h>

#define PORT 12345 // Define Port Number
#define BUFFER_SIZE 4096 // Define Buffer Size
#define SERVER_ADDRESS "127.0.0.1" // Define Server Address
#define FILE_SIZE 16384 // Define maximum file Size that can be download

// user directory command handler function
void userDirCommd(int clientSocket, char *buffer);
// user get function
void userGet(int clientSocket, char * commandBuffer);

int main(){
    int clientSocket;
    int retConnect;
    struct sockaddr_in serverAddress;
    char commandBuffer[BUFFER_SIZE];  // command buffer sent to the server
    

    // define the server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // create client socket
    clientSocket = socket(AF_INET,SOCK_STREAM,0); //TCP stream socket on IPv4
    if (clientSocket < 0){
        perror("Error in socket creation.\n");
        return -1;
    }
    else{
        printf("Client socket is created.\n");
    }
    
    // connect to the server with server address
    retConnect = connect(clientSocket,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
    if (retConnect < 0){
        perror("Error in connection.\n");
        return -1;
    }
    else{
        printf("Connected to FTP server @ %s\n",SERVER_ADDRESS);
    }

    // run the client 
    while(1){
        printf("\nuse 'help' command to display currently availbale commands.\n");
        printf("myFTPclient>");
        bzero(commandBuffer,sizeof(commandBuffer)); // clear the command buffer
        fgets(commandBuffer,sizeof(commandBuffer),stdin); //read command from user input

        // send exit/bye command to exit from the client process from the server
        if(!strncmp(commandBuffer,"bye",3) || !strncmp(commandBuffer,"exit",4)){
            send(clientSocket,commandBuffer,strlen(commandBuffer)-1,0);
            close(clientSocket);
            printf("Disconnected from FTP server\n");
            exit (0);
        }
        
        // display currently available commands for user
        else if(!strncmp(commandBuffer,"help",4)){
            printf("List files at current directory: ’lls’\n");
            printf("Display the current location: ‘lpwd’\n");
            printf("Change the current directory: ‘lcd’\n");
            printf("Exit from the client process: ‘bye’ or 'exit'\n");
            printf("Download files from the current directory: 'get'\n");
        }

        // call userDirCommd to display the current directory location, change the current directory or lsit files at current directory
        else if (!strncmp(commandBuffer,"lpwd",4)||!strncmp(commandBuffer,"lcd",3)||!strncmp(commandBuffer,"lls",3)){
            userDirCommd(clientSocket,commandBuffer);
        }
        
        // download a file at the current directory
        else if (!strncmp(commandBuffer,"get",3))
        {
            userGet(clientSocket,commandBuffer);
        }
        else{
            printf("Error in command, please enter again\n");
        }
        bzero(commandBuffer,sizeof(commandBuffer));
    }
    return 0;
}

void userDirCommd(int clientSocket, char *commandBuffer){
    char recv_data[BUFFER_SIZE];
    bzero(recv_data,sizeof(recv_data));
    send(clientSocket,commandBuffer,strlen(commandBuffer)-1,0);
    recv(clientSocket,recv_data,sizeof(recv_data),0);
    printf("%s\n",recv_data);
}

void userGet(int clientSocket, char * commandBuffer){
    char recv_data[FILE_SIZE];
    char fileName[BUFFER_SIZE];
    strcpy(fileName,commandBuffer+4);
    fileName[strlen(fileName)-1] = '\0';

    send(clientSocket,commandBuffer,strlen(commandBuffer)-1,0);

    // receive file content or error message
    recv(clientSocket,recv_data,FILE_SIZE,0);
    if (!strncmp(recv_data,"Can not find the file!",22)){
        printf("%s\n",recv_data);
    }
    else{
        // write the receive content into a local file
        FILE *fp;
        if((fp = fopen(fileName,"wb"))==NULL){
            printf("can not write the file locally\n");
        }
        else{
            fwrite (recv_data , sizeof(char), strlen(recv_data), fp);
            fclose(fp);
            printf("File download succesfully.\n");
        }
    }  
}

