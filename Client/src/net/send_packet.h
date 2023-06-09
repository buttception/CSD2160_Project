#ifndef _SEND_PACKET_H
#define _SEND_PACKET_H
#pragma once

#include "timer\timer.h"
#include "ClientNetwork.h"
#include "packets\packets_c2s.h"
#include "movables/Tank.h"

// Function prototypes.
namespace Net
{
    void send_packet_enter_game(Tank& tank);
    void send_packet_movement(Tank& tank, const float& angle, PKT_C2S_TankMovement& pkt, PKT_C2S_TankTurret& t_pkt);
    //void send_packet_tank_state(Tank& me, float deltaTime, PKT_C2S_TankState& pkt);
    void send_packet_disconnect(Tank& tank);
    void send_packet_click_start(Tank& tank);
}

#endif