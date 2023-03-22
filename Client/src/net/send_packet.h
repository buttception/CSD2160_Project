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
    void send_packet_enter_game( Tank& tank );
    void send_packet_movement( Tank& tank );
}


#endif