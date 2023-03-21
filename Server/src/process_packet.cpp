#include "main.h"
#include "../../CatNet/ServerNetwork.h"
#include "process_packet.h"
#include "packets\packets_c2s.h"
#include "packets\packets_s2c.h"
#include "send_packet.h"
#include "Ship.h"
#include "asteroid.h"
#include "speedup.h"
#include <vector>

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif

extern CatNet::ServerNetwork NetObj;
extern _Ship g_ShipList[];
extern std::vector <_Asteroid *> g_AsteroidList; // Asteroid list.
extern std::vector<SpeedUp*>  powerUpList;

void ReceviedPacketProcess( struct CatNet::ProcessSession *ToProcessSessoin )
{
    int PacketID;

    ToProcessSessoin->m_PacketMessage >> PacketID;
    switch( PacketID )
    {
        case PACKET_ID_C2S_ENTERGAME:
        {
            ReceviedPacketProcess_EnterGame( ToProcessSessoin );
        }
        break;

        case PACKET_ID_C2S_MOVEMENT:
        {
            ReceviedPacketProcess_Movement( ToProcessSessoin );
        }
        break;

        case PACKET_ID_C2S_COLLIDED:
        {
            ReceivedPacketProcess_Collided( ToProcessSessoin );
        }
		break;

		case PACKET_ID_C2S_ASTEROIDCOLLIDED:
		{
			ReceviedPacketProcess_AsteroidCollided(ToProcessSessoin);
		}
        break;

		case PACKET_ID_C2S_NEWMISSILE:
		{
			ReceviedPacketProcess_NewMissile(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_DELETEMISSILE:
		{
			ReceviedPacketProcess_DeleteMissile(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_NEWBOOM:
		{
			ReceviedPacketProcess_NewBoom(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_HITSHIP:
		{
			ReceviedPakcetProcess_ShipHit(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_RESPAWN:
		{
			ReceviedPacketProcess_Respawn(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_NEWMINE:
		{
			ReceviedPacketProcess_NewMine(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_DELETEMINE:
		{
			ReceivedPacketProcess_DeleteMine(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_NEWTIMEBOMB:
		{
			ReceivedPacketProcess_NewBomb(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_DELETETIMEBOMB:
		{
			ReceivedPacketProcess_DeleteBomb(ToProcessSessoin);
		}
		break;

		case PACKET_ID_C2S_DELETESPEEDUP:
		{
			ReceivedPacketProcess_DeleteSpeedUp(ToProcessSessoin);
		}
		break;
    }
}

void ReceviedPacketProcess_EnterGame( struct CatNet::ProcessSession *ToProcessSessoin )
{
    int ShipID = ToProcessSessoin->m_SessionIndex;
    struct PKT_C2S_EnterGame Data;

    ToProcessSessoin->m_PacketMessage >> Data;
    g_ShipList[ShipID].ship_type = Data.ShipType;
    g_ShipList[ShipID].x = Data.x;
    g_ShipList[ShipID].y = Data.y;
    g_ShipList[ShipID].w = Data.w;
    g_ShipList[ShipID].velocity_x = Data.velocity_x;
    g_ShipList[ShipID].velocity_y = Data.velocity_y;
    g_ShipList[ShipID].angular_velocity = Data.angular_velocity;

#ifdef _DEBUG
    log( "\nRecv PACKET_ID_C2S_ENTERGAME Conn:%d, ID:%d, Type:%d, x:%0.0f, y:%0.0f, w:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
         g_ShipList[ShipID].connected, ShipID, g_ShipList[ShipID].ship_type, g_ShipList[ShipID].x, g_ShipList[ShipID].y, g_ShipList[ShipID].w,
         g_ShipList[ShipID].velocity_x, g_ShipList[ShipID].velocity_y, g_ShipList[ShipID].angular_velocity );
#endif

    // Send Ack.
    struct CatNet::PacketMessage EnterGameAckPacket;
    int EnterGameAckPacketID = PACKET_ID_S2C_ENTERGAMEOK;
    EnterGameAckPacket << EnterGameAckPacketID;
	//SendPacketProcess_FullGame(ShipID);
    NetObj.SendPacket( ShipID, EnterGameAckPacket );

    // Send Asteroid list.
    struct CatNet::PacketMessage NewAsteroidPacket;
    struct PKT_S2C_NewAsteroid NewAsteroidData;
    int NewAsteroidPacketID = PACKET_ID_S2C_NEWASTEROID;
    for( auto asteroid : g_AsteroidList )
    {
        NewAsteroidData.AsteroidID = asteroid->get_ID();
        NewAsteroidData.x = asteroid->get_x();
        NewAsteroidData.y = asteroid->get_y();
        NewAsteroidData.velocity_x = asteroid->get_velocity_x();
        NewAsteroidData.velocity_y = asteroid->get_velocity_y();
        NewAsteroidData.angular_velocity = asteroid->get_angular_velocity();

        NewAsteroidPacket.Reset();
        NewAsteroidPacket << NewAsteroidPacketID;
        NewAsteroidPacket << NewAsteroidData;
        NetObj.SendPacket( ShipID, NewAsteroidPacket );
#ifdef _DEBUG
        log( "\nSend PACKET_ID_S2C_NEWASTEROID to ID:%d > AsteroidID:%d, x:%0.0f, y:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
             ShipID, NewAsteroidData.AsteroidID, NewAsteroidData.x, NewAsteroidData.y, NewAsteroidData.velocity_x, NewAsteroidData.velocity_y, NewAsteroidData.angular_velocity );
#endif
    }

    // Send Info & Notice Message.
    int Index;

    struct CatNet::PacketMessage EnemyPacket;
    struct PKT_S2C_EnemyShip EnemyPacketData;
    int EnemyPacketID = PACKET_ID_S2C_ENEMYSHIP;

    struct CatNet::PacketMessage NewEnemyShipPacket;
    struct PKT_S2C_NewEnemyShip NewEnemyShipPacketData;
    int NewEnemyPacketID = PACKET_ID_S2C_NEWENEMYSHIP;
    NewEnemyShipPacketData.ShipID = ShipID;
    NewEnemyShipPacketData.ShipType = g_ShipList[ShipID].ship_type;
    NewEnemyShipPacketData.x = g_ShipList[ShipID].x;
    NewEnemyShipPacketData.y = g_ShipList[ShipID].y;
    NewEnemyShipPacketData.w = g_ShipList[ShipID].w;
    NewEnemyShipPacketData.velocity_x = g_ShipList[ShipID].velocity_x;
    NewEnemyShipPacketData.velocity_y = g_ShipList[ShipID].velocity_y;
    NewEnemyShipPacketData.angular_velocity = g_ShipList[ShipID].angular_velocity;

    NewEnemyShipPacket << NewEnemyPacketID;
    NewEnemyShipPacket << NewEnemyShipPacketData;

    for( Index = 1; Index <= MAX_CLIENT_CONNECTION; ++Index )
    {
        if( true == g_ShipList[Index].connected )
        {
            if( ShipID == Index ) continue;

            // Send Info Message to SessionID.
            EnemyPacketData.ShipID = Index;
            EnemyPacketData.ShipType = g_ShipList[Index].ship_type;
            EnemyPacketData.x = g_ShipList[Index].x;
            EnemyPacketData.y = g_ShipList[Index].y;
            EnemyPacketData.w = g_ShipList[Index].w;
            EnemyPacketData.velocity_x = g_ShipList[Index].velocity_x;
            EnemyPacketData.velocity_y = g_ShipList[Index].velocity_y;
            EnemyPacketData.angular_velocity = g_ShipList[Index].angular_velocity;

            EnemyPacket.Reset();
            EnemyPacket << EnemyPacketID;
            EnemyPacket << EnemyPacketData;
            NetObj.SendPacket( ShipID, EnemyPacket );
#ifdef _DEBUG
            log( "\nSend PACKET_ID_S2C_ENEMYSHIP to ID:%d > ShipID:%d, Type:%d, x:%0.0f, y:%0.0f, w:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
                 ShipID, EnemyPacketData.ShipID, EnemyPacketData.ShipType, EnemyPacketData.x, EnemyPacketData.y, EnemyPacketData.w,
                 EnemyPacketData.velocity_x, EnemyPacketData.velocity_y, EnemyPacketData.angular_velocity );
#endif

            // Send Notice Message to Index.
            NetObj.SendPacket( Index, NewEnemyShipPacket );
#ifdef _DEBUG
            log( "\nSend PACKET_ID_S2C_NEWENEMYSHIP to ID:%d > ShipID:%d, Type:%d, x:%0.0f, y:%0.0f, w:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
                 Index, NewEnemyShipPacketData.ShipID, NewEnemyShipPacketData.ShipType, NewEnemyShipPacketData.x, NewEnemyShipPacketData.y, NewEnemyShipPacketData.w,
                 NewEnemyShipPacketData.velocity_x, NewEnemyShipPacketData.velocity_y, NewEnemyShipPacketData.angular_velocity );
#endif
        }
    }
}

void ReceviedPacketProcess_Movement( struct CatNet::ProcessSession *ToProcessSessoin )
{
    int ShipID = ToProcessSessoin->m_SessionIndex;
    struct PKT_C2S_Movement RecvData;

    // Update server data.
    ToProcessSessoin->m_PacketMessage >> RecvData;
    g_ShipList[ShipID].x = RecvData.x;
    g_ShipList[ShipID].y = RecvData.y;
    g_ShipList[ShipID].w = RecvData.w;
    g_ShipList[ShipID].velocity_x = RecvData.velocity_x;
    g_ShipList[ShipID].velocity_y = RecvData.velocity_y;
    g_ShipList[ShipID].angular_velocity = RecvData.angular_velocity;
#ifdef _DEBUG
    log( "\nRecv PACKET_ID_C2S_MOVEMENT ShipID:%d, x:%0.0f, y:%0.0f, w:%0.0f, vx:%0.2f, vy:%0.2f, av:%0.2f",
         ShipID, RecvData.x, RecvData.y, RecvData.w, RecvData.velocity_x, RecvData.velocity_y, RecvData.angular_velocity );
#endif

    // Send to everyone except sender.
    struct CatNet::PacketMessage SendPacket;
    struct PKT_S2C_Movement SendData;
    int PacketID = PACKET_ID_S2C_MOVEMENT;

    SendData.ShipID = ShipID;
    SendData.server_x = g_ShipList[ShipID].x;
    SendData.server_y = g_ShipList[ShipID].y;
    SendData.server_w = g_ShipList[ShipID].w;
    SendData.velocity_x = g_ShipList[ShipID].velocity_x;
    SendData.velocity_y = g_ShipList[ShipID].velocity_y;
    SendData.angular_velocity = g_ShipList[ShipID].angular_velocity;

    SendPacket << PacketID;
    SendPacket << SendData;
    NetObj.SendPacketToAll( SendPacket );
}

void ReceivedPacketProcess_Collided( struct CatNet::ProcessSession *ToProcessSessoin )
{
    int ShipID = ToProcessSessoin->m_SessionIndex;
    struct PKT_C2S_Collided RecvData;

    // Update the server data.
    ToProcessSessoin->m_PacketMessage >> RecvData;
    g_ShipList[ShipID].x = RecvData.x;
    g_ShipList[ShipID].y = RecvData.y;
    g_ShipList[ShipID].w = RecvData.w;
    g_ShipList[ShipID].velocity_x = RecvData.velocity_x;
    g_ShipList[ShipID].velocity_y = RecvData.velocity_y;
    g_ShipList[ShipID].angular_velocity = RecvData.angular_velocity;

    // Send to everyone
    struct CatNet::PacketMessage SendPacket;
    struct PKT_S2C_Collided SendData;
    int PacketID = PACKET_ID_S2C_COLLIDED;

    SendData.ShipID = ShipID;
    SendData.server_x = g_ShipList[ShipID].x;
    SendData.server_y = g_ShipList[ShipID].y;
    SendData.server_w = g_ShipList[ShipID].w;
    SendData.velocity_x = g_ShipList[ShipID].velocity_x;
    SendData.velocity_y = g_ShipList[ShipID].velocity_y;
    SendData.angular_velocity = g_ShipList[ShipID].angular_velocity;

    SendPacket << PacketID;
    SendPacket << SendData;
    NetObj.SendPacketToAll( SendPacket );
#ifdef _DEBUG
    log( "\nSend PACKET_ID_S2C_COLLIDED ShipID:%d, x:%0.0f, y:%0.0f, w:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
         SendData.ShipID, SendData.server_x, SendData.server_y, SendData.server_w,
         SendData.velocity_x, SendData.velocity_y, SendData.angular_velocity );
#endif
}

void ReceviedPacketProcess_AsteroidCollided(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_AsteroidCollided RecvData;
	int AsteroidID;

	//Update the server data
	ToProcessSession->m_PacketMessage >> RecvData;
	AsteroidID = RecvData.AsteroidID;

	for (auto itr_asteroid : g_AsteroidList) {
		if (itr_asteroid->get_ID() == AsteroidID) {
			itr_asteroid->set_x(RecvData.x);
			itr_asteroid->set_y(RecvData.y);
			itr_asteroid->set_velocity_x(RecvData.velocity_x);
			itr_asteroid->set_velocity_y(RecvData.velocity_y);
			itr_asteroid->set_angular_velocity(RecvData.angular_velocity);

			//Now let's send the collided info to all clients
			SendPacketProcess_AsteroidCollided(itr_asteroid);
		}
	}
}

void ReceviedPacketProcess_NewMissile(struct CatNet::ProcessSession *ToProcessSession)
{
	struct PKT_C2S_NewMissile RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	// Send to everyone
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_NewMissile SendData;
	int PacketID = PACKET_ID_S2C_NEWMISSILE;
	SendData.OwnerShipID = RecvData.OwnerShipID;
	SendData.x = RecvData.x;
	SendData.y = RecvData.y;
	SendData.w = RecvData.w;
	SendData.velocity_x = RecvData.velocity_x;
	SendData.velocity_y = RecvData.velocity_y;
	SendData.angular_velocity = RecvData.angular_velocity;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceviedPacketProcess_DeleteMissile(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_DeleteMissile RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	// Send to everyone
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_DeleteMissile SendData;
	int PacketID = PACKET_ID_S2C_DELETEMISSILE;
	SendData.OwnerShipID = RecvData.OwnerShipID;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceviedPacketProcess_NewBoom(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_NewBoom RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	// Send to everyone
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_NewBoom SendData;
	int PacketID = PACKET_ID_S2C_NEWBOOM;
	SendData.x = RecvData.x;
	SendData.y = RecvData.y;
	SendData.w = RecvData.w;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceviedPakcetProcess_ShipHit(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_HitShip RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	// Send to everyone
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_HitShip SendData;
	int PacketID = PACKET_ID_S2C_HITSHIP;
	SendData.ShipHitID = RecvData.ShipHitID;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceviedPacketProcess_Respawn(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_Respawn RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	// Send to everyone
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_Respawn SendData;
	int PacketID = PACKET_ID_S2C_RESPAWN;
	SendData.ShipID = RecvData.ShipID;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceviedPacketProcess_NewMine(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_NewMine RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	// Send to everyone
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_NewMine SendData;
	int PacketID = PACKET_ID_S2C_NEWMINE;
	SendData.x = RecvData.x;
	SendData.y = RecvData.y;
	SendData.OwnerID = RecvData.OwnerID;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceivedPacketProcess_DeleteMine(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_DeleteMine RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_DeleteMine SendData;
	int PacketID = PACKET_ID_S2C_DELETEMINE;
	SendData.ownerId = RecvData.ownerId;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceivedPacketProcess_NewBomb(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_NewTimeBomb RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_NewTimeBomb SendData;
	int PacketID = PACKET_ID_S2C_NEWTIMEBOMB;
	SendData.ownerId = RecvData.ownerId;
	SendData.x = RecvData.x;
	SendData.y = RecvData.y;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceivedPacketProcess_DeleteBomb(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_DeleteTimeBomb RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_DeleteTimeBomb SendData;
	int PacketID = PACKET_ID_S2C_DELETETIMEBOMB;
	SendData.ownerId = RecvData.ownerId;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);
}

void ReceivedPacketProcess_DeleteSpeedUp(CatNet::ProcessSession * ToProcessSession)
{
	struct PKT_C2S_DeleteSpeedUp RecvData;
	ToProcessSession->m_PacketMessage >> RecvData;
	struct CatNet::PacketMessage SendPacket;
	struct PKT_S2C_DeleteSpeedUp SendData;
	int PacketID = PACKET_ID_S2C_DELETESPEEDUP;
	SendData.SpeedUpID = RecvData.SpeedUpID;
	SendPacket << PacketID;
	SendPacket << SendData;
	NetObj.SendPacketToAll(SendPacket);

	for (auto it : powerUpList)
	{
		if (it->get_ID() == RecvData.SpeedUpID)
			it->active = false;
	}
}
