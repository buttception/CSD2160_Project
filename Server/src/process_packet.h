#ifndef _PROCESS_PACKET_H
#define _PROCESS_PACKET_H

// Function declarations.
void ReceivedPacketProcess(CatNet::ProcessSession* ToProcessSession);
void ReceivedPacketProcess_EnterGame(CatNet::ProcessSession* ToProcessSession);
void ReceivedPacketProcess_TankMovement(CatNet::ProcessSession* ToProcessSession);
void ReceivedPacketProcess_TankTurret(CatNet::ProcessSession* ToProcessSession);

#endif