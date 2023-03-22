#include "main.h"
#include "send_packet.h"

#include <array>

#include "tank.h"

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif

extern CatNet::ServerNetwork NetObj;
extern std::array<Tank, MAX_CLIENT_CONNECTION + 1> g_Tanks;

void SendPacketProcess_NewAccept( const int SessionID )
{
    // Add into tank list.
    g_Tanks[SessionID].connected = true;

    // Send welcome message.
    SendPacketProcess_NewAccept_SendWelcomeMessage( SessionID );
}

void SendPacketProcess_FullGame(const int SessionID)
{
	CatNet::PacketMessage Packet;
	PKT_S2C_FullGame PacketData;
	int PacketID = PACKET_ID_S2C_FULLGAME;
	PacketData.client_id = SessionID;

	Packet << PacketID;
	Packet << PacketData;
	NetObj.SendPacket(SessionID, Packet);
}

void SendPacketProcess_NewAccept_SendWelcomeMessage( const int SessionID )
{
    // Send Welcome packet with ID.
    CatNet::PacketMessage Packet;       // Create the _PacketMessage structure object.
    PKT_S2C_WelcomeMessage PacketData;
    int PacketID = PACKET_ID_S2C_WELCOMEMESSAGE;
    PacketData.client_id = SessionID;

    Packet << PacketID;
    Packet << PacketData;
    NetObj.SendPacket( SessionID, Packet );
#ifdef _DEBUG
    log( "\nSend [PACKET_ID_S2C_WELCOMEMESSAGE] ID:%d", PacketData.client_id );
#endif
}

void SendPacketProcess_Disconnect( const int SessionID )
{
    //g_ShipList[SessionID].connected = false;

    struct CatNet::PacketMessage Packet;
    int PacketID = PACKET_ID_S2C_DISCONNECT_CLIENT;
    Packet << PacketID;
    Packet << SessionID;
    
    for(int i = 1; i <= MAX_CLIENT_CONNECTION; ++i)
    {
        if( true == g_Tanks[i].connected )
        {
            if( SessionID == i) continue;

            NetObj.SendPacket(i, Packet );
#ifdef _DEBUG
            log( "\n Disconnect packet sent to ClientID:%d", i);
#endif
        }
    }
}