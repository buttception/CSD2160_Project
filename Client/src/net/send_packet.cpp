#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "send_packet.h"
#include "..\movables\mine.h"
#include "..\movables\TimeBomb.h"
#include "..\movables\SpeedUp.h"

#ifdef _DEBUG
void log( char *szFormat, ... );
#endif

extern HNet::_ClientNetwork NetObj;
// Global variables for send_packet.cpp
namespace Net
{
    _Timer net_timer;
    float timer_net_movement_update = 0;
}

namespace Net
{
    //-------------------------------------------------------------------------
    void send_packet_enter_game( Ship * myship )
    {
        struct PKT_C2S_EnterGame PacketData;
        PacketData.ShipID = myship->GetShipID();
        PacketData.ShipType = myship->GetShipType();
        PacketData.x = myship->get_x();
        PacketData.y = myship->get_y();
        PacketData.w = myship->get_w();
        PacketData.velocity_x = myship->get_velocity_x();
        PacketData.velocity_y = myship->get_velocity_y();
        PacketData.angular_velocity = myship->get_angular_velocity();

        struct HNet::_PacketMessage Packet;
        int PacketID = PACKET_ID_C2S_ENTERGAME;
        Packet << PacketID;
        Packet << PacketData;
#ifdef _DEBUG
        log( "\nSend: PACKET_ID_C2S_ENTERGAME, ID:%d, Type:%d, x:%0.0f, y:%0.0f, w:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
             PacketData.ShipID, PacketData.ShipType, PacketData.x, PacketData.y, PacketData.w,
             PacketData.velocity_x, PacketData.velocity_y, PacketData.angular_velocity );
#endif
        NetObj.SendPacket( Packet );
    }

    //-------------------------------------------------------------------------
    void send_packet_myship_movement( Ship *myship )
    {
        timer_net_movement_update += net_timer.GetTimer_msec();
        if( timer_net_movement_update > 100 )
        {
            timer_net_movement_update = 0;

            struct PKT_C2S_Movement PacketData;
            PacketData.ShipID = myship->GetShipID();
            PacketData.x = myship->get_x();
            PacketData.y = myship->get_y();
            PacketData.w = myship->get_w();
            PacketData.velocity_x = myship->get_server_velocity_x();
            PacketData.velocity_y = myship->get_server_velocity_x();
            PacketData.angular_velocity = myship->get_angular_velocity();

            struct HNet::_PacketMessage Packet;
            int PacketID = PACKET_ID_C2S_MOVEMENT;
            Packet << PacketID;
            Packet << PacketData;
//#ifdef _DEBUG
//            log( "\nSend: PACKET_ID_C2S_MOVEMENT, ID:%d, x:%0.0f, y:%0.0f, w:%0.2f, av:%0.2f",
//                 PacketData.ShipID, PacketData.x, PacketData.y, PacketData.w,
//                 PacketData.velocity_x, PacketData.velocity_y, PacketData.angular_velocity);
//#endif
            NetObj.SendPacket( Packet );
        }
    }

    //-------------------------------------------------------------------------
    void send_packet_collided( Ship *myship )
    {
        struct PKT_C2S_Collided PacketData;
        PacketData.ShipID = myship->GetShipID();
        PacketData.x = myship->get_x();
        PacketData.y = myship->get_y();
        PacketData.w = myship->get_w();
        PacketData.velocity_x = myship->get_server_velocity_x();
        PacketData.velocity_y = myship->get_server_velocity_y();
        PacketData.angular_velocity = myship->get_angular_velocity();

        struct HNet::_PacketMessage Packet;
        int PacketID = PACKET_ID_C2S_COLLIDED;
        Packet << PacketID;
        Packet << PacketData;
        NetObj.SendPacket( Packet );
    }

	void send_packet_asteroid_collided(Asteroid *asteroid)
	{
		struct PKT_C2S_AsteroidCollided PacketData;
		PacketData.AsteroidID = asteroid->get_id();
		PacketData.x = asteroid->get_x();
		PacketData.y = asteroid->get_y();
		PacketData.velocity_x = asteroid->get_server_velocity_x();
		PacketData.velocity_y = asteroid->get_server_velocity_y();
		PacketData.angular_velocity = asteroid->get_angular_velocity();
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_ASTEROIDCOLLIDED;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}

	void send_packet_new_missile(Missile *NewMissile)
	{
		struct PKT_C2S_NewMissile PacketData;
		PacketData.OwnerShipID = NewMissile->get_ownerid();
		PacketData.x = NewMissile->get_x();
		PacketData.y = NewMissile->get_y();
		PacketData.w = NewMissile->get_w();
		PacketData.velocity_x = NewMissile->get_server_velocity_x();
		PacketData.velocity_y = NewMissile->get_server_velocity_y();
		PacketData.angular_velocity = NewMissile->get_angular_velocity();
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_NEWMISSILE;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_delete_missile(Missile * missile)
	{
		struct PKT_C2S_DeleteMissile PacketData;
		PacketData.OwnerShipID = missile->ownerid_;
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_DELETEMISSILE;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_new_boom(Boom * boom)
	{
		struct PKT_C2S_NewBoom PacketData;
		PacketData.x = boom->get_x();
		PacketData.y = boom->get_y();
		PacketData.w = boom->get_w();
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_NEWBOOM;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_ship_hit(const int shipID, float x, float y)
	{
		struct PKT_C2S_HitShip PacketData;
		PacketData.ShipHitID = shipID;
		PacketData.x = x;
		PacketData.y = y;
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_HITSHIP;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_respawn(const int shipID)
	{
		struct PKT_C2S_Respawn PacketData;
		PacketData.ShipID = shipID;
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_RESPAWN;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_new_mine(Mine * mine)
	{
		struct PKT_C2S_NewMine PacketData;
		PacketData.x = mine->get_x();
		PacketData.y = mine->get_y();
		PacketData.OwnerID = mine->ownerid_;
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_NEWMINE;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_delete_mine(Mine * mine)
	{
		struct PKT_C2S_DeleteMine PacketData;
		PacketData.ownerId = mine->ownerid_;
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_DELETEMINE;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_new_boom(TimeBomb * bomb)
	{
		struct PKT_C2S_NewTimeBomb PacketData;
		PacketData.x = bomb->get_x();
		PacketData.y = bomb->get_y();
		PacketData.ownerId = bomb->ownerid_;
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_NEWTIMEBOMB;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_delete_time_bomb(TimeBomb * bomb)
	{
		struct PKT_C2S_DeleteTimeBomb PacketData;
		PacketData.ownerId = bomb->ownerid_;
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_DELETETIMEBOMB;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
	void send_packet_delete_speed_up(SpeedUp * sp)
	{
		struct PKT_C2S_DeleteSpeedUp PacketData;
		PacketData.SpeedUpID = sp->id;
		struct HNet::_PacketMessage Packet;
		int PacketID = PACKET_ID_C2S_DELETESPEEDUP;
		Packet << PacketID;
		Packet << PacketData;
		NetObj.SendPacket(Packet);
	}
}
