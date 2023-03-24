#ifndef _SENDPACKET_H
#define _SENDPACKET_H

#include "ServerNetwork.h"
#include "packets/packets_s2c.h"
#include "tank.h"

///////////////////////////////////////////////////////////////////////////////
// Global function declarations.
void SendPacketProcess_NewAccept(const int& sessionID);
void SendPacketProcess_FullGame(const int& sessionID);
void SendPacketProcess_NewAccept_SendWelcomeMessage(const int& sessionID);
void SendPacketProcess_Disconnect(const int& sessionID);
void SendPacketProcess_TankMovement(const Tank& Tank);

#endif