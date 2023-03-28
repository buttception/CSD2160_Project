#include "ClientNetwork.h"
#include <packets/Packets_s2c.h>

namespace CatNet
{
	int s2cSize(int id)
	{
		switch (id)
		{
		case PACKET_ID_S2C_EMPTY:
			return 0;
			break;
		case PACKET_ID_S2C_WELCOMEMESSAGE:
			return sizeof(PKT_S2C_WelcomeMessage);
			break;
		case PACKET_ID_S2C_FULLGAME:
			return sizeof(PKT_S2C_FullGame);
			break;
		case PACKET_ID_S2C_ENTERGAMEOK:
			return 0;
			break;
		case PACKET_ID_S2C_NEW_CLIENT:
			return sizeof(PKT_S2C_ClientPos);
			break;
		case PACKET_ID_S2C_OLD_CLIENT:
			return sizeof(PKT_S2C_ClientPos);
			break;
		case PACKET_ID_S2C_DISCONNECT_CLIENT:
			return sizeof(PKT_S2C_Disconnect);
			break;
		case PACKET_ID_S2C_TANKMOVEMENT:
			return sizeof(PKT_S2C_TankMovement);
			break;
		case PACKET_ID_S2C_TANKTURRET:
			return sizeof(PKT_S2C_TankTurret);
			break;
		case PACKET_ID_S2C_MISSILE:
			return sizeof(PKT_S2C_Missile);
			break;
		case PACKET_ID_S2C_TANKSTATE:
			return sizeof(PKT_S2C_TankState);
			break;
		case PACKET_ID_S2C_END:
			return 0;
			break;
		}

		std::cout << "Unknown packet type ID! [" << id << "]\n";
		return -1;
	}

	DWORD WINAPI RunClientThread(void* args)
	{
		ClientNetwork* client{ static_cast<ClientNetwork*>(args) };
		do
		{
			//int length = recv(client->GetSocket(), client->GetSession()->GetRecvBuffer(), SEND_BUFSIZE, 0);
			//client->GetSession()->SetRecvBufferWritePos(length);
			//if (client->CheckPacketRecevied() > 0)
			//{
			//    client->GetProcessList()->Attach(client->GetSession(), SESSION_STATE_READPACKET, client->CheckPacketRecevied(), client->GetSession()->GetRecvBuffer());
			//}

			char m_RecvBuf[RECV_BUFSIZE];
			client->GetSession()->ClearRecvBuffer();
			int length = recv(client->GetSocket(), m_RecvBuf, RECV_BUFSIZE, 0);
			int offset = 0;
			if (length > 0)
			{
				while (offset < length)
				{
					int id = *reinterpret_cast<int*>(m_RecvBuf + offset);
					int dataSize = s2cSize(id);
					client->GetSession()->ClearRecvBuffer();
					memcpy(client->GetSession()->GetRecvBuffer(), m_RecvBuf + offset, dataSize + 4);
					client->GetProcessList()->Attach(client->GetSession(), SESSION_STATE_READPACKET, dataSize + 4, client->GetSession()->GetRecvBuffer());
					offset += dataSize + 4;
				}
				memset(m_RecvBuf, '\0', RECV_BUFSIZE);
			}
				
			else if (0 == client->CheckPacketRecevied())
			{
				printf("Connection closed\n");
				break;
			}
			else
			{
				printf("Recv failed: %d\n", WSAGetLastError());
				break;
			}
				

		} while (true);
		std::cout << "thread exit" << std::endl;
		return 0;
	}
}