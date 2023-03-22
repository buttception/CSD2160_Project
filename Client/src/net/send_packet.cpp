#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "send_packet.h"

#ifdef _DEBUG
void log( char *szFormat, ... );
#endif

extern CatNet::ClientNetwork NetObj;
// Global variables for send_packet.cpp
namespace Net
{
    _Timer net_timer;
    float timer_net_movement_update = 0;
}

namespace Net
{
    //-------------------------------------------------------------------------
    void send_packet_enter_game( Tank& me )
    {
        struct PKT_C2S_EnterGame enter_game_data;
        enter_game_data.user_id = me.tank_id;
        enter_game_data.x = me.get_x();
        enter_game_data.y = me.get_y();

        struct CatNet::PacketMessage Packet;
        int PacketID = PACKET_ID_C2S_ENTERGAME;
        Packet << PacketID;
        Packet << enter_game_data;
#ifdef _DEBUG
        log( "\nSend: PACKET_ID_C2S_ENTERGAME, ID:%d, x:%0.0f, y:%0.0f\n",
			enter_game_data.user_id, enter_game_data.x, enter_game_data.y );
#endif
        NetObj.SendPacket( Packet );
    }

    //-------------------------------------------------------------------------
    void send_packet_movement( Tank& me )
    {
        timer_net_movement_update += net_timer.GetTimer_msec();
        if( timer_net_movement_update > 100 )
        {
            timer_net_movement_update = 0;

            PKT_C2S_Movement movement_data;
            movement_data.user_id = me.tank_id;
            movement_data.rotate = me.rotate;
            movement_data.throttle = me.throttle;

            CatNet::PacketMessage Packet;
            int PacketID = PACKET_ID_C2S_MOVEMENT;
            Packet << PacketID;
            Packet << movement_data;
//#ifdef _DEBUG
//            log( "\nSend: PACKET_ID_C2S_MOVEMENT, ID:%d, x:%0.0f, y:%0.0f, w:%0.2f, av:%0.2f",
//                 PacketData.ShipID, PacketData.x, PacketData.y, PacketData.w,
//                 PacketData.velocity_x, PacketData.velocity_y, PacketData.angular_velocity);
//#endif
            NetObj.SendPacket( Packet );
        }
    }
}
