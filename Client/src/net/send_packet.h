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
    void send_packet_movement(Tank& tank, float deltaTime, PKT_C2S_TankMovement& pkt);
    void send_packet_turret_angle(Tank& me, float deltaTime, const float& angle, PKT_C2S_TankTurret& pkt);
}

#endif