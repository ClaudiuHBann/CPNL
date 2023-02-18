#ifndef _BCLIENT_H
#define _BCLIENT_H

#include <Base.h>
#include <BSocket.h>

class BClient : public BSocket
{
public:
	BClient();
	BClient(BSocket& bSocket);
	BClient(const int af, const int type, const int protocol, const int family, const unsigned short port, const std::string& serverIP);
	BClient(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string& serverIP);
	BClient(const SocketInfo& si);
	BClient(const BClient &bClient);
	virtual ~BClient();

	int Connect();
	int SendData(const std::string& data, const int flags = 0);
	std::string ReceiveData(const int bufLen = DEFAULT_BUFFER_LENGTH, const int flags = 0);
	int Disconnect();
	void DisconnectSocketWithoutEcho(const SOCKET socket);

protected:
	virtual void OnConnect(const SOCKET socket);
	virtual void OnSendData(const SOCKET socket);
	virtual void OnReceiveData(const SOCKET socket);
	virtual void OnDisconnect(const SOCKET socket);
};

#endif // _BCLIENT_H