#ifndef _SENDPACKET_H
#define _SENDPACKET_H

#include "ServerNetwork.h"
#include "packets/packets_s2c.h"
#include "tank.h"
#include "missile.h"

///////////////////////////////////////////////////////////////////////////////
// Global function declarations.
void SendPacketProcess_NewAccept(const int& sessionID);
void SendPacketProcess_FullGame(const int& sessionID);
void SendPacketProcess_NewAccept_SendWelcomeMessage(const int& sessionID);
void SendPacketProcess_Disconnect(const int& sessionID);
void SendPacketProcess_TankMovement(const Tank& Tank);
void SendPacketProcess_TankTurret(const Tank& tank);
void SendPacketProcess_ClickStart(const int& sessionID, const bool& active);
void SendPacketProcess_Missile(const Missile& missile);
void SendPacketProcess_TankState(const Tank& missile);

#endif