#ifndef _BSOCKET_H
#define _BSOCKET_H

#include <Base.h>

typedef struct socketInfo
{
    sockaddr_in sin;
    int af;
    int type;
    int protocol;
    int backlog;
} SocketInfo;

enum class DefaultSocketConfigurations
{
    TCP = 0,
    UDP
};

enum class TCPServerDefaultSocketConfiguration
{
    ADDR = INADDR_ANY,
    FAMILY = AF_INET,
    PORT = DEFAULT_PORT,
    AF = AF_INET,
    TYPE = SOCK_STREAM,
    PROTOCOL = IPPROTO_TCP,
    BACKLOG = SOMAXCONN
};

enum class UDPServerDefaultSocketConfiguration
{
    ADDR = INADDR_ANY,
    FAMILY = AF_INET,
    PORT = DEFAULT_PORT,
    AF = AF_INET,
    TYPE = SOCK_DGRAM,
    PROTOCOL = IPPROTO_UDP,
    BACKLOG = SOMAXCONN
};

enum class TCPClientDefaultSocketConfiguration
{
    FAMILY = AF_INET,
    PORT = DEFAULT_PORT,
    AF = AF_INET,
    TYPE = SOCK_STREAM,
};

enum class UDPClientDefaultSocketConfiguration
{
    FAMILY = AF_INET,
    PORT = DEFAULT_PORT,
    AF = AF_INET,
    TYPE = SOCK_DGRAM,
};

class BSocket
{
public:
    BSocket();
    BSocket(const int af, const int type, const int protocol);
    BSocket(const int af, const int type, const int protocol, const unsigned long addr, const int family, const unsigned short port, const int backlog);
    BSocket(const int af, const int type, const int protocol, const int family, const unsigned short port, const std::string &serverIP);
    BSocket(const DefaultSocketConfigurations dsc, const unsigned short port);
    BSocket(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string &serverIP);
    BSocket(const SocketInfo &si);
    BSocket(const BSocket &bSocket);
    virtual ~BSocket();

    void SetBSocket(const int af, const int type, const int protocol);
    void SetBSocket(const int af, const int type, const int protocol, const unsigned long addr, const int family, const unsigned short port, const int backlog);
    void SetBSocket(const int af, const int type, const int protocol, const int family, const unsigned short port, const std::string &serverIP);
    void SetBSocket(const DefaultSocketConfigurations dsc, const unsigned short port);
    void SetBSocket(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string &serverIP);
    void SetBSocket(const SocketInfo &si);
    void SetBSocket(const BSocket &bSocket);
    void SetSocket(const SOCKET socket);
    void SetSocketInfo(const SocketInfo &si);

    SOCKET *GetSocket();
    SocketInfo *GetSocketInfo();

    bool EvolveToServer(const unsigned long addr, const int family, const unsigned short port, const int backlog);
    bool EvolveToServer(const DefaultSocketConfigurations dsc, const unsigned short port);
    bool EvolveToClient(const int family, const unsigned short port, const std::string &serverIP);
    bool EvolveToClient(const DefaultSocketConfigurations dsc, const unsigned short port, const std::string &serverIP);

    bool IsEvolved() const;
    bool IsServer() const;
    bool IsClient() const;

    bool friend operator==(const BSocket &lbs, const BSocket &rbs)
    {
        return lbs.isClient == rbs.isClient &&
               lbs.isServer == rbs.isServer &&
               lbs.socket == rbs.socket &&
               lbs.si.af == rbs.si.af &&
               lbs.si.backlog == rbs.si.backlog &&
               lbs.si.protocol == rbs.si.protocol &&
               lbs.si.sin.sin_addr.s_addr == rbs.si.sin.sin_addr.s_addr &&
               lbs.si.sin.sin_family == rbs.si.sin.sin_family &&
               lbs.si.sin.sin_port == rbs.si.sin.sin_port &&
               // lbs.si.sin.sin_zero == rbs.si.sin.sin_zero && // Seems like this is useless because is a padding, to make it same size as struct sockaddr
               lbs.si.type == rbs.si.type;
    }

private:
    bool isServer = false;
    bool isClient = false;

protected:
    SOCKET socket = 0;
    SocketInfo si;
};

#endif // _BSOCKET_H