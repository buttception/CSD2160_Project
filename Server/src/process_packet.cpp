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
	NetObj.SendPacket(client_id, EnterGameAckPacket);

#ifdef _DEBUG
	log("\nSend [PACKET_ID_S2C_ENTERGAMEOK] TO:%d", client_id);
#endif


	// Send info about older clients.
	for (auto& enemy : g_Tanks)
	{
		if (!enemy.connected || client_id == enemy.client_id)
			continue;

		CatNet::PacketMessage Packet;       // Create the _PacketMessage structure object.
		PKT_S2C_ClientPos PacketData;
		int PacketID = PACKET_ID_S2C_OLD_CLIENT;
		PacketData.client_id = enemy.client_id;
		PacketData.x = enemy.x;
		PacketData.y = enemy.y;

		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(client_id, Packet);

#ifdef _DEBUG
		log("\nSend [PACKET_ID_S2C_OLD_CLIENT] TO:%d DATA-ID:%d", client_id, PacketData.client_id);
#endif
	}
}

void ReceivedPacketProcess_TankMovement(CatNet::ProcessSession* ToProcessSession)
{
	int client_id = ToProcessSession->m_SessionIndex;
	PKT_C2S_TankMovement data;
	ToProcessSession->m_PacketMessage >> data;

	// Store packet sequence ID.
	g_Tanks[client_id].input_queue.push({data.sequence_id, data.rotate, data.throttle, data.frameTime});
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
