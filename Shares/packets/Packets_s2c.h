#ifndef _PACKETS_S2C_H
#define _PACKETS_S2C_H
#pragma once


enum _PACKET_ID_SERVER_TO_CLIENT {
	PACKET_ID_S2C_EMPTY = 0,            // Start of the list. Do not remove this.

	PACKET_ID_S2C_WELCOMEMESSAGE,
	PACKET_ID_S2C_FULLGAME,
	PACKET_ID_S2C_ENTERGAMEOK,
    PACKET_ID_S2C_NEW_CLIENT,
	PACKET_ID_S2C_DISCONNECT_CLIENT,
	PACKET_ID_S2C_MOVEMENT,

    PACKET_ID_S2C_END
};


/**
* This is the list of data structures for each packet.
* ** IMPORTANT ** All the structures have to be fixed size.
*/
#pragma pack(push, 1) // Pack the memory alignment. 
struct PKT_S2C_WelcomeMessage {
    int client_id; // client_id is same as SessionID in the server.
};

struct PKT_S2C_FullGame {
	int client_id;
};

struct PKT_S2C_NewClient {
	int client_id;
	float x, y;
};

struct PKT_S2C_Disconnect {
    int client_id;
};

struct PKT_S2C_Movement {
	int client_id;
	float x, y, w;
	float vx, vy;
};


#pragma pack(pop) // Return back to default memory alignment.


#endif