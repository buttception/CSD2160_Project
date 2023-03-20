#ifndef _PROCESSPACKET_H
#define _PROCESSPACKET_H
#pragma once

#include "..\CatNet\ClientNetwork.h"

// Function declarerations.
namespace Net {
    bool InitNetwork( void );
    void ProcessPacket( Application *thisapp );

    void WelcomeMessage( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin );
    void NewEnemyShip( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin );
    void NewAstreroid( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin );
    void DisconnectEnemyShip( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin );
    void ShipMovement( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin );
    void ShipCollided( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin );
    void AsteroidMovement( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin );
	void AsteroidCollided(Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin);
	void NewMissile(Application* thisapp, struct CatNet::ProcessSession* ToProcessSessoin);
	void DeleteMissile(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
	void NewBoom(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
	void HitShip(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
	void Respawn(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
	void NewMine(Application* thisapp, struct CatNet::ProcessSession* ToPrcoessSession);
	void DeleteMine(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
	void NewTimeBomb(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
	void DeleteTimeBomb(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
	void NewSpeedUp(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
	void DeleteSpeedUp(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
}



#endif
