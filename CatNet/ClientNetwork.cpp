#include "ClientNetwork.h"

namespace CatNet
{
    DWORD WINAPI RunClientThread(void* args)
    {
        ClientNetwork* client{ static_cast<ClientNetwork*>(args) };
        do
        {
            client->GetSession()->ClearRecvBuffer();
            recv(client->GetSocket(), client->GetSession()->GetRecvBuffer(), SEND_BUFSIZE, 0);
            if (client->CheckPacketRecevied() > 0)
                client->GetProcessList()->Attach(client->GetSession(), SESSION_STATE_READPACKET, client->CheckPacketRecevied(), client->GetSession()->GetRecvBuffer());
            else if (0 == client->CheckPacketRecevied())
            {
                printf("Connection closed\n");
                break;
            }
            else
                printf("Recv failed: %d\n", WSAGetLastError());
            break;
        } while (true);
        return 0;
    }
}