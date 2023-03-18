#ifndef _CLIENTNETWORK_H
#define _CLIENTNETWORK_H

#pragma once

#include "NetworkLib.h"

namespace CatNet
{
    DWORD WINAPI RunClientThread(void* args);
}

namespace CatNet
{
    class ClientNetwork : public NetLib
    {
    private:
        SessionNode m_Session;
        ProcessSessionList m_ToProcessList;

    public:
        ClientNetwork() = default;
        ~ClientNetwork() = default;

    public:
        SOCKET GetSocket() const { return GetMainSocket(); }
        SessionNode* GetSession() { return &m_Session; }
        ProcessSessionList* GetProcessList() { return &m_ToProcessList; }

    public:
        bool InitNet(int AppType, int ProtocolType, char* ServerIpAddress, int PortNumber)
        {
            if (!InitCatNet(AppType, ProtocolType, PortNumber))
                return false;

            if (GetAppType() != APP_TYPE::APPTYPE_CLIENT)
            {
                GetErrorCode() = ERROR_TYPE::CATNET_ERROR_WRONGAPPTYPE;
                return false;
            }

            if (!ConnectToServer(ServerIpAddress, PortNumber))
                return false;
            GetRecvThreadHandle() = CreateThread(nullptr, 0, RunClientThread, this, 0, &GetRecvThreadID());

            m_Session.GetSocket() = GetMainSocket();

            return true;
        }
        int ConnectToServer(char* ServerIPAddress, int PortNumber)
        {
            // Create the structure describing various Server parameters
            sockaddr_in& serverAddr{ m_Session.GetSessionAddr() };

            serverAddr.sin_family = AF_INET;     // The address family. MUST be AF_INET
            serverAddr.sin_addr.s_addr = inet_addr(ServerIPAddress);
            serverAddr.sin_port = htons(PortNumber);
            //serverAddr.sin_zero = 0;

            if (connect(GetMainSocket(), reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
            {
                GetErrorCode() = ERROR_TYPE::CATNET_ERROR_SOCKETFAILED;
                //closesocket(GetMainSocket());
                //WSAGetLastError();
                //WSACleanup();
                return 0;
            }

            return 1;
        }

        int SendPacket(const PacketMessage& PacketMessage)
        {
            return NetLib::SendPacket(GetMainSocket(), PacketMessage);
        }

        int CheckPacketRecevied()
        {
            return m_Session.GetRecvBufferWritePos();
        }

    };
}

namespace CatNet
{
    DWORD WINAPI RunClientThread(void* args)
    {
        ClientNetwork* client{ static_cast<ClientNetwork*>(args) };
        do
        {
            client->GetSession()->ClearRecvBuffer();
            recv(client->GetSocket(), client->GetSession()->GetRecvBuffer(), SEND_BUFSIZE, 0);
            if (client->CheckPacketRecevied() > 0)
                client->GetProcessList()->Attach(client->GetSession(), SESSION_STATE_READPACKET, client->CheckPacketRecevied(), client->GetSession()->GetRecvBuffer());
            else if (0 == client->CheckPacketRecevied())
            {
                printf("Connection closed\n");
                break;
            }
            else
                printf("Recv failed: %d\n", WSAGetLastError());
            break;
        } while (true);
        return 0;
    }
}

#endif
