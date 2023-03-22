#include <vector>
#include <array>

#include "tank.h"
#include "main.h"
#include "../../CatNet/ServerNetwork.h"
#include "process_packet.h"
#include "packets\packets_c2s.h"
#include "packets\packets_s2c.h"
#include "send_packet.h"

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif

extern CatNet::ServerNetwork NetObj;
extern std::array<Tank, MAX_CLIENT_CONNECTION + 1> g_Tanks;

void ReceivedPacketProcess( struct CatNet::ProcessSession *ToProcessSession )
{
    int PacketID;

    ToProcessSession->m_PacketMessage >> PacketID;
    switch( PacketID )
    {
        case PACKET_ID_C2S_ENTERGAME:
        {
            ReceivedPacketProcess_EnterGame( ToProcessSession );
        }
        break;

        case PACKET_ID_C2S_MOVEMENT:
        {
            ReceivedPacketProcess_Movement( ToProcessSession );
        }
        break;

        //handle client leaving 
    }
}

void ReceivedPacketProcess_EnterGame( struct CatNet::ProcessSession *ToProcessSession )
{
    int client_id = ToProcessSession->m_SessionIndex;
    PKT_C2S_EnterGame Data;

    ToProcessSession->m_PacketMessage >> Data;
    g_Tanks[client_id].connected = true;
    g_Tanks[client_id].x = Data.x;
    g_Tanks[client_id].y = Data.y;

#ifdef _DEBUG
    /*log( "\nRecv PACKET_ID_C2S_ENTERGAME Conn:%d, ID:%d, Type:%d, x:%0.0f, y:%0.0f, w:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
         g_ShipList[client_id].connected, client_id, g_ShipList[client_id].ship_type, g_ShipList[client_id].x, g_ShipList[client_id].y, g_ShipList[client_id].w,
         g_ShipList[client_id].velocity_x, g_ShipList[client_id].velocity_y, g_ShipList[client_id].angular_velocity );*/
#endif

    // Send Ack.
    CatNet::PacketMessage EnterGameAckPacket;
    int EnterGameAckPacketID = PACKET_ID_S2C_ENTERGAMEOK;
    EnterGameAckPacket << EnterGameAckPacketID;
	//SendPacketProcess_FullGame(client_id);
    NetObj.SendPacket(client_id, EnterGameAckPacket );
}

void ReceivedPacketProcess_Movement( struct CatNet::ProcessSession *ToProcessSession )
{
    int client_id = ToProcessSession->m_SessionIndex;
    PKT_C2S_Movement movement_data;
    ToProcessSession->m_PacketMessage >> movement_data;

#define ROT_SPEED 1.f;
#define THROTTLE_SPEED 100.f;

    if (movement_data.rotate == -1)
    {
	    g_Tanks[client_id].angular_velocity = -ROT_SPEED;
        std::cout << "\nturn left";
    }
    else if (movement_data.rotate == 1)
    {
	    g_Tanks[client_id].angular_velocity = ROT_SPEED;
        std::cout << "\nturn right";
    }
    else
    {
	    g_Tanks[client_id].angular_velocity = 0.f;
    }

    if (movement_data.throttle == -1)
    {
        std::cout << "\ngo backwards";
    }
    else if (movement_data.throttle == 1)
    {
        std::cout << "\ngo forward";
    }
    else
    {
        g_Tanks[client_id].velocity_x = g_Tanks[client_id].velocity_y = 0.f;
    }
}
