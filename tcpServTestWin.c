//=---------------------------------------------------------------------------=
//=   Build:                                                                  =
//=            MinGW: gcc tcpServTestWin.c -lws2_32 -o tcpServTestWin         =
//=            Borland: bcc32 tcpServTestWin.c                                =
//=            Visual C: cl tcpServTestWin.c wsock32.lib                      =
//=---------------------------------------------------------------------------=

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h> // For _beginthreadex

#define PORT 2000
#define BUF_SIZE 4096

// Function executed by each client thread
unsigned int __stdcall cli_thread(void* param) {
  SOCKET cli_sockfd = *(SOCKET*)param;
  free(param); // Free the memory allocated for the socket pointer
  
  char msg_buf[BUF_SIZE];
  int ret;
  char *srv_reply = "message received";

  memset(msg_buf, 0, BUF_SIZE);

  while ((ret = recv(cli_sockfd, msg_buf, sizeof(msg_buf), 0)) > 0) {
    // Null-terminate the buffer for string operations if needed
    msg_buf[ret] = '\0';
    
    // Example: Echo the message back to the client
    send(cli_sockfd, srv_reply, strlen(srv_reply), 0);
    
    printf("Client message: %s\n", msg_buf);
    memset(msg_buf, 0, BUF_SIZE);
  }

  if (ret == 0) {
    printf("Client disconnected\n");
  } else {
    printf("Client error: %d\n", WSAGetLastError());
  }

  closesocket(cli_sockfd);
  return 0;
}

int main() {
  WSADATA wsaData;
  SOCKET sockfd, cli_sockfd;
  struct sockaddr_in serv_addr, cli_addr;
  int cli_addr_size = sizeof(cli_addr);
  unsigned int threadId;
  HANDLE hThread;

  // Initialize Winsock
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    printf("WSAStartup failed: %d\n", WSAGetLastError());
    return 1;
  }

  // Create server socket
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == INVALID_SOCKET) {
    printf("Socket creation failed: %d\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  // Set up server address structure
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT); // Listening port

  // Bind the socket
  if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
    printf("Bind failed: %d\n", WSAGetLastError());
    closesocket(sockfd);
    WSACleanup();
    return 1;
  }

  // Listen for incoming connections
  if (listen(sockfd, 5) == SOCKET_ERROR) {
    printf("Listen failed: %d\n", WSAGetLastError());
    closesocket(sockfd);
    WSACleanup();
    return 1;
  }

  printf("Server started \nListening on port %d\n", PORT);

  // Accept loop
  while (1) {
    cli_sockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_addr_size);
    if (cli_sockfd == INVALID_SOCKET) {
      printf("Accept failed: %d\n", WSAGetLastError());
      continue;
    }

    printf("Client connected \nClient IP: %s\n", inet_ntoa(cli_addr.sin_addr));

    // Allocate memory for the socket to pass to the thread
    SOCKET* pCli_sockfd = malloc(sizeof(SOCKET));
    *pCli_sockfd = cli_sockfd;

    // Create a new thread for the client
    hThread = (HANDLE)_beginthreadex(NULL, 0, cli_thread, pCli_sockfd, 0, &threadId);
    if (hThread == 0) {
      printf("Thread creation failed.\n");
      closesocket(cli_sockfd);
      free(pCli_sockfd);
    } else {
      CloseHandle(hThread);
    }
  }
  closesocket(sockfd);
  WSACleanup();
  return 0;
}   


