#ifndef _PROCESS_PACKET_H
#define _PROCESS_PACKET_H
#pragma once


// Function declarations.
void ReceivedPacketProcess( struct CatNet::ProcessSession *ToProcessSession );
void ReceivedPacketProcess_EnterGame( struct CatNet::ProcessSession *ToProcessSession );
void ReceivedPacketProcess_Movement( struct CatNet::ProcessSession *ToProcessSession );

#endif