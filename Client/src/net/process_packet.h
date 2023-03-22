#ifndef _PROCESSPACKET_H
#define _PROCESSPACKET_H
#pragma once

#include "..\CatNet\ClientNetwork.h"

// Function declarerations.
namespace Net {
    bool InitNetwork( void );
    void ProcessPacket( Application *thisapp );

    void WelcomeMessage( Application *thisapp, struct CatNet::ProcessSession *ToProcessSession );
    void NewClient( Application *thisapp, struct CatNet::ProcessSession * ToProcessSession);
    void DisconnectClient( Application *thisapp, struct CatNet::ProcessSession * ToProcessSession);
    void Movement(Application* thisapp, struct CatNet::ProcessSession* ToProcessSession);
}



#endif
