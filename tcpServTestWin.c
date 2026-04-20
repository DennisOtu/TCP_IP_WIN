//=---------------------------------------------------------------------------=
//=   Build:                                                                  =
//=            MinGW: gcc tcpServTestWin.c -lws2_32 -o tcpServTestWin         =
//=            Borland: bcc32 tcpServTestWin.c                                =
//=            Visual C: cl tcpServTestWin.c wsock32.lib                      =
//=---------------------------------------------------------------------------=

#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#include <windows.h>      // Needed for all Winsock stuff

#define  PORT_NUM   2000    // Arbitrary port number for the server

int main(){
  WORD wVersionRequested = MAKEWORD(1,1);   // For WSA functions
  WSADATA wsaData;                          // For WSA functions              
  int                  sockfd;       
  struct sockaddr_in   serv_addr;     
  int                  cli_sockfd;       
  struct sockaddr_in   cli_addr;     
  struct in_addr       cli_ip;  
  int                  addr_len;        
  char                 out_buf[4096];   
  char                 in_buf[4096];    
  int                  ret;         
  // Iitialize winsock
  WSAStartup(wVersionRequested, &wsaData);
  // Create a welcome socket
  //   - AF_INET is Address Family Internet and SOCK_STREAM is streams
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  serv_addr.sin_family = AF_INET;                 
  serv_addr.sin_port = htons(PORT_NUM);           
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 

  ret = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    
  if (ret < 0){
    printf("*** ERROR - bind() failed \n");
    exit(-1);
  }
  // Listen on welcome socket for a connection
  listen(sockfd, 5);
  // Accept a connection.  The accept() will block and then return with
  // cli_sockfd assigned and cli_addr filled-in.
  printf("Server Started: Listening... \n");

  addr_len = sizeof(cli_addr);
  cli_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &addr_len);

  if (cli_sockfd < 0){
    printf("*** ERROR - accept() failed \n");
    exit(-1);
  }
  // Copy the four-byte client IP address into an IP address structure
  memcpy(&cli_ip, &cli_addr.sin_addr.s_addr, 4);

  // Print an informational message that accept completed
  printf("Client Connected: IP Address = %s  Port = %d \n",
    inet_ntoa(cli_ip), ntohs(cli_addr.sin_port));

  memset(in_buf, 0, 4096);
  memset(out_buf, 0, 4096);

  while (1){
    // Receive from the client using the connect socket
    ret = recv(cli_sockfd, in_buf, sizeof(in_buf), 0);

    if (ret < 0){
      puts("Client Disconnected");
      puts("Server Closed");
      exit(-1);
    }

    printf("Client Message: %s \n", in_buf);

    // Send to the client using the connect socket
    strcpy(out_buf, "message recieved");
    ret = send(cli_sockfd, out_buf, (strlen(out_buf) + 1), 0);

    if (ret < 0){
      printf("*** ERROR - failed to send response \n");
      exit(-1);
    }

    memset(in_buf, 0, 4096);
    memset(out_buf, 0, 4096);
  }
  // Close the sockets
  ret = closesocket(sockfd);
  if (ret < 0){
    printf("*** ERROR - closesocket() failed \n");
    exit(-1);
  }
  
  ret = closesocket(cli_sockfd);
  if (ret < 0){
    printf("*** ERROR - closesocket() failed \n");
    exit(-1);
  }
  // Clean-up winsock
  WSACleanup();
  // Return zero and terminate
  return(0);
}

