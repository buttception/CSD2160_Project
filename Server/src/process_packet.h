#ifndef _PROCESS_PACKET_H
#define _PROCESS_PACKET_H
#pragma once


// Function declarations.
void ReceviedPacketProcess( struct HNet::_ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_EnterGame( struct HNet::_ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_Movement( struct HNet::_ProcessSession *ToProcessSessoin );
void ReceivedPacketProcess_Collided( struct HNet::_ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_AsteroidCollided(struct HNet::_ProcessSession * ToProcessSession);
void ReceviedPacketProcess_NewMissile(struct HNet::_ProcessSession* ToProcessSession);
void ReceviedPacketProcess_DeleteMissile(struct HNet::_ProcessSession* ToProcessSession);
void ReceviedPacketProcess_NewBoom(struct HNet::_ProcessSession* ToProcessSession);
void ReceviedPakcetProcess_ShipHit(struct HNet::_ProcessSession* ToProcessSession);
void ReceviedPacketProcess_Respawn(struct HNet::_ProcessSession* ToProcessSession);
void ReceviedPacketProcess_NewMine(struct HNet::_ProcessSession* ToProcessSession);
void ReceivedPacketProcess_DeleteMine(struct HNet::_ProcessSession* ToProcessSession);
void ReceivedPacketProcess_NewBomb(struct HNet::_ProcessSession* ToProcessSession);
void ReceivedPacketProcess_DeleteBomb(struct HNet::_ProcessSession* ToProcessSession);
void ReceivedPacketProcess_DeleteSpeedUp(struct HNet::_ProcessSession* ToProcessSession);

#endif