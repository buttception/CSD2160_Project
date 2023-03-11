#ifndef _SEND_PACKET_H
#define _SEND_PACKET_H
#pragma once

#include "..\Shares\timer\timer.h"
#include "..\movables\ship.h"
#include "..\movables\asteroid.h"
#include "..\movables\missile.h"
#include "..\movables\Boom.h"
#include "ClientNetwork.h"
#include "packets\packets_c2s.h"

class Mine;
class TimeBomb;
class SpeedUp;

// Function prototypes.
namespace Net
{
    void send_packet_enter_game( Ship *myship );
    void send_packet_myship_movement( Ship *myship );
    void send_packet_collided( Ship *myship );
	void send_packet_asteroid_collided(Asteroid *asteroid);
	void send_packet_new_missile(Missile *NewMissile);
	void send_packet_delete_missile(Missile *missile);
	void send_packet_new_boom(Boom* boom);
	void send_packet_ship_hit(const int shipID, float x, float y);
	void send_packet_respawn(const int shipID);
	void send_packet_new_mine(Mine* mine);
	void send_packet_delete_mine(Mine*mine);
	void send_packet_new_boom(TimeBomb* bomb);
	void send_packet_delete_time_bomb(TimeBomb* bomb);
	void send_packet_delete_speed_up(SpeedUp* sp);
}


#endif