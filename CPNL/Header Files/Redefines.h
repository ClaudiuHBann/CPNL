#ifndef _REDEFINES_H
#define _REDEFINES_H

#ifdef _WIN32

#define errnum h_errno
#define CLOSE_SOCKET(s) closesocket(s)

#define SHUTDOWN_RECEIVE	SD_RECEIVE
#define SHUTDOWN_SEND		SD_SEND
#define SHUTDOWN_BOTH		SD_BOTH

#elif defined(__linux__)

#define errnum errno
#define CLOSE_SOCKET(s) close(s)

#define SOCKET unsigned long long

#define INVALID_SOCKET	0
#define SOCKET_ERROR	-1

#define SHUTDOWN_RECEIVE	SHUT_RD
#define SHUTDOWN_SEND		SHUT_WR
#define SHUTDOWN_BOTH		SHUT_RDWR

#endif

#endif // _REDEFINES_H
