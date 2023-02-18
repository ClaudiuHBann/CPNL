#include "BClient.h"

BClient::BClient()
{

}

BClient::BClient(const int af, const int type, const int protocol, const int family, const unsigned short port, const std::string& serverIP)
{
	if (NULL63(&serverIP, "std::string& parameter from BClient constructor call is null!"))
	{
		return;
	}

	SetBSocket(af, type, protocol, family, port, serverIP);
}

BClient::BClient(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string& serverIP)
{
	if (NULL63(&serverIP, "std::string& parameter from BClient constructor call is null!"))
	{
		return;
	}

	SetBSocket(dsc, port, serverIP);
}

BClient::BClient(const SocketInfo& si)
{
	if (NULL63(&si, "SocketInfo& parameter from BClient constructor call is null!"))
	{
		return;
	}

	if (si.backlog >= 0)
	{
		Base::PrintError("SocketInfo& parameter is not configured for a BClient!");
		return;
	}

	SetBSocket(si);
}

BClient::BClient(BSocket& bSocket)
{
	if (NULL63(&bSocket, "BSocket& parameter from BClient constructor call is null!"))
	{
		return;
	}

	socket = *bSocket.GetSocket();
	si = *bSocket.GetSocketInfo();
}

BClient::BClient(const BClient& bClient)
{
	if (NULL63(&bClient, "BClient& parameter from BClient constructor call is null!"))
	{
		return;
	}

	socket = bClient.socket;
	si = bClient.si;
}

BClient::~BClient()
{
	OnDisconnect(socket);
	DisconnectSocketWithoutEcho(socket);
}

int BClient::Connect()
{
	int iResult = connect(socket, (sockaddr*)&GetSocketInfo()->sin, sizeof(GetSocketInfo()->sin));
	if (iResult == SOCKET_ERROR)
	{
		Base::PrintError("Failed on connecting to the server! Error: " + std::to_string(errnum));
		DisconnectSocketWithoutEcho(socket);
	}
	else
	{
		OnConnect(socket);
	}

	return iResult;
}

int BClient::SendData(const std::string& data, const int flags /* = 0 */)
{
	if (NULL63(&data, "std::string& parameter from BClient::SendData function call is null!"))
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

std::string BClient::ReceiveData(const int bufLen /* = DEFAULT_BUFFER_LENGTH */, const int flags /* = 0 */)
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

void BClient::DisconnectSocketWithoutEcho(const SOCKET socket)
{
	OnDisconnect(socket);

	shutdown(socket, SHUTDOWN_BOTH);
	CLOSE_SOCKET(socket);
}

int BClient::Disconnect()
{
	OnDisconnect(socket);

	int iResult = shutdown(socket, SHUTDOWN_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		Base::PrintError("Failed on disconnecting from server! Error: " + std::to_string(errnum));
	}

	CLOSE_SOCKET(socket);

	return iResult;
}

void BClient::OnConnect(const SOCKET socket)
{
	std::cout << "You have been connected to the server!" << std::endl;
}

void BClient::OnSendData(const SOCKET socket)
{
	std::cout << "You have sent data to the server!" << std::endl;
}

void BClient::OnReceiveData(const SOCKET socket)
{
	std::cout << "You have received data from the server!" << std::endl;
}

void BClient::OnDisconnect(const SOCKET socket)
{
	std::cout << "You have been disconnected from the server!" << std::endl;
}
