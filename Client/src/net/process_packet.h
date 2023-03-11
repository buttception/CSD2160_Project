#ifndef _PROCESSPACKET_H
#define _PROCESSPACKET_H
#pragma once

#include "ClientNetwork.h"

// Function declarerations.
namespace Net {
    bool InitNetwork( void );
    void ProcessPacket( Application *thisapp );

    void WelcomeMessage( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void NewEnemyShip( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void NewAstreroid( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void DisconnectEnemyShip( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void ShipMovement( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void ShipCollided( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
    void AsteroidMovement( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin );
	void AsteroidCollided(Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin);
	void NewMissile(Application* thisapp, struct HNet::_ProcessSession* ToProcessSessoin);
	void DeleteMissile(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
	void NewBoom(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
	void HitShip(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
	void Respawn(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
	void NewMine(Application* thisapp, struct HNet::_ProcessSession* ToPrcoessSession);
	void DeleteMine(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
	void NewTimeBomb(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
	void DeleteTimeBomb(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
	void NewSpeedUp(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
	void DeleteSpeedUp(Application* thisapp, struct HNet::_ProcessSession* ToProcessSession);
}



#endif
