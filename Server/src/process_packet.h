#ifndef _PROCESS_PACKET_H
#define _PROCESS_PACKET_H
#pragma once


// Function declarations.
void ReceviedPacketProcess( struct CatNet::ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_EnterGame( struct CatNet::ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_Movement( struct CatNet::ProcessSession *ToProcessSessoin );
void ReceivedPacketProcess_Collided( struct CatNet::ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_AsteroidCollided(struct CatNet::ProcessSession * ToProcessSession);
void ReceviedPacketProcess_NewMissile(struct CatNet::ProcessSession* ToProcessSession);
void ReceviedPacketProcess_DeleteMissile(struct CatNet::ProcessSession* ToProcessSession);
void ReceviedPacketProcess_NewBoom(struct CatNet::ProcessSession* ToProcessSession);
void ReceviedPakcetProcess_ShipHit(struct CatNet::ProcessSession* ToProcessSession);
void ReceviedPacketProcess_Respawn(struct CatNet::ProcessSession* ToProcessSession);
void ReceviedPacketProcess_NewMine(struct CatNet::ProcessSession* ToProcessSession);
void ReceivedPacketProcess_DeleteMine(struct CatNet::ProcessSession* ToProcessSession);
void ReceivedPacketProcess_NewBomb(struct CatNet::ProcessSession* ToProcessSession);
void ReceivedPacketProcess_DeleteBomb(struct CatNet::ProcessSession* ToProcessSession);
void ReceivedPacketProcess_DeleteSpeedUp(struct CatNet::ProcessSession* ToProcessSession);

#endif