#ifndef _PACKETS_C2S_H
#define _PACKETS_C2S_H

enum _PACKET_ID_CLIENT_TO_SERVER {
    PACKET_ID_C2S_EMPTY = 0,            // Start of the list. Do not remove this.

    PACKET_ID_C2S_ENTERGAME,
    PACKET_ID_C2S_MOVEMENT,

    PACKET_ID_C2S_END                   // This is the indicator for the end of the list. Do not put any packet ID after this.
};


/**
* This is the list of data structures for each packet.
* ** IMPORTANT ** All the structures have to be fixed size.
*/
#pragma pack(push, 1) // Pack the memory alignment. 
struct PKT_C2S_EnterGame {
    int user_id;
    float x, y;
};

struct PKT_C2S_Movement {
    int user_id;
    int rotate, throttle;
};


#pragma pack(pop) // Return back to default memory alignment.


#endif