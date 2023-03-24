#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "send_packet.h"

#ifdef _DEBUG
void log( char *szFormat, ... );
#endif

extern CatNet::ClientNetwork NetObj;

// Static variables for send_packet.cpp
namespace
{
	_Timer net_timer;
	float timer_net_movement_update = 0;
}

namespace Net
{
	//-------------------------------------------------------------------------
	void send_packet_enter_game(Tank& me)
	{
		PKT_C2S_EnterGame enter_game_data;
		enter_game_data.user_id = me.tank_id;
		enter_game_data.x = me.get_x();
		enter_game_data.y = me.get_y();

		CatNet::PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_ENTERGAME;
		Packet << PacketID;
		Packet << enter_game_data;
		NetObj.SendPacket( Packet );
	}

	//-------------------------------------------------------------------------
	void send_packet_movement(Tank& me)
	{
		static int currSequenceID = 0;

		timer_net_movement_update += net_timer.GetTimer_msec();
		if(timer_net_movement_update > 100)
		{
			timer_net_movement_update = 0;

			PKT_C2S_TankMovement movement_data;
			movement_data.user_id = me.tank_id;
			movement_data.rotate = me.rotate;
			movement_data.throttle = me.throttle;
			movement_data.sequence_id = currSequenceID++;

			CatNet::PacketMessage Packet;
			int PacketID = PACKET_ID_C2S_TANKMOVEMENT;
			Packet << PacketID;
			Packet << movement_data;
			NetObj.SendPacket( Packet );
		}
	}

	void send_packet_turret_angle(Tank& me, const int& angle)
	{
		static int currSequenceID = 0;

		PKT_C2S_TankTurret turret_data;
		turret_data.user_id = me.tank_id;
		turret_data.angle = angle;
		turret_data.timestamp = static_cast<int64_t>(time(nullptr));
		turret_data.sequence_id = currSequenceID++;

		CatNet::PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_TANKTURRET;
		Packet << PacketID;
		Packet << turret_data;
		NetObj.SendPacket(Packet);
	}
}
