#include "ServerNetwork.h"
#include <packets/packets_c2s.h>


namespace CatNet
{
    int c2sSize(int id)
    {
        switch (id)
        {
        case PACKET_ID_C2S_EMPTY:
            return 0;
            break;
        case PACKET_ID_C2S_ENTERGAME:
            return sizeof(PKT_C2S_EnterGame);
            break;
        case PACKET_ID_C2S_TANKMOVEMENT:
            return sizeof(PKT_C2S_TankMovement);
            break;
        case PACKET_ID_C2S_TANKTURRET:
            return sizeof(PKT_C2S_TankTurret);
            break;
        case PACKET_ID_C2S_QUIT:
            return 0;
            break;
        case PACKET_ID_C2S_END:
            return 0;
            break;
        }

        return -1;
    }
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
            int readyCount = select(0, &readySockets, nullptr, nullptr, nullptr);
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
                    
                    int index = -1;
                    for(int j = 0; i < MAX_CONNECTION+1; j++)
                    {
	                    if(!server->GetSessionList()->CheckIndex(j))
	                    {
                            index = j;
                            break;
	                    }
                    }
                    if(index > -1)
                    {
                        SessionNode* newClient = new SessionNode();
                        newClient->SetNewConnectedSession(client, server->serverAddr);
                        newClient->SetIndex(index);
                        server->GetSessionList()->AddSession(newClient);
                        newClient->SetRecvBufferWritePos(recv(socket, newClient->GetRecvBuffer(), RECV_BUFSIZE, 0));
                        server->GetProcessList()->Attach(newClient, SESSION_STATE_NEWCONNECTION, newClient->GetRecvBufferWritePos(), newClient->GetRecvBuffer());
                    }
                        
                }
                else
                {
                    auto currClient = server->GetSessionList()->GetSessionNodeBySocket(socket);
                    char m_RecvBuf[RECV_BUFSIZE];
                    currClient->ClearRecvBuffer();
                    int length = recv(socket, m_RecvBuf, RECV_BUFSIZE, 0);
                    int offset = 0;
                    if (length > 0)
                    {
                        while (offset < length)
                        {
                            int id = *reinterpret_cast<int*>(m_RecvBuf + offset);
                            if(id == PACKET_ID_C2S_QUIT)
                            {
                                server->GetSessionList()->RemoveSession(server->GetSessionList()->GetSessionIndexBySocket(socket));
                                currClient->CloseSession();
                                FD_CLR(socket, &connectedSockets);
                                break;
                            }
                            int dataSize = c2sSize(id);
                            currClient->ClearRecvBuffer();
                            memcpy(currClient->GetRecvBuffer(), m_RecvBuf + offset, dataSize + 4);
                            server->GetProcessList()->Attach(currClient, SESSION_STATE_READPACKET, dataSize + 4, currClient->GetRecvBuffer());
                            offset += dataSize + 4;
                        }
                        _strset_s(m_RecvBuf, '\0');
                    }

                    
                   /* currClient->SetRecvBufferWritePos(recv(socket, currClient->GetRecvBuffer(), SEND_BUFSIZE, 0));
                    int hi = WSAGetLastError();
                    if (currClient->GetRecvBufferWritePos() > 0)
                    {
                        server->GetProcessList()->Attach(currClient, SESSION_STATE_READPACKET, currClient->GetRecvBufferWritePos(), currClient->GetRecvBuffer());
                    }
                    else if(currClient->GetRecvBufferWritePos() == 0)
                    {

                        server->GetSessionList()->RemoveSession(server->GetSessionList()->GetSessionIndexBySocket(socket));
                        currClient->CloseSession();
                        FD_CLR(socket, &connectedSockets);
                    }*/
                    
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
        return m_SessionList.GetSessionCount();
    };
    int ServerNetwork::CloseSession(SessionNode* m_SessionNode)
    {
        m_SessionNode->CloseSession();
        m_SessionList.RemoveSession(m_SessionNode->GetIndex());
        return  m_SessionNode->GetSocket();
    }

    SOCKET ServerNetwork::GetMainSocket() { return NetLib::GetMainSocket(); }
    SessionList* ServerNetwork::GetSessionList() { return &m_SessionList; }
    ProcessSessionList* ServerNetwork::GetProcessList() { return &m_ToProcessList; }

    int ServerNetwork::SendPacket(int SessionIndex, struct PacketMessage packet_message)
    {
        if (SessionIndex < 0)
            return -1;

        auto& client = m_SessionList.GetSessionNodeByIndex(SessionIndex);
        if (!client)
            return -1;

       // PacketMessage encoded{};
        //encoded << "Socket=" << std::to_string(client->GetSocket()).c_str() << " Message=" << packet_message.Buffer;
        return NetLib::SendPacket(client->GetSocket(), packet_message);
    }
    int ServerNetwork::SendPacketToAll(PacketMessage packet_message)
    {
        for(int i = 0; i < MAX_CONNECTION +1; i++)
        {
	        if(m_SessionList.CheckIndex(i))
	        {
               // PacketMessage encoded{};
               // encoded << "Socket=" << std::to_string(m_SessionList.GetSessionNodeByIndex(i)->GetSocket()).c_str() << " Message=" << packet_message.Buffer;
                NetLib::SendPacket(m_SessionList.GetSessionNodeByIndex(i)->GetSocket(), packet_message);
	        }
        }
        return m_SessionList.GetSessionCount();
    }
    int ServerNetwork::SendPacketToAllExcept(PacketMessage packet_message, int SessionIndex)
    {
        for (int i = 0; i < MAX_CONNECTION + 1; i++)
        {
            if (m_SessionList.CheckIndex(i) && i != SessionIndex)
            {
               // PacketMessage encoded{};
               // encoded << "Socket=" << std::to_string(m_SessionList.GetSessionNodeByIndex(i)->GetSocket()).c_str() << " Message=" << packet_message.Buffer;
                NetLib::SendPacket(m_SessionList.GetSessionNodeByIndex(i)->GetSocket(), packet_message);
            }
        }
        return m_SessionList.GetSessionCount();
    }

    int ServerNetwork::CloseSessionByIndex(int SessionIndex)
    {
        return CloseSession(m_SessionList.GetSessionNodeByIndex(SessionIndex));
    }
}