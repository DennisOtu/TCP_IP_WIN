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
#include <stdbool.h>

#define  PORT_NUM         2000   // Port number used at the server
#define  IP_ADDR    "127.0.0.1"  // IP address of server (*** HARDWIRED ***)

int main(){
  WORD wVersionRequested = MAKEWORD(1,1);       // For WSA functions
  WSADATA wsaData;                              // For WSA functions
  int                  sockfd;        
  struct sockaddr_in   serv_addr;     
  char                 out_buf[4096];   
  char                 in_buf[4096];  
  int                  ret;        
  // Initialize winsock
  WSAStartup(wVersionRequested, &wsaData);
  // Create a client socket
  //   - AF_INET is Address Family Internet and SOCK_STREAM is streams
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  serv_addr.sin_family = AF_INET;                 
  serv_addr.sin_port = htons(PORT_NUM);           
  serv_addr.sin_addr.s_addr = inet_addr(IP_ADDR); 

  ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if (ret < 0){
    printf("*** ERROR - connect() failed \n");
    exit(-1);
  }

  memset(in_buf, 0, 4096);
  memset(out_buf, 0, 4096);
  
  while (true){
    printf("Enter message (type 'exit' to close): ");
    fgets(in_buf, 4096, stdin);
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
    
    memset(in_buf, 0 , 4096);
    memset(out_buf, 0, 4096);
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


