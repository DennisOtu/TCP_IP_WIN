//=---------------------------------------------------------------------------=
//=   Build:                                                                  =
//=            MinGW: gcc tcpCliTestWin.c -lws2_32 -o tcpCliTestWin           =
//=            Borland: bcc32 tcpCliTestWin.c                                 =
//=            Visual C: cl tcpCliTestWin.c wsock32.lib                       =
//=---------------------------------------------------------------------------=

#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#include <windows.h>      // Needed for all Winsock stuff

#define  PORT         2000   // Port number used at the server
#define  SERV_IP    "127.0.0.1"  // IP address of server (*** HARDWIRED ***)
#define BUF_SIZE 4096

int main(){
  WORD wVersionRequested = MAKEWORD(1,1);       // For WSA functions
  WSADATA wsaData;                              // For WSA functions
  int                  sockfd;        
  struct sockaddr_in   serv_addr;     
  char                 out_buf[BUF_SIZE];   
  char                 in_buf[BUF_SIZE];  
  int                  ret;        
  // Initialize winsock
  WSAStartup(wVersionRequested, &wsaData);
  // Create a client socket
  // AF_INET is Address Family Internet and SOCK_STREAM is streams
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  serv_addr.sin_family = AF_INET;                 
  serv_addr.sin_port = htons(PORT);           
  serv_addr.sin_addr.s_addr = inet_addr(SERV_IP); 

  ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if (ret < 0){
    printf("*** ERROR - connect() failed \n");
    exit(-1);
  }

  memset(in_buf, 0, BUF_SIZE);
  memset(out_buf, 0, BUF_SIZE);
  
  while (1){
    printf("Enter message (type 'exit' to close): ");
    fgets(in_buf, BUF_SIZE, stdin);
    // Remove new line character if present
    in_buf[strcspn(in_buf, "\n")] = '\0';

    if (strcmp(in_buf, "exit") == 0) {
      break;
    }
    // Send to the server using the client socket
    strcpy(out_buf, in_buf);

    ret = send(sockfd, out_buf, (strlen(out_buf) + 1), 0);
    if (ret < 0){
      printf("*** ERROR failed to send message \n");
      exit(-1);
    }
    
    memset(in_buf, 0 , BUF_SIZE);
    memset(out_buf, 0, BUF_SIZE);
    // Receive from the server using the client socket
    ret = recv(sockfd, in_buf, sizeof(in_buf), 0);
    if (ret < 0){
      printf("*** ERROR - Server not responding \n");
      exit(-1);
    }
    // Output the received message
    printf("Server Response: %s \n", in_buf);
  }
  // Close the client socket
  ret = closesocket(sockfd);
  if (ret < 0){
    printf("*** ERROR - closesocket() failed \n");
    exit(-1);
  }
  // Clean-up winsock
  WSACleanup();
  // Return zero and terminate
  return(0);
}


