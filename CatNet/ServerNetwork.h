///////////////////////////////////////////////////////////////////////////////
// ** CAUSION **                                                             //
// YOU SHOULD NOT TOUCH ANYTHING INSIDE THIS HEADER FILE.                    //
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERVERNETWORK_H
#define _SERVERNETWORK_H
#pragma once

#include "NetworkLib.h"
#include <iostream>
#include <string>

//-------------------------
// Class for server network


namespace CatNet
{
    DWORD WINAPI ServerRecvThread(void* arg);

    class ServerNetwork : public NetLib
    {
        private:
            SessionList m_SessionList;
            struct ProcessSessionList m_ToProcessList;

        public:
            ServerNetwork() = default;
            ~ServerNetwork() = default;

        private:
            int CloseSessionByIndex(int SessionIndex);

        public:
            sockaddr_in serverAddr{ };
            bool InitNet(int AppType, int ProtocolType, int PortNumber);
            int GetConnectedCount(void);
            int CloseSession(SessionNode* m_SessionNode);

            SOCKET GetMainSocket();
            SessionList* GetSessionList();
            ProcessSessionList* GetProcessList();

            int SendPacket(int SessionIndex, struct PacketMessage packet_message);
            int SendPacketToAll(PacketMessage packet_message);
            int SendPacketToAllExcept(PacketMessage packet_message, int SessionIndex);
    };
}



#endif
