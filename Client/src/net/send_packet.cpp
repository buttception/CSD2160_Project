#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "send_packet.h"
#include <chrono>

using namespace std::chrono;

#ifdef _DEBUG
void log( char *szFormat, ... );
#endif

extern CatNet::ClientNetwork NetObj;

// Static variables for send_packet.cpp
namespace
{
	_Timer net_timer;
	float timer_net_movement_update = 0;
	float timer_net_turret_update = 0;
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
		enter_game_data.sprite_size_x = me.sprite_->GetWidth();
		enter_game_data.sprite_size_y = me.sprite_->GetHeight();

		CatNet::PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_ENTERGAME;
		Packet << PacketID;
		Packet << enter_game_data;
		NetObj.SendPacket( Packet );
	}

	//-------------------------------------------------------------------------
	void send_packet_movement(Tank& me, const float& angle, PKT_C2S_TankMovement& pkt, PKT_C2S_TankTurret& t_pkt)
	{
		static int currSequenceID = 0;

		// 100ms delay to limit number of packets sent to the server.
		timer_net_movement_update += net_timer.GetTimer_msec();
		if (timer_net_movement_update > 10)
		{

			PKT_C2S_TankMovement data;
			data.user_id = me.tank_id;
			data.rotate = me.rotate;
			data.throttle = me.throttle;
			data.sequence_id = currSequenceID++;
			data.frameTime = timer_net_movement_update / 1000.f;
			pkt = data;

			CatNet::PacketMessage Packet;
			int PacketID = PACKET_ID_C2S_TANKMOVEMENT;
			Packet << PacketID;
			Packet << data;
			NetObj.SendPacket(Packet);

			static int currTurrSequenceID = 0;

			PKT_C2S_TankTurret turret_data;
			turret_data.user_id = me.tank_id;
			turret_data.angle = angle;
			turret_data.missile_shot = me.missile_shot;
			me.missile_shot = false;
			turret_data.sequence_id = currTurrSequenceID++;
			turret_data.frameTime = timer_net_movement_update / 1000.f;
			t_pkt = turret_data;

			CatNet::PacketMessage turret_packet;
			int turret_id = PACKET_ID_C2S_TANKTURRET;
			turret_packet << turret_id;
			turret_packet << turret_data;
			NetObj.SendPacket(turret_packet);

			timer_net_movement_update = 0;
		}
	}

	//-------------------------------------------------------------------------
	void send_packet_turret_angle(Tank& me, float deltaTime, const float& angle, PKT_C2S_TankTurret& pkt)
	{
	}
	
	//-------------------------------------------------------------------------
	/*void send_packet_tank_state(Tank& me, float deltaTime, PKT_C2S_TankState& pkt)
	{
		static int currTurrSequenceID = 0;

		PKT_C2S_TankState data;
		data.user_id = me.tank_id;
		data.active = me.active;
		data.hp = me.hp;
		data.timestamp = static_cast<int64_t>(time(nullptr));
		data.sequence_id = currTurrSequenceID++;
		data.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		data.frameTime = deltaTime;
		pkt = data;

		CatNet::PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_TANKSTATE;
		Packet << PacketID;
		Packet << data;
		NetObj.SendPacket(Packet);
	}*/

	//-------------------------------------------------------------------------
	void send_packet_disconnect(Tank& tank)
	{
		PKT_C2S_Disconnect data;
		data.user_id = tank.tank_id;

		CatNet::PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_DISCONNECT;
		Packet << PacketID;
		Packet << data;
		NetObj.SendPacket(Packet);
	}

	void send_packet_click_start(Tank& tank)
	{
		PKT_C2S_ClickStart data;
		data.user_id = tank.tank_id;
		data.active = tank.active;

		CatNet::PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_CLICKSTART;
		Packet << PacketID;
		Packet << data;
		NetObj.SendPacket(Packet);
	}
}
