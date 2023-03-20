#include "ServerNetwork.h"


namespace CatNet
{
    DWORD WINAPI ServerRecvThread(void* arg)
    {
        ServerNetwork* server{ static_cast<ServerNetwork*>(arg) };
        fd_set connectedSockets;
        fd_set readySockets;
        FD_ZERO(&connectedSockets);
        FD_SET(server->GetMainSocket(), &connectedSockets);

        while (true)
        {
            readySockets = connectedSockets;
            const int readyCount = select(0, &readySockets, nullptr, nullptr, nullptr);

            for (SOCKET i = 0; i < static_cast<unsigned>(readyCount); ++i)
            {
                SOCKET socket = readySockets.fd_array[i];

                //New client connecting
                if (socket == server->GetMainSocket())
                {
                    // Accept a new connection
                    SOCKET client = accept(server->GetMainSocket(), nullptr, nullptr);

                    // Add the connection to the list of clients
                    FD_SET(client, &connectedSockets);
                    SessionNode* newClient;
                    newClient->SetNewConnectedSession(client, server->serverAddr);
                    server->GetSessionList()->ActiveList.AttachNode(newClient);
                }
                else
                {
                    auto currClient = server->GetSessionList()->ActiveList.GetSessionNodeBySocket(socket);
                    currClient->SetRecvBufferWritePos(recv(socket, currClient->GetRecvBuffer(), SEND_BUFSIZE, 0));
                    if (currClient->GetRecvBufferWritePos() > 0)
                    {
                        server->GetProcessList()->Attach(currClient, SESSION_STATE_READPACKET, currClient->GetRecvBufferWritePos(), currClient->GetRecvBuffer());
                    }
                    else
                    {
                        currClient->CloseSession();
                        server->GetSessionList()->ActiveList.DetachNode(currClient);
                        FD_CLR(socket, &connectedSockets);
                    }
                }
            }
        }

        return 0;
    }

    bool ServerNetwork::InitNet(int AppType, int ProtocolType, int PortNumber)
    {
        if (!InitCatNet(AppType, ProtocolType, PortNumber))
            return false;

        if (GetAppType() != APP_TYPE::APPTYPE_SERVER)
        {
            GetErrorCode() = ERROR_TYPE::CATNET_ERROR_WRONGAPPTYPE;
            return false;
        }

        // Create the structure describing various Server parameters

        char ServerIPAddress[] = "127.0.0.1";
        serverAddr.sin_family = AF_INET;     // The address family. MUST be AF_INET
        serverAddr.sin_addr.s_addr = inet_addr(ServerIPAddress);
        serverAddr.sin_port = htons(PortNumber);

        if (bind(GetMainSocket(), (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            //cout << "Unable to bind to " << szServerIPAddr << " port " << nServerPort << endl;
            // Free the socket and cleanup the environment initialized by WSAStartup()
            closesocket(GetMainSocket());
            WSACleanup();
            return false;
        }

        // Put the Server socket in listen state so that it can wait for client connections
        if (listen(GetMainSocket(), SOMAXCONN) == SOCKET_ERROR)
        {
            //cout << "Unable to put server in listen state" << endl;
            // Free the socket and cleanup the environment initialized by WSAStartup()
            closesocket(GetMainSocket());
            WSACleanup();
            return false;
        }

        GetRecvThreadHandle() = CreateThread(nullptr, 0, ServerRecvThread, this, 0, &GetRecvThreadID());

        return true;
    }
    int ServerNetwork::GetConnectedCount(void)
    {
        return m_SessionList.ActiveList.GetNodeCount();
    };
    int ServerNetwork::CloseSession(SessionNode* m_SessionNode)
    {
        m_SessionNode->CloseSession();
        m_SessionList.ActiveList.DetachNode(m_SessionNode);
    }

    SOCKET ServerNetwork::GetMainSocket() { return GetMainSocket(); }
    SessionList* ServerNetwork::GetSessionList() { return &m_SessionList; }
    ProcessSessionList* ServerNetwork::GetProcessList() { return &m_ToProcessList; }

    int ServerNetwork::SendPacket(int SessionIndex, struct PacketMessage packet_message)
    {
        if (SessionIndex < 0)
            return -1;

        auto& client = m_SessionList.GetSessionNodeByIndex(SessionIndex);
        if (!client)
            return -1;

        PacketMessage encoded{};
        encoded << "Socket=" << std::to_string(client->GetSocket()).c_str() << " Message=" << packet_message.Buffer;
        return NetLib::SendPacket(client->GetSocket(), encoded);
    }
    int ServerNetwork::SendPacketToAll(PacketMessage packet_message)
    {
        SessionNode* curr = m_SessionList.ActiveList.GetHead();
        while (curr != nullptr)
        {
            PacketMessage encoded{};
            encoded << "Socket=" << std::to_string(curr->GetSocket()).c_str() << " Message=" << packet_message.Buffer;
            NetLib::SendPacket(curr->GetSocket(), encoded);
            curr == curr->GetNext();
        }
        return 1;
    }
    int ServerNetwork::SendPacketToAllExcept(PacketMessage packet_message, int SessionIndex)
    {
        SessionNode* curr = m_SessionList.ActiveList.GetHead();
        while (curr != nullptr)
        {
            if (curr->GetIndex() != SessionIndex)
            {
                PacketMessage encoded{};
                encoded << "Socket=" << std::to_string(curr->GetSocket()).c_str() << " Message=" << packet_message.Buffer;
                NetLib::SendPacket(curr->GetSocket(), encoded);
            }

            curr = curr->GetNext();
        }
    }

    int ServerNetwork::CloseSessionByIndex(int SessionIndex)
    {
        return CloseSession(m_SessionList.GetSessionNodeByIndex(SessionIndex));
    }
}