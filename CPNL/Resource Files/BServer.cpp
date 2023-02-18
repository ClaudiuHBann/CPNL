#include "BServer.h"

BServer::BServer()
{
	OnStart();
}

BServer::BServer(BSocket& bSocket)
{
	if (NULL63(&bSocket, "BSocket& parameter from BServer constructor call is null!"))
	{
		return;
	}

	OnStart();

	socket = *bSocket.GetSocket();
	si = *bSocket.GetSocketInfo();
}

BServer::BServer(const BServer& bServer)
{
	if (NULL63(&bServer, "BServer& parameter from BServer constructor call is null!"))
	{
		return;
	}

	OnStart();

	socket = bServer.socket;
	si = bServer.si;

	for (auto& i : bServer.clients)
	{
		BSocket* newBSocket = new BSocket(*i);
		clients.push_back(newBSocket);
	}
}

BServer::BServer(const int af, const int type, const int protocol, const unsigned long addr, const int family, const unsigned short port, const int backlog)
{
	OnStart();
	SetBSocket(af, type, protocol, addr, family, port, backlog);
}

BServer::BServer(const DefaultSocketConfigurations dsc, const unsigned short port)
{
	OnStart();
	SetBSocket(dsc, port);
}

BServer::BServer(const SocketInfo& si)
{
	if (NULL63(&si, "SocketInfo& parameter from BServer constructor call is null!"))
	{
		return;
	}

	if (si.backlog < 0)
	{
		Base::PrintError("SocketInfo& parameter is not configured for a BServer!");
		return;
	}

	OnStart();
	SetBSocket(si);
}

BServer::~BServer()
{
	OnEnd();

	for (auto &&i : clients)
	{
		DisconnectSocketWithoutEcho(*i->GetSocket());
		delete i;
	}

	clients.clear();

	DisconnectSocketWithoutEcho(socket);
}

int BServer::SendData(const SOCKET socket, const std::string& data, const int flags /* = 0 */)
{
	if (NULL63(&data, "std::string& parameter from BServer::SendData function call is null!"))
	{
		return -1;
	}

	int bytesSend = send(socket, data.c_str(), data.size() + 1, flags);
	if (bytesSend == SOCKET_ERROR)
	{
		Base::PrintError("Failed on sending data to the socket! Error: " + std::to_string(errnum));
		DisconnectSocketWithoutEcho(socket);
	}
	else
	{
		OnSendData(socket);
	}

	return bytesSend;
}

int BServer::Listen()
{
	int iResult = listen(socket, si.backlog);
	if (iResult == SOCKET_ERROR)
	{
		Base::PrintError("Failed on listening the socket! Error: " + std::to_string(errnum));
		CLOSE_SOCKET(socket);
	}

	return iResult;
}

std::string BServer::ReceiveData(const SOCKET socket, const int bufLen /* = DEFAULT_BUFFER_LENGTH */, const int flags /* = 0 */)
{
	char* buffer = new char[bufLen];
	std::string receivedData("");

	int iResult = recv(socket, buffer, bufLen, flags);
	if (iResult > 0)
	{
		OnReceiveData(socket);
		receivedData.assign(buffer);
	}
	else if (iResult == 0)
	{
		DisconnectSocketWithoutEcho(socket);
	}
	else
	{
		Base::PrintError("Failed on receiving data from the socket! Error: " + std::to_string(errnum));
		DisconnectSocketWithoutEcho(socket);
	}

	delete[] buffer;

	return receivedData;
}

int BServer::Disconnect(const SOCKET socket)
{
	OnDisconnect(socket);

	for (auto&& i : clients)
	{
		if (*i->GetSocket() == socket)
		{
			clients.remove(i);
			delete i;
			break;
		}
	}

	int iResult = shutdown(socket, SHUTDOWN_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		Base::PrintError("Failed on disconnecting the socket! Error: " + std::to_string(errnum));
	}

	CLOSE_SOCKET(socket);

	return iResult;
}

void BServer::DisconnectSocketWithoutEcho(const SOCKET socket)
{
	OnDisconnect(socket);

	for (auto&& i : clients)
	{
		if (*i->GetSocket() == socket)
		{
			delete i;
			clients.remove(i);
			break;
		}
	}

	shutdown(socket, SHUTDOWN_BOTH);
	CLOSE_SOCKET(socket);
}

std::list<BSocket*>* BServer::GetClients()
{
	return &clients;
}

int BServer::Block(const SOCKET socket, const int how)
{
	int iResult = shutdown(socket, how);
	if (iResult == SOCKET_ERROR)
	{
		Base::PrintError("Failed on blocking the socket! Error: " + std::to_string(errnum));
		DisconnectSocketWithoutEcho(socket);
	}
	else
	{
		OnBlock(socket);
	}

	return iResult;
}

SOCKET BServer::Accept()
{
	BSocket* newBSocket = new BSocket();
	int addrLen = sizeof(newBSocket->GetSocketInfo()->sin);

	SOCKET newSocket = accept(socket, (sockaddr*)&newBSocket->GetSocketInfo()->sin, (socklen_t*)&addrLen);
	if (newSocket == INVALID_SOCKET)
	{
		Base::PrintError("Failed on accepting a socket! Error: " + std::to_string(errnum));
		delete newBSocket;
	}
	else
	{
		*newBSocket->GetSocket() = newSocket;
		clients.push_back(newBSocket);

		OnAccept(newSocket);
	}

	return newSocket;
}

int BServer::SendData(BSocket& bSocket, const std::string& data, const int flags /* = 0 */)
{
	if (NULL63(&bSocket, "BSocket& parameter from BServer::SendData function call is null!") ||
		NULL63(&data, "std::string& parameter from BServer::SendData function call is null!"))
	{
		return -1;
	}

	return SendData(*bSocket.GetSocket(), data, flags);
}

int BServer::SendData(BClient& bClient, const std::string& data, const int flags /* = 0 */)
{
	if (NULL63(&bClient, "BClient& parameter from BServer::SendData function call is null!") ||
		NULL63(&data, "std::string& parameter from BServer::SendData function call is null!"))
	{
		return -1;
	}

	return SendData(*bClient.GetSocket(), data, flags);
}

std::string BServer::ReceiveData(BSocket& bSocket, const int bufLen /* = DEFAULT_BUFFER_LENGTH */, const int flags /* = 0 */)
{
	if (NULL63(&bSocket, "BSocket& parameter from BServer::ReceiveData function call is null!"))
	{
		return std::string("");
	}

	return ReceiveData(*bSocket.GetSocket(), bufLen, flags);
}

std::string BServer::ReceiveData(BClient& bClient, const int bufLen /* = DEFAULT_BUFFER_LENGTH */, const int flags /* = 0 */)
{
	if (NULL63(&bClient, "BClient& parameter from BServer::ReceiveData function call is null!"))
	{
		return std::string("");
	}

	return ReceiveData(*bClient.GetSocket(), bufLen, flags);
}

bool BServer::Accept(SOCKET& socket)
{
	SOCKET toReturn = Accept();

	if (toReturn != INVALID_SOCKET)
	{
		socket = *clients.back()->GetSocket();
	}

	return toReturn != INVALID_SOCKET;
}

bool BServer::Accept(BSocket& bSocket)
{
	SOCKET toReturn = Accept();

	if (toReturn != INVALID_SOCKET)
	{
		bSocket = *clients.back();
	}

	return toReturn != INVALID_SOCKET;
}

bool BServer::Accept(BClient& bClient)
{
	SOCKET toReturn = Accept();

	if (toReturn != INVALID_SOCKET)
	{
		*bClient.GetSocketInfo() = *clients.back()->GetSocketInfo();
		*bClient.GetSocket() = *clients.back()->GetSocket();
	}

	return toReturn != INVALID_SOCKET;
}

int BServer::Disconnect(BSocket& bSocket)
{
	if (NULL63(&bSocket, "bSocket& parameter from BServer::Disconnect function call is null!"))
	{
		return -1;
	}

	return Disconnect(*bSocket.GetSocket());
}

int BServer::Disconnect(BClient& bClient)
{
	if (NULL63(&bClient, "BClient& parameter from BServer::Disconnect function call is null!"))
	{
		return -1;
	}

	return Disconnect(*bClient.GetSocket());
}

int BServer::Block(BSocket& bSocket, const int how)
{
	if (NULL63(&bSocket, "BSocket& parameter from BServer::Block function call is null!"))
	{
		return -1;
	}

	return Block(*bSocket.GetSocket(), how);
}

int BServer::Block(BClient& bClient, const int how)
{
	if (NULL63(&bClient, "BClient& parameter from BServer::Block function call is null!"))
	{
		return -1;
	}

	return Block(*bClient.GetSocket(), how);
}

void BServer::OnStart()
{
	std::cout << "The server has been started!" << std::endl;
}

void BServer::OnAccept(const SOCKET socket)
{
	std::cout << "A client has been accepted!" << std::endl;
}

void BServer::OnSendData(const SOCKET socket)
{
	std::cout << "The server has sent data!" << std::endl;
}

void BServer::OnReceiveData(const SOCKET socket)
{
	std::cout << "The server has received data!" << std::endl;
}

void BServer::OnBlock(const SOCKET socket)
{
	std::cout << "A client has been blocked!" << std::endl;
}

void BServer::OnDisconnect(const SOCKET socket)
{
	std::cout << "A client has been disconnected!" << std::endl;
}

void BServer::OnEnd()
{
	std::cout << "The server has been ended!" << std::endl;
}