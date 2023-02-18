#ifndef _BASE_H
#define _BASE_H

#undef UNICODE

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string.h>
#include <string>
#include <list>

#ifdef _WIN32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment (lib, "Ws2_32.lib")

#elif defined(__linux__)

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif

#include <Redefines.h>

#define DEFAULT_PORT 27015
#define DEFAULT_LOCAL_HOST_IP "127.0.0.1"
#define DEFAULT_BUFFER_LENGTH 512

#define NULL63(var, err) Base::IsNull(var, err)

enum class ConsoleColors
{
#ifdef _WIN32
	FG_BLUE = 1,
	FG_GREEN = 2,
	FG_RED = 4,
	FG_INTENSITY = 8,
	BG_BLUE = 16,
	BG_GREEN = 32,
	BG_RED = 64,
	BG_INTENSITY = 128,
#elif defined(__linux__)
	FG_BLACK = 30,
	FG_RED,
	FG_GREEN,
	FG_YELLOW,
	FG_BLUE,
	FG_MAGENTA,
	FG_CYAN,
	FG_WHITE,
	FG_BRIGHT_BLACK = 90,
	FG_BRIGHT_RED,
	FG_BRIGHT_GREEN,
	FG_BRIGHT_YELLOW,
	FG_BRIGHT_BLUE,
	FG_BRIGHT_MAGENTA,
	FG_BRIGHT_CYAN,
	FG_BRIGHT_WHITE,
	BG_BLACK = 40,
	BG_RED,
	BG_GREEN,
	BG_YELLOW,
	BG_BLUE,
	BG_MAGENTA,
	BG_CYAN,
	BG_WHITE,
	BG_BRIGHT_BLACK = 100,
	BG_BRIGHT_RED,
	BG_BRIGHT_GREEN,
	BG_BRIGHT_YELLOW,
	BG_BRIGHT_BLUE,
	BG_BRIGHT_MAGENTA,
	BG_BRIGHT_CYAN,
	BG_BRIGHT_WHITE,
#endif
};

class Base
{
public:

	static void PrintError(const std::string& errorMessage, const unsigned short color = (unsigned short)ConsoleColors::FG_RED
#ifdef _WIN32
		, HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE)
#endif
	);
	static bool IsNull(const void* variable, const std::string& error);

#ifdef _WIN32

	static bool IsWSAInitialized();

	static WSADATA* CreateWSADATAPointer();
	static WSADATA CreateWSADATA();

#endif

	static sockaddr_in* CreateSockAddrInPointer(const unsigned long addr, const int family, const unsigned short port);
	static sockaddr_in CreateSockAddrIn(const unsigned long addr, const int family, const unsigned short port);

	static SOCKET CreateSocket(const int af, const int type, const int protocol);
};

#endif // _BASE_H
