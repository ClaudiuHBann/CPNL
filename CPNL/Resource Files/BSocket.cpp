#include "BSocket.h"

BSocket::BSocket()
{

}

BSocket::BSocket(const int af, const int type, const int protocol)
{
	SetBSocket(af, type, protocol);
}

BSocket::BSocket(const SocketInfo& si)
{
	if (NULL63(&si, "SocketInfo& parameter from BSocket constructor call is null!"))
	{
		return;
	}

	SetBSocket(si);
}

BSocket::BSocket(const int af, const int type, const int protocol, const unsigned long addr, const int family, const unsigned short port, const int backlog)
{
	SetBSocket(af, type, protocol);
	EvolveToServer(addr, family, port, backlog);
}

BSocket::BSocket(const int af, const int type, const int protocol, const int family, const unsigned short port, const std::string& serverIP)
{
	if (NULL63(&serverIP, "std::string& parameter from BSocket constructor call is null!"))
	{
		return;
	}

	SetBSocket(af, type, protocol);
	EvolveToClient(family, port, serverIP);
}

BSocket::BSocket(const DefaultSocketConfigurations dsc, const unsigned short port)
{
	SetBSocket(dsc, port);
}

BSocket::BSocket(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string& serverIP)
{
	if (NULL63(&serverIP, "std::string& parameter from BSocket constructor call is null!"))
	{
		return;
	}

	SetBSocket(dsc, port, serverIP);
}

BSocket::BSocket(const BSocket& bSocket)
{
	if (NULL63(&si, "BSocket& parameter from BSocket constructor call is null!"))
	{
		return;
	}

	socket = bSocket.socket;
	si = bSocket.si;
}

BSocket::~BSocket()
{
// 	int iResult = shutdown(socket, SHUTDOWN_BOTH);
// 	if (iResult == SOCKET_ERROR)
// 	{
// 		Base::PrintError("Failed on shutting down the socket! Error: " + std::to_string(errnum));
// 	}
// 
// 	CLOSE_SOCKET(socket);
}

bool BSocket::IsEvolved() const
{
	return (isServer == false && isClient == false) ? false : true;
}

bool BSocket::IsClient() const
{
	return isClient;
}

bool BSocket::IsServer() const
{
	return isServer;
}

void BSocket::SetBSocket(const int af, const int type, const int protocol, const unsigned long addr, const int family, const unsigned short port, const int backlog)
{
	SetBSocket(af, type, protocol);
	EvolveToServer(addr, family, port, backlog);
}

void BSocket::SetBSocket(const int af, const int type, const int protocol, const int family, const unsigned short port, const std::string& serverIP)
{
	if (NULL63(&serverIP, "std::string& parameter from BSocket constructor call is null!"))
	{
		return;
	}

	SetBSocket(af, type, protocol);
	EvolveToClient(family, port, serverIP);
}

void BSocket::SetBSocket(const DefaultSocketConfigurations dsc, const unsigned short port)
{
	(dsc == DefaultSocketConfigurations::TCP) ? SetBSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP) : SetBSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	EvolveToServer(dsc, port);
}

void BSocket::SetBSocket(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string& serverIP)
{
	if (NULL63(&serverIP, "std::string& parameter from BSocket::SetBSocket constructor call is null!"))
	{
		return;
	}

	(dsc == DefaultSocketConfigurations::TCP) ? SetBSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP) : SetBSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	EvolveToClient(dsc, port, serverIP);
}

void BSocket::SetBSocket(const BSocket& bSocket)
{
	if (NULL63(&bSocket, "BSocket& parameter from BSocket::SetBSocket constructor call is null!"))
	{
		return;
	}

	socket = bSocket.socket;
	si = bSocket.si;
}

void BSocket::SetBSocket(const int af, const int type, const int protocol)
{
	si.sin.sin_addr.s_addr = 0;
	si.sin.sin_family = 0;
	si.sin.sin_port = 0;
	si.af = af;
	si.type = type;
	si.protocol = protocol;
	si.backlog = -1;

	socket = Base::CreateSocket(af, type, protocol);
}

void BSocket::SetBSocket(const SocketInfo& si)
{
	if (NULL63(&si, "SocketInfo& parameter from BSocket::SetBSocket function call is null!"))
	{
		return;
	}

	socket = Base::CreateSocket(si.af, si.type, si.protocol);
	this->si = si;

	if (si.backlog != -1)
	{
		EvolveToServer(si.sin.sin_addr.s_addr, si.sin.sin_family, htons(si.sin.sin_port), si.backlog);
	}
	else
	{
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(si.sin.sin_addr), str, INET_ADDRSTRLEN);
		EvolveToClient(si.sin.sin_family, htons(si.sin.sin_port), str);
	}
}

bool BSocket::EvolveToServer(const DefaultSocketConfigurations dsc, const unsigned short port)
{
	si.af = AF_INET;

	if (dsc == DefaultSocketConfigurations::TCP)
	{
		si.type = SOCK_STREAM;
		si.protocol = IPPROTO_TCP;
	}
	else
	{
		si.type = SOCK_DGRAM;
		si.protocol = IPPROTO_UDP;
	}

	return EvolveToServer(INADDR_ANY, AF_INET, port, SOMAXCONN);
}

bool BSocket::EvolveToServer(const unsigned long addr, const int family, const unsigned short port, const int backlog)
{
	if (IsEvolved())
	{
		Base::PrintError("The BSocket has been already evolved in a" + (isServer) ? "server!" : "client!");
		return false;
	}

	bool toReturn = true;

	si.sin.sin_addr.s_addr = addr;
	si.sin.sin_family = family;
	si.sin.sin_port = htons(port);
	si.backlog = backlog;

	int iResult = bind(socket, (sockaddr*)&si.sin, sizeof(si.sin));
	if (iResult == SOCKET_ERROR)
	{
		toReturn = false;
		Base::PrintError("Failed on binding the socket! Error: " + std::to_string(errnum));
		CLOSE_SOCKET(socket);
	}
	else
	{
		isServer = true;
	}

	return toReturn;
}

bool BSocket::EvolveToClient(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string& serverIP)
{
	si.af = AF_INET;

	if (dsc == DefaultSocketConfigurations::TCP)
	{
		si.type = SOCK_STREAM;
		si.protocol = IPPROTO_TCP;
	}
	else
	{
		si.type = SOCK_DGRAM;
		si.protocol = IPPROTO_UDP;
	}

	return EvolveToClient(AF_INET, port, serverIP);
}

bool BSocket::EvolveToClient(const int family, const unsigned short port, const std::string& serverIP)
{
	if (IsEvolved())
	{
		Base::PrintError("The BSocket has been already evolved in a" + (isServer) ? "server!" : "client!");
		return false;
	}

	bool toReturn = true;

	si.sin.sin_family = family;
	si.sin.sin_port = htons(port);

	if (inet_pton(family, serverIP.c_str(), &si.sin.sin_addr) <= 0)
	{
		toReturn = false;
		Base::PrintError("Failed on converting the IP of the server! Error: " + std::to_string(errnum));
	}
	else
	{
		isClient = true;
	}

	return toReturn;
}

SOCKET* BSocket::GetSocket()
{
	return &socket;
}

SocketInfo* BSocket::GetSocketInfo()
{
	return &si;
}

void BSocket::SetSocket(const SOCKET socket)
{
	this->socket = socket;
}

void BSocket::SetSocketInfo(const SocketInfo& si)
{
	this->si = si;
}
