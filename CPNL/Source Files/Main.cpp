#include <CPNL.h>
#include <chrono>

#include <thread>
#include <vector>

#define PASSWORD "sarmale1"
#define BUG(string)                             \
    for (auto &c : string)                      \
        std::cout << c << "-" << (int)c << " "; \
    std::cout << std::endl;

BServer server(DefaultSocketConfigurations::TCP, 32406);
bool closeClientThreads = false;

std::vector<std::pair<BSocket, std::vector<BSocket>>> roots;

void FindAllOccurrences(const std::string &data, const std::string &query, std::vector<size_t> &occurancesPoss)
{
    size_t pos = data.find(query);
    while (pos != std::string::npos)
    {
        occurancesPoss.push_back(pos);
        pos = data.find(query, pos + query.size());
    }
}

void Split(const std::string &data, const std::string &delimiter, std::vector<std::string> &dest)
{
    std::vector<size_t> occurrences;
    FindAllOccurrences(data, delimiter, occurrences);

    if (occurrences.empty())
    {
        dest.push_back(data);
        return;
    }

    size_t last = 0;
    for (size_t current = 0; current < occurrences.size(); current++)
    {
        std::string pushable(data.substr(last, occurrences[current] - last));
        if (!pushable.empty())
        {
            dest.push_back(pushable);
        }

        last = occurrences[current] + delimiter.size();
    }

    std::string pushable(data.substr(last));
    if (!pushable.empty())
    {
        dest.push_back(pushable);
    }
}

inline int8_t GetIndexOfRoots(BSocket &bSocket)
{
    size_t index = 0;
    for (auto item = roots.begin(); item != roots.end(); item++)
    {
        if (item->first == bSocket)
        {
            break;
        }

        index++;
    }

    return index;
}

inline std::string inet_ntoa_s(struct in_addr __in)
{
    uint32_t ip = __in.s_addr;

    std::string hl = std::to_string(ip & 0xFF);
    ip >>= 8;
    std::string hr = std::to_string(ip & 0xFF);
    ip >>= 8;
    std::string ll = std::to_string(ip & 0xFF);
    ip >>= 8;
    std::string lr = std::to_string(ip & 0xFF);

    return hl + '.' + hr + '.' + ll + '.' + lr;
}

std::string ExecuteCommand(BSocket bSocket, const std::string &command)
{
    std::vector<std::string> args;
    Split(command, " ", args);

    std::string output;
    if (args[0] == "list")
    {
        for (auto client : *server.GetClients())
        {
            output += inet_ntoa_s(client->GetSocketInfo()->sin.sin_addr) + '\n';
        }
        if (!output.empty())
        {
            output.pop_back();
        }
        else
        {
            output = "There are no clients connected to the server.";
        }
    }
    else if (args[0] == "listc")
    {
        for (auto &client : roots[GetIndexOfRoots(bSocket)].second)
        {
            output += inet_ntoa_s(client.GetSocketInfo()->sin.sin_addr) + '\n';
        }
        if (!output.empty())
        {
            output.pop_back();
        }
        else
        {
            output = "There are no clients connected with you.";
        }
    }
    else if (args[0] == "connect")
    {
        if (args.size() == 1)
        {
            output = "There is no ipv4 to connect to...";
            return output;
        }

        bool exists = false;
        for (auto client : *server.GetClients())
        {
            if (args[1] == inet_ntoa_s(client->GetSocketInfo()->sin.sin_addr))
            {
                roots[GetIndexOfRoots(bSocket)].second.push_back(*client);
                exists = true;
                break;
            }
        }

        if (!exists)
        {
            output = "There is no client with the ipv4 " + args[1];
        }
        else
        {
            output = "You have been connected with the client with the ipv4 " + args[1];
        }
    }
    else if (args[0] == "disconnect")
    {
        if (args.size() == 1)
        {
            output = "There is no ipv4 to disconnect to...";
            return output;
        }

        size_t pos = 0;
        bool exists = false;
        for (auto client = roots[GetIndexOfRoots(bSocket)].second.begin(); client != roots[GetIndexOfRoots(bSocket)].second.end(); client++, pos++)
        {
            if (args[1] == inet_ntoa_s((*client).GetSocketInfo()->sin.sin_addr))
            {
                roots[GetIndexOfRoots(bSocket)].second.erase(roots[GetIndexOfRoots(bSocket)].second.begin() + pos);
                exists = true;
                break;
            }
        }

        if (!exists)
        {
            output = "There is no client with the ipv4 of " + args[1];
        }
        else
        {
            output = "You have been disconnected from the client with the ipv4 of " + args[1];
        }
    }
    else if (args[0] == "message")
    {
        if (roots[GetIndexOfRoots(bSocket)].second.empty())
        {
            output = "There are no clients to send the message to...";
            return output;
        }

        if (args.size() == 1)
        {
            output = "There is no message...";
            return output;
        }

        for (auto &client : roots[GetIndexOfRoots(bSocket)].second)
        {
            server.SendData(client, command.substr(8));
            output += std::string(inet_ntoa_s(client.GetSocketInfo()->sin.sin_addr)) + ":\n\n" + server.ReceiveData(client, 1024) + "\n\n\n\n";
        }
    }
    else
    {
        output = "Could not recognize command...";
    }

    return output;
}

void NewRoot(BSocket bSocket)
{
    while (!closeClientThreads)
    {
        std::string receivedData = server.ReceiveData(bSocket);
        if (!receivedData.empty())
        {
            receivedData.pop_back(); // Linux shit new line
        }
        if (receivedData == "")
        {
            roots.erase(roots.begin() + GetIndexOfRoots(bSocket));
            break;
        }

        BUG(receivedData);
        std::cout << "Income: " << receivedData << std::endl;
        std::string output = ExecuteCommand(bSocket, receivedData) + '\n';
        std::cout << "Outcome: " << output << std::endl;
        server.SendData(bSocket, output);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void NewClient(BSocket bSocket)
{
    while (!closeClientThreads)
    {
        std::string receivedData = server.ReceiveData(bSocket);
        if (receivedData == "")
        {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main()
{
    server.Listen();

    BSocket newBSocket;
    while (true)
    {
        bool connectedSuccessfully = server.Accept(newBSocket);
        if (!connectedSuccessfully)
        {
            continue;
        }

        uint8_t howManyTimesIsOneIPConnected = 0;
        for (auto &client : *server.GetClients())
        {
            if (newBSocket.GetSocketInfo()->sin.sin_addr.s_addr == client->GetSocketInfo()->sin.sin_addr.s_addr)
            {
                if (++howManyTimesIsOneIPConnected > 1)
                {
                    connectedSuccessfully = false;
                    server.DisconnectSocketWithoutEcho(*newBSocket.GetSocket());

                    break;
                }
            }
        }

        if (connectedSuccessfully)
        {
            server.SendData(newBSocket, "Give me a password:\n");

            std::string rd = server.ReceiveData(newBSocket);
            if (!rd.empty())
            {
                rd.pop_back(); // Linux shit new line
            }

            if (rd == PASSWORD)
            {
                roots.push_back(std::pair<BSocket, std::vector<BSocket>>(newBSocket, std::vector<BSocket>()));
                server.GetClients()->pop_back();

                std::thread newRootThread(NewRoot, newBSocket);
                newRootThread.detach();
            }
            else
            {
                std::thread newClientThread(NewClient, newBSocket);
                newClientThread.detach();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    closeClientThreads = true;

#ifdef _WIN32
    WSACleanup();
#endif
    return EXIT_SUCCESS;
}

/*
    Look at SendData and ReceiveData
    poate de disc de la cineva ce nu Exista
    poate connect de 2 ori la acelasi om
    mesaje mai bune
    la comanda gresita se blocheaza ca asteapta mesaj (poate nu e plin bufferul ca sa-l trimita)
*/