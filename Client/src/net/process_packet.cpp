#include <string>
#include <thread>

#include "ClientNetwork.h"
#include "application.h"
#include "packets\packets_s2c.h"
#include "process_packet.h"
#include "send_packet.h"
#include "movables/Tank.h"
#include "globals.h"

// NetLib Step 2. Client network object to use network library.
CatNet::ClientNetwork NetObj;

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif

namespace Net
{
	bool InitNetwork(void)
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
				case PACKET_ID_S2C_OLD_CLIENT:
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
				case PACKET_ID_S2C_MISSILE:
					UpdateMissile(thisapp, ToProcessSession);
					break;
				case PACKET_ID_S2C_TANKSTATE:
					UpdateTankState(thisapp, ToProcessSession);
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
	void WelcomeMessage(Application* thisapp, CatNet::ProcessSession* ToProcessSession)
	{
		struct PKT_S2C_WelcomeMessage welcome_message_data;
		ToProcessSession->m_PacketMessage >> welcome_message_data;
		thisapp->GetPlayer().tank_id = welcome_message_data.client_id;
#ifdef _DEBUG
		log("\nReceived: PACKET_ID_S2C_WELCOMEMESSAGE. ShipID:%d\n", welcome_message_data.client_id);
#endif
		// Send my spaceship info to server for synchronization.
		Net::send_packet_enter_game(thisapp->GetPlayer());
	}

	//-------------------------------------------------------------------------
	void NewClient(Application* thisapp, CatNet::ProcessSession* ToProcessSession)
	{
		PKT_S2C_ClientPos data;
		ToProcessSession->m_PacketMessage >> data;
		if (thisapp->GetPlayer().tank_id == data.client_id)
			return;

		std::string enemy_name = "Enemy" + std::to_string(data.client_id);

		Tank enemy("tank1.png");
		enemy.player_name = enemy_name;
		enemy.tank_id = data.client_id;
		enemy.set_x(data.x);
		enemy.set_y(data.y);
		thisapp->GetClients().push_back(enemy);
	}

	//-------------------------------------------------------------------------
	void DisconnectClient(Application* thisapp, CatNet::ProcessSession* ToProcessSessoin)
	{
		PKT_S2C_Disconnect data;
		ToProcessSessoin->m_PacketMessage >> data;
		//clear the enemy tank that got disconnected

		for (auto i = thisapp->GetClients().begin(); i != thisapp->GetClients().end(); )
		{
			if (i->tank_id == data.id)
			{
				i = thisapp->GetClients().erase(i);
			}
			else
			{
				++i;
			}
		}
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
			//std::cout << "\nDiscarding outdated input packets after " << data.sequence_id << "... BufSize: " << thisapp->QueuedPlayerMovements.size() << std::endl;
			for (int i = thisapp->QueuedPlayerMovements.size(); i > 0; --i)
			{
				PKT_C2S_TankMovement temp = thisapp->QueuedPlayerMovements.front();
				if (temp.sequence_id == INT_MIN || temp.sequence_id <= data.sequence_id)
				{
					//std::cout << "DISCARDED PKT ID: " << temp.sequence_id << " I: "<< i << std::endl;
					if(!Global::application->isMechanism[Application::MCH_RECONCILIATION])
					{
						// for when the reconciliation is deactivated, clear the queue
						// and save the same seq
						if(temp.sequence_id == data.sequence_id)
						{
							thisapp->QueuedPlayerMovements.clear();
							thisapp->QueuedPlayerMovements.push_back(temp);
							break;
						}
					}
					thisapp->QueuedPlayerMovements.pop_front();
				}
			}

			// Set client's server data
			tank->set_server_x(data.x);
			tank->set_server_y(data.y);
			tank->set_server_w(data.w);

			//std::cout << "Setting Pos: [" << tank->get_client_x() << ", " << tank->get_client_y() << ", " << tank->get_client_w()
			//		  << "] to [" << tank->get_server_x() << ", " << tank->get_server_y() << ", " << tank->get_server_w() << "]\n";
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
		PKT_S2C_TankTurret data;
		ToProcessSession->m_PacketMessage >> data;

		Tank* tank{};
		if (thisapp->GetPlayer().tank_id == data.client_id)
		{
			// Update my tank.
			tank = &thisapp->GetPlayer();

			// Discard outdated inputs.
			for (int i = thisapp->QueuedPlayerTurret.size(); i > 0; --i)
			{
				PKT_C2S_TankTurret temp = thisapp->QueuedPlayerTurret.front();
				if (temp.sequence_id == INT_MIN || temp.sequence_id <= data.sequence_id)
				{
					//std::cout << "DISCARDED PKT ID: " << temp.sequence_id << " I: "<< i << std::endl;
					if (!Global::application->isMechanism[Application::MCH_RECONCILIATION])
					{
						if (temp.sequence_id == data.sequence_id)
						{
							thisapp->QueuedPlayerTurret.clear();
							thisapp->QueuedPlayerTurret.push_back(temp);
							break;
						}
					}
					thisapp->QueuedPlayerTurret.pop_front();
				}
			}

			// Set client's turret rot
			//tank->turret_rotation = data.angle;
			tank->server_turret_rot = data.angle;
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
				// interpolate the rotate of turret
				//tank->turret_rotation = data.angle;
				tank->server_turret_rot = data.angle;
				if(Global::application->isMechanism[Application::MCH_INTERPOLATE])
				tank->client_turret_rot = tank->turret_rotation;
			}
		}
	}

	//-------------------------------------------------------------------------
	void UpdateMissile(Application* thisapp, CatNet::ProcessSession* ToProcessSession)
	{
		PKT_S2C_Missile data;
		ToProcessSession->m_PacketMessage >> data;

		// Add to missiles vector, missiles vector cleared at beginning of every udate
		if (!thisapp->IsExistingMissile(data.missile_id))
			thisapp->EmplaceMissile(Missile(data.x, data.y, data.w,
				data.vx, data.vy, data.missile_id, data.client_id));
		else
		{
			Missile& missile = thisapp->GetMissile(data.missile_id);
			missile.set_server_x(data.x);
			missile.set_server_y(data.y);
			missile.set_server_w(data.w);
			missile.set_server_velocity_x(data.vx);
			missile.set_server_velocity_y(data.vy);
			missile.do_interpolate_update();
			missile.alive = data.alive;
		}
	}

	//-------------------------------------------------------------------------
	void UpdateTankState(Application* thisapp, CatNet::ProcessSession* ToProcessSession)
	{
		PKT_S2C_TankState data;
		ToProcessSession->m_PacketMessage >> data;

		Tank* tank{};
		if (thisapp->GetPlayer().tank_id == data.client_id)
		{
			// Update my tank.
			tank = &thisapp->GetPlayer();
			tank->active = data.active;
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
				tank->active = data.active;
			}
		}
	}
}