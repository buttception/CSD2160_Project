#include <vector>
#include <array>

#include "tank.h"
#include "main.h"
#include "../../CatNet/ServerNetwork.h"
#include "process_packet.h"
#include "packets\packets_c2s.h"
#include "packets\packets_s2c.h"
#include "send_packet.h"
#include "HGEKeyCodes.h"

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
			ReceivedPacketProcess_EnterGame(ToProcessSession);
		}
		break;
		case PACKET_ID_C2S_TANKMOVEMENT:
		{
			ReceivedPacketProcess_TankMovement(ToProcessSession);
		}
		break;
		case PACKET_ID_C2S_TANKTURRET:
		{
			ReceivedPacketProcess_TankTurret(ToProcessSession);
		}
		break;

		// TODO: Handle client leaving 
	}
}

void ReceivedPacketProcess_EnterGame(CatNet::ProcessSession* ToProcessSession)
{
	int client_id = ToProcessSession->m_SessionIndex;
	PKT_C2S_EnterGame data;

	ToProcessSession->m_PacketMessage >> data;
	g_Tanks[client_id].connected = true;
	g_Tanks[client_id].client_id = client_id;
	g_Tanks[client_id].x = data.x;
	g_Tanks[client_id].y = data.y;

	// Send Ack.
	CatNet::PacketMessage EnterGameAckPacket;
	int EnterGameAckPacketID = PACKET_ID_S2C_ENTERGAMEOK;
	EnterGameAckPacket << EnterGameAckPacketID;
	//SendPacketProcess_FullGame(client_id);
	NetObj.SendPacket(client_id, EnterGameAckPacket);
}

void ReceivedPacketProcess_TankMovement(CatNet::ProcessSession* ToProcessSession)
{
	int client_id = ToProcessSession->m_SessionIndex;
	PKT_C2S_TankMovement data;
	ToProcessSession->m_PacketMessage >> data;

	// Store packet sequence ID.
	g_Tanks[client_id].input_queue.push({data.sequence_id, data.rotate, data.throttle, data.frameTime});

	// Rotate tank.
	//if (data.rotate == -1)
	//{
	//	g_Tanks[client_id].angular_velocity = -TANK_ROT_SPEED;
	//}
	//else if (data.rotate == 1)
	//{
	//	g_Tanks[client_id].angular_velocity = TANK_ROT_SPEED;
	//}
	//else
	//{
	//	g_Tanks[client_id].angular_velocity = 0.f;
	//}

	//// Translate tank.
	//if (data.throttle == -1)
	//{
	//	g_Tanks[client_id].velocity_x = -cos(g_Tanks[client_id].w) * TANK_MOV_SPEED;
	//	g_Tanks[client_id].velocity_y = -sin(g_Tanks[client_id].w) * TANK_MOV_SPEED;
	//}
	//else if (data.throttle == 1)
	//{
	//	g_Tanks[client_id].velocity_x = cos(g_Tanks[client_id].w) * TANK_MOV_SPEED;
	//	g_Tanks[client_id].velocity_y = sin(g_Tanks[client_id].w) * TANK_MOV_SPEED;
	//}
	//else
	//{
	//	g_Tanks[client_id].velocity_x = g_Tanks[client_id].velocity_y = 0.f;
	//}

	// right now the assumption is that the server can receive, and process client inputs
	// before the client can send a second packet.

	// once change to queuing client inputs, i'll need to change the sequence processing
	// to a queue of sequence numbers as well.
}

void ReceivedPacketProcess_TankTurret(CatNet::ProcessSession* ToProcessSession)
{
	int client_id = ToProcessSession->m_SessionIndex;
	PKT_C2S_TankTurret data;
	ToProcessSession->m_PacketMessage >> data;

	// Store packet sequence ID.
	g_Tanks[client_id].turret_sequence_IDs.push(data.sequence_id);

	// TODO: Rotate turret.

}
