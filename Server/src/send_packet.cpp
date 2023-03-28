#include "main.h"
#include "send_packet.h"

#include <array>

#include "tank.h"

#ifdef _DEBUG
extern void log(char* szFormat, ...);
#endif

extern CatNet::ServerNetwork NetObj;
extern std::array<Tank, MAX_CLIENT_CONNECTION + 1> g_Tanks;

void SendPacketProcess_NewAccept(const int& sessionID)
{
	// Add into tank list.
	g_Tanks[sessionID].connected = true;

	// Send welcome message.
	SendPacketProcess_NewAccept_SendWelcomeMessage(sessionID);

	// Send new client to all other clients.
	CatNet::PacketMessage Packet;       // Create the _PacketMessage structure object.
	PKT_S2C_ClientPos PacketData;
	int PacketID = PACKET_ID_S2C_NEW_CLIENT;
	PacketData.client_id = sessionID;
	PacketData.x = g_Tanks[sessionID].x;
	PacketData.y = g_Tanks[sessionID].y;

	Packet << PacketID;
	Packet << PacketData;
	NetObj.SendPacketToAll(Packet);

#ifdef _DEBUG
	log("\nSend [PACKET_ID_S2C_NEW_CLIENT] TO:%d DATA-ID:%d", sessionID, PacketData.client_id);
#endif
}

void SendPacketProcess_FullGame(const int& sessionID)
{
	CatNet::PacketMessage Packet;
	PKT_S2C_FullGame PacketData;
	int PacketID = PACKET_ID_S2C_FULLGAME;
	PacketData.client_id = sessionID;

	Packet << PacketID;
	Packet << PacketData;
	NetObj.SendPacket(sessionID, Packet);
}

void SendPacketProcess_NewAccept_SendWelcomeMessage(const int& sessionID)
{
	// Send Welcome packet with ID.
	CatNet::PacketMessage Packet;       // Create the _PacketMessage structure object.
	PKT_S2C_WelcomeMessage PacketData;
	int PacketID = PACKET_ID_S2C_WELCOMEMESSAGE;
	PacketData.client_id = sessionID;

	Packet << PacketID;
	Packet << PacketData;
	NetObj.SendPacket(sessionID, Packet);

#ifdef _DEBUG
	log("\nSend [PACKET_ID_S2C_WELCOMEMESSAGE] TO:%d DATA-ID:%d", sessionID, PacketData.client_id);
#endif
}

void SendPacketProcess_Disconnect(const int& sessionID)
{
	//g_ShipList[sessionID].connected = false;

	struct CatNet::PacketMessage Packet;
	int PacketID = PACKET_ID_S2C_DISCONNECT_CLIENT;
	Packet << PacketID;
	Packet << sessionID;
	NetObj.SendPacketToAll(Packet);
	//for (int i = 1; i <= MAX_CLIENT_CONNECTION; ++i)
	//{
	//	if (true == g_Tanks[i].connected)
	//	{
	//		if (sessionID == i) continue;

	//		NetObj.SendPacket(i, Packet);
	//	}
	//}
}

void SendPacketProcess_TankMovement(const Tank& tank)
{
	CatNet::PacketMessage movement_update_packet;
	int id = PACKET_ID_S2C_TANKMOVEMENT;
	movement_update_packet << id;

	PKT_S2C_TankMovement movement;
	movement.sequence_id = tank.latest_sequence_ID;
	movement.client_id = tank.client_id;
	movement.x = tank.x;
	movement.y = tank.y;
	movement.w = tank.w;
	movement.vx = tank.velocity_x;
	movement.vy = tank.velocity_y;
	movement_update_packet << movement;
	NetObj.SendPacketToAll(movement_update_packet);
}

void SendPacketProcess_TankTurret(const Tank& tank)
{
	CatNet::PacketMessage turret_update_packet;
	int id = PACKET_ID_S2C_TANKTURRET;
	turret_update_packet << id;

	PKT_S2C_TankTurret turret;
	turret.sequence_id = tank.latest_turret_seq_ID;
	turret.client_id = tank.client_id;
	turret.angle = tank.turret_rotation;
	turret_update_packet << turret;

	NetObj.SendPacketToAll(turret_update_packet);
}

void SendPacketProcess_Missile(const Missile& missile)
{
	CatNet::PacketMessage movement_update_packet;
	int id = PACKET_ID_S2C_MISSILE;
	movement_update_packet << id;

	PKT_S2C_Missile movement;
	movement.alive = missile.alive;
	movement.missile_id = missile.id;
	movement.client_id = missile.owner_id;
	movement.x = missile.x;
	movement.y = missile.y;
	movement.w = missile.w;
	movement.vx = missile.velocity_x;
	movement.vy = missile.velocity_y;
	movement_update_packet << movement;
	NetObj.SendPacketToAll(movement_update_packet);
}

void SendPacketProcess_TankState(const Tank& tank)
{
	CatNet::PacketMessage movement_update_packet;
	int id = PACKET_ID_S2C_TANKSTATE;
	movement_update_packet << id;

	PKT_S2C_TankState movement;
	movement.client_id = tank.client_id;
	movement.active = tank.active;
	movement_update_packet << movement;
	NetObj.SendPacketToAll(movement_update_packet);
}
