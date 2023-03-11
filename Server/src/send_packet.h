#ifndef _SENDPACKET_H
#define _SENDPACKET_H

#include "ServerNetwork.h"
#include "packets\packets_s2c.h"
#include "asteroid.h"
#include "Ship.h"

class SpeedUp;


///////////////////////////////////////////////////////////////////////////////
// Global function declarations.
void SendPacketProcess_NewAccept( const int SessionID );
void SendPacketProcess_FullGame( const int SessionID );
void SendPacketProcess_NewAccept_SendWelcomeMessage( const int SessionID );
void SendPacketProcess_EnemyShipDisconnect( const int SessionID );
void SendPacketProcess_AsteroidMovement( _Asteroid *asteroid );
void SendPacketProcess_AsteroidCollided(_Asteroid *asteroid);
void SendPacketProcess_NewSpeedUp(SpeedUp* speedup);
void SendPacketProcess_SpeedUpMovement(SpeedUp* speedup);
void SendPacketProcess_DestroySpeedUp(SpeedUp* speedup);

#endif