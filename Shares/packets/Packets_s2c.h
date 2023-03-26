#ifndef _PACKETS_S2C_H
#define _PACKETS_S2C_H
#pragma once


enum _PACKET_ID_SERVER_TO_CLIENT
{
	PACKET_ID_S2C_EMPTY = 0,		// Start of the list. Do not remove this.

	PACKET_ID_S2C_WELCOMEMESSAGE,
	PACKET_ID_S2C_FULLGAME,
	PACKET_ID_S2C_ENTERGAMEOK,

	PACKET_ID_S2C_NEW_CLIENT,
	PACKET_ID_S2C_OLD_CLIENT,
	PACKET_ID_S2C_DISCONNECT_CLIENT,

	PACKET_ID_S2C_TANKMOVEMENT,
	PACKET_ID_S2C_TANKTURRET,
	PACKET_ID_S2C_MISSILE,

	PACKET_ID_S2C_END
};

#pragma pack(push, 1) // Pack the memory alignment. 

//-- Base Structures
struct PKT_S2C_BASE
{
	int client_id;	// client_id is same as SessionID in the server.
};
//-- END Base Structures


//-- Messages
struct PKT_S2C_WelcomeMessage : PKT_S2C_BASE
{

};

struct PKT_S2C_FullGame : PKT_S2C_BASE
{

};

struct PKT_S2C_ClientPos : PKT_S2C_BASE
{
	float x, y;
};

struct PKT_S2C_Disconnect : PKT_S2C_BASE
{

};

struct PKT_S2C_TankMovement : PKT_S2C_BASE
{
	int sequence_id;

	float x, y, w;
	float vx, vy;
};

struct PKT_S2C_TankTurret : PKT_S2C_BASE
{
	int sequence_id;

	float angle;
};

struct PKT_S2C_Missile : PKT_S2C_BASE
{
	int sequence_id;

	float x, y, w;
	float vx, vy;
};
//-- END Messages

#pragma pack(pop) // Return back to default memory alignment.

#endif
