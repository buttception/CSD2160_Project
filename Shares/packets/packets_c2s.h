#ifndef _PACKETS_C2S_H
#define _PACKETS_C2S_H

enum _PACKET_ID_CLIENT_TO_SERVER {
    PACKET_ID_C2S_EMPTY = 0,            // Start of the list. Do not remove this.

    PACKET_ID_C2S_ENTERGAME,
    PACKET_ID_C2S_MOVEMENT,
    PACKET_ID_C2S_COLLIDED,
	PACKET_ID_C2S_ASTEROIDCOLLIDED,
	PACKET_ID_C2S_NEWMISSILE,
	PACKET_ID_C2S_DELETEMISSILE,
	PACKET_ID_C2S_NEWBOOM,
	PACKET_ID_C2S_HITSHIP,
	PACKET_ID_C2S_RESPAWN,
	PACKET_ID_C2S_NEWMINE,
	PACKET_ID_C2S_DELETEMINE,
	PACKET_ID_C2S_NEWTIMEBOMB,
	PACKET_ID_C2S_DELETETIMEBOMB,
	PACKET_ID_C2S_DELETESPEEDUP,

    PACKET_ID_C2S_END                   // This is the indicator for the end of the list. Do not put any packet ID after this.
};


/**
* This is the list of data structures for each packet.
* ** IMPORTANT ** All the structures have to be fixed size.
*/
#pragma pack(push, 1) // Pack the memory alignment. 
struct PKT_C2S_EnterGame {
    int ShipID;
    int ShipType;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_C2S_Movement {
    int ShipID;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_C2S_Collided {
    int ShipID;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_C2S_AsteroidCollided {
	int AsteroidID;
	float x;
	float y;
	float velocity_x;
	float velocity_y;
	float server_velocity_x;
	float server_velocity_y;
	float angular_velocity;
};

struct PKT_C2S_NewMissile {
	int OwnerShipID;
	float x;
	float y;
	float w;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

struct PKT_C2S_DeleteMissile {
	int OwnerShipID;
};

struct PKT_C2S_UpdateMissile {
};

struct PKT_C2S_NewBoom {
	float x;
	float y;
	float w;
};

struct PKT_C2S_HitShip {
	int ShipHitID;
	float x;
	float y;
};

struct PKT_C2S_Respawn {
	int ShipID;
};

struct PKT_C2S_NewMine {
	float x;
	float y;
	int OwnerID;
};

struct PKT_C2S_DeleteMine {
	int ownerId;
};

struct PKT_C2S_NewTimeBomb {
	int ownerId;
	float x;
	float y;
};

struct PKT_C2S_DeleteTimeBomb {
	int ownerId;
};

struct PKT_C2S_DeleteSpeedUp {
	int SpeedUpID;
};


#pragma pack(pop) // Return back to default memory alignment.


#endif