#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>

#include <sys/EfiSysCall.h>
#include <sys/fd_set.h>
#include <sys/socket.h>


/**
  This function is the main TCP client for UEFI. It allows the user to connect to a server
  via TCP/IPv4, send messages, and receive responses. It uses standard BSD sockets and
  UEFI system calls for network communication.

  @param[in] Argc     The number of command line arguments passed to the application.
  @param[in] Argv     The list of command line arguments.

  @retval EFI_SUCCESS       The function executes successfully.
  @retval EFI_ABORTED       The function encounters an error, such as connection failure.

**/
EFI_STATUS
tcp4Client (
  IN int   Argc,
  IN char  **Argv
  )
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 ServerIPAddr[4];
    UINT16 ServerPort;
    struct sockaddr_in ServerIp;
    UINTN socketID;
    UINTN recvLen;
    UINTN connectStatus;
    UINTN sendStatus;
    char msgStr[1024];
    char recvStr[1024];

    // 1. Get the server IP address and port number from command-line arguments
    if (Argc != 3) {
        Print(L"UEFI TCP Client. Usage: %a <ServerIP> <port>\n", Argv[0]);
        Status = EFI_ABORTED;
        return Status;
    } else {
        // Convert the command line arguments into server IP address and port number
        // StdLib\LibC\Stdio\sscanf.c
        sscanf(Argv[2], "%d", &ServerPort);
        sscanf(Argv[1], "%d.%d.%d.%d", &ServerIPAddr[0], &ServerIPAddr[1], &ServerIPAddr[2], &ServerIPAddr[3]);
    }
  
    Print(L"IP: %d.%d.%d.%d, port: %d \n", ServerIPAddr[0], ServerIPAddr[1], ServerIPAddr[2], ServerIPAddr[3], ServerPort);

    // 2. Create a socket for TCP communication
    // The socket function creates a new socket using IPv4 (AF_INET), stream (SOCK_STREAM), and TCP (IPPROTO_TCP)
    // StdLib\BsdSocketLib\socket.c
    socketID = socket (
        AF_INET,      // /* internetwork: UDP, TCP, etc. */
        SOCK_STREAM,  // /* stream socket */
        IPPROTO_TCP   // /* tcp */
    );

    Print(L"Create the socket, please wait...\n");

    if (socketID == -1) {
        Status = EFI_ABORTED;
        Print(L"Can't create socket, quit the app!\n");
    } 

    // 3. Set up the server address and connect to the server
    memset(&ServerIp, 0, sizeof(ServerIp));  // Zero out the server address structure
    ServerIp.sin_len         = sizeof(ServerIp);  // Set the length of the address structure
    ServerIp.sin_family      = AF_INET;  // Address family: IPv4
    ServerIp.sin_port        = htons(ServerPort);  // Convert port number to network byte order
    ServerIp.sin_addr.s_addr = (ServerIPAddr[3] << 24) |
                               (ServerIPAddr[2] << 16) |
                               (ServerIPAddr[1] << 8)  |
                                ServerIPAddr[0];  // Convert IP address to network byte order
    // The connect function connects the socket to the specified server address
    // StdLib\BsdSocketLib\connect.c
    connectStatus = connect (
        socketID,                     // Socket file descriptor returned from ::socket.
        (struct sockaddr*)&ServerIp,  // Network address of the remote system
        sizeof(ServerIp)              // Length of the remote network address
    );

    Print(L"Connect to server, please wait...\n");

    if (connectStatus == -1) {
        Status = EFI_ABORTED;
        Print(L"Connect() error, quit the app!\n");
        return Status;
    }

    // 4. Enter into a loop to send and receive messages from the server
    while (1)
    {
        #if 1
        // 4. Get message from the user and send it to the server
        memset(msgStr, 0, 1024);  // Clear previous message

        Print(L"Please input message:");

        // StdLib\LibC\Stdio\gets.c
        gets(msgStr); //get string from keyboard
        if (!strcmp(msgStr, "q") || !strcmp(msgStr, "Q")) {
            break;
        }

        // StdLib\BsdSocketLib\send.c
        sendStatus = send (
            socketID,         // Socket file descriptor returned from ::socket.
            msgStr,           // Address of a buffer containing the data to send.
            strlen(msgStr),   // Length of the buffer in bytes.
            0                 // Message control flags
        );
        if (sendStatus == -1) {
            Print(L"send() error!\n");
        }
        #endif

        // StdLib\BsdSocketLib\recv.c
        recvLen = (UINTN)recv (
            socketID,         // Socket file descriptor returned from ::socket.
            recvStr,          // Address of a buffer to receive the data.
            sizeof(recvStr),  // Length of the buffer in bytes.
            0                 // Message control flags
        );
        recvStr[recvLen] = '\0';
        Print(L"Message from server: %s\n", recvStr);
    }

    // StdLib\LibC\Uefi\SysCalls.c
    close(socketID);

    return Status;
}


/**
  The entry point for the application. It calls the tcp4Client function
  to initiate a TCP/IPv4 connection and communicate with the server.

  @param[in] Argc     The number of command line arguments passed to the application.
  @param[in] Argv     The list of command line arguments.

  @retval 0           The application executes successfully.
**/
int
main (
  IN int   Argc,
  IN char  **Argv
  )
{
    tcp4Client(Argc, Argv);
    return 0;
}