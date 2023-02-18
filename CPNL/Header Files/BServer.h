#ifndef _BSERVER_H
#define _BSERVER_H

#include <Base.h>
#include <BSocket.h>
#include <BClient.h>

class BServer : public BSocket
{
public:
	BServer();
	BServer(BSocket &bSocket);
	BServer(const int af, const int type, const int protocol);
	BServer(const int af, const int type, const int protocol, const unsigned long addr, const int family, const unsigned short port, const int backlog);
	BServer(const int af, const int type, const int protocol, const int family, const unsigned short port, const std::string& serverIP);
	BServer(const DefaultSocketConfigurations dsc, const unsigned short port);
	BServer(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string& serverIP);
	BServer(const SocketInfo& si);
	BServer(const BServer& bServer);
	virtual ~BServer();

	std::list<BSocket*>* GetClients();

	int Listen();
	int SendData(const SOCKET socket, const std::string &data, const int flags = 0);
	int SendData(BSocket& bSocket, const std::string &data, const int flags = 0);
	int SendData(BClient& bClient, const std::string &data, const int flags = 0);
	std::string ReceiveData(const SOCKET socket, const int bufLen = DEFAULT_BUFFER_LENGTH, const int flags = 0);
	std::string ReceiveData(BSocket& bSocket, const int bufLen = DEFAULT_BUFFER_LENGTH, const int flags = 0);
	std::string ReceiveData(BClient& bClient, const int bufLen = DEFAULT_BUFFER_LENGTH, const int flags = 0);
	SOCKET Accept();
	bool Accept(SOCKET& socket);
	bool Accept(BSocket& bSocket);
	bool Accept(BClient& bClient);
	int Disconnect(const SOCKET socket);
	int Disconnect(BSocket& bSocket);
	int Disconnect(BClient& bClient);
	void DisconnectSocketWithoutEcho(const SOCKET socket);
	int Block(const SOCKET socket, const int how);
	int Block(BSocket& bSocket, const int how);
	int Block(BClient& bClient, const int how);

protected:
	std::list<BSocket*> clients;

	virtual void OnStart();
	virtual void OnAccept(const SOCKET socket);
	virtual void OnSendData(const SOCKET socket);
	virtual void OnReceiveData(const SOCKET socket);
	virtual void OnBlock(const SOCKET socket);
	virtual void OnDisconnect(const SOCKET socket);
	virtual void OnEnd();
};

#endif // _BSERVER_H