#ifndef _PACKETS_C2S_H
#define _PACKETS_C2S_H

enum _PACKET_ID_CLIENT_TO_SERVER
{
	PACKET_ID_C2S_EMPTY = 0,	// Start of the list. Do not remove this.

	PACKET_ID_C2S_ENTERGAME,	// Client entering server.
	PACKET_ID_C2S_TANKMOVEMENT,	// Client moving tank.
	PACKET_ID_C2S_TANKTURRET,	// Client rotating turret.

	PACKET_ID_C2S_END			// This is the indicator for the end of the list. Do not put any packet ID after this.
};


#pragma pack(push, 1) // Pack the memory alignment.

//-- Base Structures
struct PKT_C2S_BASE
{
	int user_id;
};

struct PKT_C2S_INPUTBASE : PKT_C2S_BASE
{
	int sequence_id;	// Sequence number of the packet that is incremented every time a packet is sent.
	int64_t timestamp;	// Timestamp in seconds since UNIX epoch (01-01-1970 0000hrs)
};
//-- END Base Structures



//-- Messages
struct PKT_C2S_EnterGame : PKT_C2S_BASE
{
	float x, y;
};

struct PKT_C2S_TankMovement : PKT_C2S_INPUTBASE
{
	int rotate, throttle;
};

struct PKT_C2S_TankTurret : PKT_C2S_INPUTBASE
{
	int angle;			// Angle in degrees of turret's orientation.
};
//-- END Messages

#pragma pack(pop) // Return back to default memory alignment.

#endif
