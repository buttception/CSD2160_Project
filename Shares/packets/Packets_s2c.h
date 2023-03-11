#ifndef _PACKETS_S2C_H
#define _PACKETS_S2C_H
#pragma once


enum _PACKET_ID_SERVER_TO_CLIENT {
	PACKET_ID_S2C_EMPTY = 0,            // Start of the list. Do not remove this.

	PACKET_ID_S2C_WELCOMEMESSAGE,
	PACKET_ID_S2C_FULLGAME,
	PACKET_ID_S2C_ENTERGAMEOK,
	PACKET_ID_S2C_ENEMYSHIP,
	PACKET_ID_S2C_NEWENEMYSHIP,
	PACKET_ID_S2C_NEWASTEROID,
	PACKET_ID_S2C_DISCONNECTENEMYSHIP,
	PACKET_ID_S2C_MOVEMENT,
	PACKET_ID_S2C_COLLIDED,
	PACKET_ID_S2C_ASTEROIDMOVEMENT,
	PACKET_ID_S2C_ASTEROIDCOLLIDED,
	PACKET_ID_S2C_NEWMISSILE,
	PACKET_ID_S2C_DELETEMISSILE,
	PACKET_ID_S2C_NEWBOOM,
	PACKET_ID_S2C_HITSHIP,
	PACKET_ID_S2C_RESPAWN,
	PACKET_ID_S2C_NEWMINE,
	PACKET_ID_S2C_DELETEMINE,
	PACKET_ID_S2C_NEWTIMEBOMB,
	PACKET_ID_S2C_DELETETIMEBOMB,
	PACKET_ID_S2C_NEWSPEEDUP,
	PACKET_ID_S2C_DELETESPEEDUP,
	PACKET_ID_S2C_SPEEDUPMOVEMENT,

    PACKET_ID_S2C_END
};


/**
* This is the list of data structures for each packet.
* ** IMPORTANT ** All the structures have to be fixed size.
*/
#pragma pack(push, 1) // Pack the memory alignment. 
struct PKT_S2C_WelcomeMessage {
    int ShipID; // ShipID is same as SessionID in the server.
};

struct PKT_S2C_FullGame {
	int ShipID;
};

struct PKT_S2C_EnemyShip {
    int ShipID;
    int ShipType;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_NewEnemyShip {
    int ShipID;
    int ShipType;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_NewAsteroid {
    int AsteroidID;
    float x;
    float y;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_EnemyShipDisconnect {
    int ShipID;
};

struct PKT_S2C_Movement {
    int ShipID;
    float server_x;
    float server_y;
    float server_w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_Collided {
    int ShipID;
    float server_x;
    float server_y;
    float server_w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_AsteroidMovement {
    int AsteroidID;
    float server_x;
    float server_y;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_S2C_AsteroidCollided {
	int AsteroidID;
	float server_x;
	float server_y;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

struct PKT_S2C_NewMissile {
	int OwnerShipID;
	int x;
	int y;
	int w;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

struct PKT_S2C_DeleteMissile {
	int OwnerShipID;
};

struct PKT_S2C_NewBoom {
	float x;
	float y;
	float w;
};

struct PKT_S2C_HitShip {
	int ShipHitID;
};

struct PKT_S2C_Respawn {
	int ShipID;
};

struct PKT_S2C_NewMine {
	float x;
	float y;
	int OwnerID;
};

struct PKT_S2C_DeleteMine {
	int ownerId;
};

struct PKT_S2C_DeleteTimeBomb {
	int ownerId;
};

struct PKT_S2C_NewTimeBomb {
	int ownerId;
	float x;
	float y;
};

struct PKT_S2C_NewSpeedUp {
	int SpeedUpID;
	float server_x;
	float server_y;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

struct PKT_S2C_DeleteSpeedUp {
	int SpeedUpID;
};

struct PKT_S2C_SpeedUpMovement {
	int SpeedUpID;
	float server_x;
	float server_y;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

#pragma pack(pop) // Return back to default memory alignment.


#endif