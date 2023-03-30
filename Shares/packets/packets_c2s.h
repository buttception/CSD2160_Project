#ifndef _PACKETS_C2S_H
#define _PACKETS_C2S_H

enum _PACKET_ID_CLIENT_TO_SERVER
{
	PACKET_ID_C2S_EMPTY = 0,	// Start of the list. Do not remove this.

	PACKET_ID_C2S_ENTERGAME,	// Client entering server.
	PACKET_ID_C2S_TANKMOVEMENT,	// Client moving tank.
	PACKET_ID_C2S_TANKTURRET,	// Client rotating turret.
	PACKET_ID_C2S_DISCONNECT,   // Client disconnect
	PACKET_ID_C2S_CLICKSTART,   // Client clicks start in main menu

	PACKET_ID_C2S_END			// This is the indicator for the end of the list. Do not put any packet ID after this.
};


#pragma pack(push, 1) // Pack the memory alignment.

//-- Base Structures
struct PKT_C2S_BASE
{
	int8_t user_id;
};

struct PKT_C2S_INPUTBASE : PKT_C2S_BASE
{
	int sequence_id;	// Sequence number of the packet that is incremented every time a packet is sent.
	float frameTime;	// Frametime of the frame when the packet was sent.
};
//-- END Base Structures



//-- Messages
struct PKT_C2S_EnterGame : PKT_C2S_BASE
{
	float x, y;
	float sprite_size_x, sprite_size_y;

	int64_t pad1;
};

struct PKT_C2S_TankMovement : PKT_C2S_INPUTBASE // 28 bytes
{
	int8_t rotate, throttle;
};

struct PKT_C2S_TankTurret : PKT_C2S_INPUTBASE
{
	float angle;			// Angle in degrees of turret's orientation.
	bool missile_shot;
};

struct PKT_C2S_Disconnect : PKT_C2S_BASE
{
};

struct PKT_C2S_ClickStart : PKT_C2S_BASE
{
	bool active;
};
//-- END Messages

#pragma pack(pop) // Return back to default memory alignment.

#endif
