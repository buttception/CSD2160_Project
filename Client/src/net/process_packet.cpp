#include <string>
#include <thread>

#include "ClientNetwork.h"
#include "application.h"
#include "packets\packets_s2c.h"
#include "process_packet.h"
#include "send_packet.h"
#include "movables/Tank.h"

// NetLib Step 2. Client network object to use network library.
CatNet::ClientNetwork NetObj;

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif

namespace
{
	constexpr float TANK_ROT_SPEED = 5.f;
	constexpr float TANK_MOV_SPEED = 500.f;
}

namespace Net
{
	bool InitNetwork( void )
	{
		//return NetObj.InitNet( CatNet::APP_TYPE::APPTYPE_CLIENT, CatNet::PROTOCOL_TYPE::PROTOCOL_TCP, "127.0.0.1", 3456 );
		return NetObj.InitNet(2, 1, "127.0.0.1", 5050);
	}

	//-------------------------------------------------------------------------
	void ProcessPacket(Application* thisapp)
	{
		// Check any message from network! You can try to make this message processing with another thread but becareful with synchronization.
		// NetLib Step 6. Prepare the pointer of _ProcessSession and buffer structure of _PacketMessage.
		//         _ProcessSession pointer will give you the session information if there is any network communication from any client.
		//         _PacketMessage is for fetch the each of the actual data inside the packet buffer.
		CatNet::ProcessSession* ToProcessSession;
		// NetLib Step 7. Message Loop.
		//         Check any message from server and process.
		while (nullptr != (ToProcessSession = NetObj.GetProcessList()->GetFirstSession()))
		{ 
			// Something recevied from network.
			int PacketID;
			CatNet::PacketMessage temp;

			ToProcessSession->m_PacketMessage >> PacketID;
			switch (thisapp->GetGameState())
			{
			case GAMESTATE_INITIALIZING:
			{
				switch (PacketID)
				{
				case PACKET_ID_S2C_WELCOMEMESSAGE:
					WelcomeMessage(thisapp, ToProcessSession);
					break;

				case PACKET_ID_S2C_ENTERGAMEOK:
					thisapp->SetGameState(GAMESTATE_INPLAY);
					break;

				case PACKET_ID_S2C_FULLGAME:
					std::cout << "\nGame is full\n";
					break;
				}
			}
			break;

			case GAMESTATE_INPLAY:
			{
				switch (PacketID)
				{
				case PACKET_ID_S2C_NEW_CLIENT:
					NewClient(thisapp, ToProcessSession);
					break;
				case PACKET_ID_S2C_DISCONNECT_CLIENT:
					DisconnectClient(thisapp, ToProcessSession);
					break;
				case PACKET_ID_S2C_TANKMOVEMENT:
					UpdateTankMovement(thisapp, ToProcessSession);
					break;
				case PACKET_ID_S2C_TANKTURRET:
					UpdateTankTurret(thisapp, ToProcessSession);
					break;
				}
			}
			break;

			}

			// Step 8. After finish the process with packet, You should delete the session message.
			//         If you forget to delete, it will cause memory leak!
			NetObj.GetProcessList()->DeleteFirstSession();
		}
	}

	//-------------------------------------------------------------------------
	void WelcomeMessage( Application *thisapp, CatNet::ProcessSession *ToProcessSession )
	{
		struct PKT_S2C_WelcomeMessage welcome_message_data;
		ToProcessSession->m_PacketMessage >> welcome_message_data;
		thisapp->GetPlayer().tank_id = welcome_message_data.client_id;
#ifdef _DEBUG
		log( "\nReceived: PACKET_ID_S2C_WELCOMEMESSAGE. ShipID:%d", welcome_message_data.client_id );
#endif
		// Send my spaceship info to server for synchronization.
		Net::send_packet_enter_game( thisapp->GetPlayer() );
	}

	//-------------------------------------------------------------------------
	void NewClient( Application *thisapp, CatNet::ProcessSession *ToProcessSession )
	{
		//struct PKT_S2C_EnemyShip EnemyshipPacketData;
		PKT_S2C_NewClient new_client_packet;

		ToProcessSession->m_PacketMessage >> new_client_packet;

		std::string enemy_name = "Enemy" + std::to_string( new_client_packet.client_id );
		//create and push the new enemy tank into the vector

		/*Ship *EnemyShip = new Ship( new_client_packet.ShipType, EnemyShipName, new_client_packet.x, new_client_packet.y );

		EnemyShip->SetShipID( new_client_packet.ShipID );
		EnemyShip->SetShipName( EnemyShipName );
		EnemyShip->SetShipType( new_client_packet.ShipType );
		EnemyShip->set_x( new_client_packet.x );
		EnemyShip->set_y( new_client_packet.y );
		EnemyShip->set_w( new_client_packet.w );
		EnemyShip->set_velocity_x( new_client_packet.velocity_x );
		EnemyShip->set_velocity_y( new_client_packet.velocity_y );
		EnemyShip->set_angular_velocity( new_client_packet.angular_velocity );

		thisapp->GetEnemyShipList()->push_back( EnemyShip );*/
#ifdef _DEBUG
		log( "\nReceived: PACKET_ID_S2C_ENEMYSHIP or PACKET_ID_S2C_NEWENEMYSHIP. ShipID:%d", new_client_packet.client_id);
#endif
	}

	//-------------------------------------------------------------------------
	void DisconnectClient( Application *thisapp, CatNet::ProcessSession *ToProcessSessoin )
	{
		PKT_S2C_Disconnect client_disconnect_data;
		ToProcessSessoin->m_PacketMessage >> client_disconnect_data;
		//clear the enemy tank that got disconnected
		for(auto i = thisapp->GetClients().begin(); i != thisapp->GetClients().end(); ++i)
		{
			if(i->tank_id == client_disconnect_data.client_id)
			{
				thisapp->GetClients().erase(i);
			}
		}
#ifdef _DEBUG
		log("\nReceived: PACKET_ID_S2C_DISCONNECT. ShipID: %d", client_disconnect_data.client_id);
#endif
		}

	//-------------------------------------------------------------------------
	void UpdateTankMovement(Application* thisapp, CatNet::ProcessSession* ToProcessSession)
	{
		PKT_S2C_TankMovement data;
		ToProcessSession->m_PacketMessage >> data;

		Tank* tank{};
		if (thisapp->GetPlayer().tank_id == data.client_id)
		{ 
			// Update my tank.
			tank = &thisapp->GetPlayer();

			// Discard outdated inputs.
			std::cout << "\nDiscarding outdated input packets... Data ID: " << data.sequence_id << " BufSize: " << thisapp->QueuedPlayerMovements.size() << std::endl;
			for (int i = thisapp->QueuedPlayerMovements.size(); i > 0; --i)
			{
				PKT_C2S_TankMovement temp = thisapp->QueuedPlayerMovements.front();
				if (temp.sequence_id == INT_MIN || temp.sequence_id <= data.sequence_id)
				{
					std::cout << "PKT ID: " << temp.sequence_id << " I: "<< i << std::endl;
					thisapp->QueuedPlayerMovements.pop_front();
				}
			}
			// Apply reconciliation.
			for (const auto& temp : thisapp->QueuedPlayerMovements)
			{
				// Rotate tank.
				if (temp.rotate == -1)
				{
					tank->set_angular_velocity(-TANK_ROT_SPEED);
				}
				else if (temp.rotate == 1)
				{
					tank->set_angular_velocity(TANK_ROT_SPEED);
				}
				else
				{
					tank->set_angular_velocity(0.f);
				}

				// Translate tank.
				if (temp.throttle == -1)
				{
					tank->set_server_velocity_x(-cos(tank->get_server_w()) * TANK_MOV_SPEED);
					tank->set_server_velocity_y(-sin(tank->get_server_w()) * TANK_MOV_SPEED);
				}
				else if (temp.throttle == 1)
				{
					tank->set_server_velocity_x(cos(tank->get_server_w()) * TANK_MOV_SPEED);
					tank->set_server_velocity_y(sin(tank->get_server_w()) * TANK_MOV_SPEED);
				}
				else
				{
					tank->set_server_velocity_x(0.f);
					tank->set_server_velocity_y(0.f);
				}

				tank->do_interpolate_update();
			}
		}
		else
		{ 
			// Look for matching connected client.
			for (auto& it : thisapp->GetClients())
			{
				if (it.tank_id == data.client_id)
				{
					tank = &it;
					break;
				}
			}

			// Update other tank.
			if (tank != nullptr)
			{
				tank->set_server_x(data.x);
				tank->set_server_y(data.y);
				tank->set_server_w(data.w);
				tank->set_server_velocity_x(data.vx);
				tank->set_server_velocity_y(data.vy);
				tank->do_interpolate_update();
			}
		}
	}
	
	//-------------------------------------------------------------------------
	void UpdateTankTurret(Application* thisapp, CatNet::ProcessSession* ToProcessSession)
	{
	}
}

