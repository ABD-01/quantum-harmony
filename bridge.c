#include <stdio.h>
#include <windows.h>
// #include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

/******************************************************************************
**                         SERIAL APIS 
******************************************************************************/

EXPORT int serial_write(HANDLE hComm, char *buf, int len)
{
    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE)
    {
        printf("ERR HANDLE\r\n");
        return -1;
    }
    DWORD dwBytesWritten;
    if(!WriteFile(hComm, buf, len, &dwBytesWritten, NULL))
    {
        printf("ERR WRITE FILE %ld\r\n", GetLastError());
        return -1;
    }    
    return 0;
}

EXPORT DWORD serial_read(HANDLE hComm, char *buf, int len)
{
    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE)
    {
        printf("ERR HANDLE\r\n");
        return -1;
    }
    DWORD dwBytesRead = 0;
    if(!ReadFile(hComm, buf, len, &dwBytesRead, NULL))
    {
        printf("ERR READ FILE %ld\r\n", GetLastError());
        return -1;
    }    
    return dwBytesRead;
}

/**
 * @brief Reads a line terminating with \n or at most `limit` bytes from the serial port into the provided buffer.
 *
 * @param hSerial The handle to the serial port.
 * @param buffer The buffer to store the read data.
 * @param limit The maximum number of bytes to read.
 *
 * @return The total number of bytes read into the buffer. (-1) if there was an error reading from the serial port.
 * 
 */
EXPORT int readline(HANDLE hSerial, char *buffer, int limit)
{
    if (hSerial == NULL || hSerial == INVALID_HANDLE_VALUE) {
        printf("ERR HANDLE\r\n");
        return -1;
    }
    DWORD bytesRead;
    char ch;
    int buff_size = 0;
    memset(buffer, 0, limit);

    while (buff_size < limit) {
        if (!ReadFile(hSerial, &ch, 1, &bytesRead, NULL)) {
            printf("ERR READ FILE %ld\r\n", GetLastError());
            return -1;
        }
        if (bytesRead > 0) {
            buffer[buff_size++] = ch;
            if (ch == '\n') {
                break;
            }
        }

    }
    return buff_size;         
}

/**
 * @brief Clears the input buffer of the serial port.
 *
 * @param hSerial The handle to the serial port.
 *
 * @return 0 on success, -1 on failure.
 */
EXPORT int resetInputBuffer(HANDLE hSerial) {
    if (hSerial == NULL || hSerial == INVALID_HANDLE_VALUE) {
        printf("ERR HANDLE\r\n");
        return -1;
    }

    if(!PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_RXABORT)) 
        return -1;

    return 0;
}

EXPORT int resetOutputBuffer(HANDLE hSerial) {
    if (hSerial == NULL || hSerial == INVALID_HANDLE_VALUE) {
        printf("ERR HANDLE\r\n");
        return -1;
    }

    if(!PurgeComm(hSerial, PURGE_TXCLEAR | PURGE_TXABORT))
        return -1;

    return 0;
}

/******************************************************************************
**                         PROGRESS APIS 
******************************************************************************/

#define BUILD_IP(b1, b2, b3, b4)    ((b1) << 24 | (b2) << 16 | (b3) << 8 | (b4))
#define PORT_NO                     (42337)

EXPORT void sendProgress(int num_to_send) {
    WSADATA wsadata;
    SOCKET sock;
    struct sockaddr_in addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        // printf("Failed to initialize Winsock\n");
        return;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
        // printf("Failed to create socket\n");
        WSACleanup();
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(BUILD_IP(127, 0, 0, 1));
    addr.sin_port = htons(PORT_NO);

    char buff[2];
    buff[0] = (num_to_send >> 8) & 0x00FF;  // MSB
    buff[1] = num_to_send & 0x00FF;         // LSB
    if (sendto(sock, buff, sizeof(buff), 0, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        // printf("Failed to send data\n");
        closesocket(sock);
        WSACleanup();
        return;
    }

    closesocket(sock);
    WSACleanup();
}
