#ifndef _PROCESSPACKET_H
#define _PROCESSPACKET_H
#pragma once

#include "..\CatNet\ClientNetwork.h"

// Function declarerations.
namespace Net
{
    bool InitNetwork(void);
    void ProcessPacket(Application* thisapp);

    void WelcomeMessage(Application* thisapp, CatNet::ProcessSession* ToProcessSession);
    void NewClient(Application* thisapp, CatNet::ProcessSession* ToProcessSession);
    void DisconnectClient(Application* thisapp, CatNet::ProcessSession* ToProcessSession);

    void UpdateTankMovement(Application* thisapp, CatNet::ProcessSession* ToProcessSession);
    void UpdateTankTurret(Application* thisapp, CatNet::ProcessSession* ToProcessSession);

    void ClickedStart(Application* thisapp, CatNet::ProcessSession* ToProcessSession);
}

#endif
