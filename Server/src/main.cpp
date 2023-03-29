///////////////////////////////////////////////////////////////////////////////
// Simple message broadcasting server for SpaceShooter!                      //
///////////////////////////////////////////////////////////////////////////////
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "main.h"
#include "ServerNetwork.h"
#include "send_packet.h"
#include "process_packet.h"
#include "timer\timer.h"
#include "tank.h"
#include "missile.h"

#include <array>
#include <thread>

#include "collision.h"

#ifdef _DEBUG
#include <io.h>
#endif

constexpr float pi_2 = 3.14159265359f * 2.f;

CatNet::ServerNetwork NetObj;
std::array<Tank, MAX_CLIENT_CONNECTION + 1> g_Tanks;
std::vector<Missile> g_missiles;

_Timer g_LoopTimer;

namespace
{
	constexpr float TANK_ROT_SPEED = 5.f;
	constexpr float TANK_MOV_SPEED = 100.f;
	constexpr float MISSILE_SPEED = 500.f;
}

#ifdef _DEBUG
void log( char *szFormat, ... )
{
	char Buff[1024];
	char Msg[1024];
	va_list arg;

	va_start( arg, szFormat );
	_vsnprintf_s( Msg, 1024, szFormat, arg );
	va_end( arg );

	sprintf_s( Buff, 1024, "%s", Msg );
	_write( 1, Buff, strlen( Buff ) );
}
#endif

std::thread g_game_thread;

void GameUpdate(_Timer* framet_ptr, std::array<Tank, MAX_CLIENT_CONNECTION + 1>* tanks_ptr)
{
	constexpr float frame_time = 1.f / 60.f;
	constexpr float tickrate = 1.f / 30.f;
	float timer{}, server_timer{};
	std::array<Tank, MAX_CLIENT_CONNECTION + 1>& tanks = *tanks_ptr;
	_Timer framet = *framet_ptr;
	while(1)
	{
		// Positional Update.
		timer += framet.GetTimer_sec();
		if(timer > frame_time)
		{
			//lock the mutex then work on updating all the clients
			for(auto& tank : tanks)
			{
				if(!tank.connected)
					continue;
				if (!tank.active && tank.respawn_timer > 0.f)
				{
					tank.respawn_timer -= timer;
					if (tank.respawn_timer <= 0.f)
						tank.active = true;
					continue;
				}

				//update the clients based on their input queues
				while (!tank.input_queue.empty())
				{
					//just clear the queue and teleport the player
					Tank::InputData data = tank.input_queue.front();
					tank.angular_velocity = (float)data.rotate * TANK_ROT_SPEED;
					tank.w += tank.angular_velocity * data.frametime;
					if (tank.w > pi_2)   tank.w -= pi_2;
					if (tank.w < 0.0f) tank.w += pi_2;
					tank.velocity_x = cos(tank.w) * (float)data.throttle;
					tank.velocity_y = sin(tank.w) * (float)data.throttle;
					tank.x += tank.velocity_x * TANK_MOV_SPEED * data.frametime;
					tank.y += tank.velocity_y * TANK_MOV_SPEED * data.frametime;

					// check collision with every single missile
					for (auto& missile : g_missiles)
					{
						if (!missile.alive)
							continue;

						if (missile.owner_id == tank.client_id)
							continue;

						// check collision
						if (Collision::CheckCollision(Collision::Circle(tank.x, tank.y, 32.f, tank.velocity_x, tank.velocity_y),
							Collision::Circle(missile.x, missile.y, 16.f, missile.velocity_x, missile.velocity_y)))
						{
							//std::cout << "collided\n";
							if (missile.alive)
							{
								tank.hp -= 10;
								if (tank.hp <= 0)
								{
									tank.active = false;
								}
							}
							missile.alive = false;
							//tank.active = false; // tank destroyed

							tank.hp -= 50;
							if (tank.hp <= 0)
							{
								g_Tanks[missile.owner_id].score += 10;
							}
						}
					}

					if (tank.hp <= 0)
					{
						tank.active = false;
						//respawn tank
						tank.x = 400;
						tank.y = 300;
						tank.w = 0;
						tank.hp = tank.max_hp;
						tank.respawn_timer = 1.f;
					}


					//wrap the positions
					if (tank.x > CLIENT_SCREEN_WIDTH + tank.sprite_size_x / 2.f)
						tank.x -= CLIENT_SCREEN_WIDTH + tank.sprite_size_x;
					else if (tank.x < -tank.sprite_size_x / 2.f)
						tank.x += CLIENT_SCREEN_WIDTH + tank.sprite_size_x;
					if (tank.y > CLIENT_SCREEN_HEIGHT + tank.sprite_size_y / 2.f)
						tank.y -= CLIENT_SCREEN_HEIGHT + tank.sprite_size_y;
					else if (tank.y < -tank.sprite_size_y / 2.f)
						tank.y += CLIENT_SCREEN_HEIGHT + tank.sprite_size_y;
					tank.latest_sequence_ID = data.movement_sequence_ID;
					tank.input_queue.pop();
				}
				// update the turret rots
				while (!tank.turret_input_queue.empty())
				{
					Tank::TurretInputData data = tank.turret_input_queue.front();
					tank.turret_rotation = data.angle;
					tank.missile_shot = data.missile_shot;

					float missile_velX = cos(data.angle) * MISSILE_SPEED;
					float missile_velY = sin(data.angle) * MISSILE_SPEED;

					// if client shot missile
					if (tank.missile_shot)
					{
						//std::cout << tank.missile_shot << "<-missile shot\n";
						g_missiles.push_back(Missile(tank.x, tank.y, tank.turret_rotation,
							missile_velX, missile_velY, tank.client_id));
					}
					tank.latest_turret_seq_ID = data.turret_sequence_ID;
					tank.turret_input_queue.pop();
				}
			}
			
			// UPDATE MISSILES POS HERE
			for (auto& missile : g_missiles)
			{
				// update position of missile
				missile.x += missile.velocity_x * timer;
				missile.y += missile.velocity_y * timer;

				if (missile.x > CLIENT_SCREEN_WIDTH || missile.x < 0 || missile.y > CLIENT_SCREEN_HEIGHT || missile.y < 0)
					missile.alive = false;
				// no need to update w because missile will not change direction
			}

			timer = 0.f;
		}

		// Send-Packet Update.
		server_timer += framet.GetTimer_sec();
		if(server_timer > tickrate)
		{
			// Send update packets to all clients
			for(auto& tank : tanks)
			{
				if (!NetObj.GetSessionList()->CheckIndex(tank.client_id) && tank.connected)
				{
					tank.connected = false;
					SendPacketProcess_Disconnect(tank.client_id);
				}
				if(tank.connected)
				{
					SendPacketProcess_TankMovement(tank);
					SendPacketProcess_TankTurret(tank);

					// send packet for missiles to client
					for (const auto& tank : g_missiles)
						SendPacketProcess_Missile(tank);
					
					SendPacketProcess_TankState(tank);
				}
			}

			// Remove destroyed missiles and missiles of disconnected clients
			auto missile = g_missiles.begin();
			while (missile != g_missiles.end())
			{
				if (!missile->alive || !NetObj.GetSessionList()->CheckIndex(missile->owner_id))
					missile = g_missiles.erase(missile);
				else
					++missile;
			}

			server_timer = 0.f;
		}
	}
}

void init_game_server( void )
{
	std::cout << "Server->Client packet sizes:\n";
	std::cout << "PKT_S2C_WelcomeMessage " << sizeof(PKT_S2C_WelcomeMessage) << std::endl;
	std::cout << "PKT_S2C_FullGame       " << sizeof(PKT_S2C_FullGame) << std::endl;
	std::cout << "PKT_S2C_ClientPos      " << sizeof(PKT_S2C_ClientPos) << std::endl;
	std::cout << "PKT_S2C_Disconnect     " << sizeof(PKT_S2C_Disconnect) << std::endl;
	std::cout << "PKT_S2C_TankMovement   " << sizeof(PKT_S2C_TankMovement) << std::endl;
	std::cout << "PKT_S2C_TankTurret     " << sizeof(PKT_S2C_TankTurret) << std::endl;
	std::cout << "PKT_S2C_ClickStart     " << sizeof(PKT_S2C_ClickStart) << std::endl;
	std::cout << "PKT_S2C_Missile        " << sizeof(PKT_S2C_Missile) << std::endl;
	std::cout << "PKT_S2C_TankState      " << sizeof(PKT_S2C_TankState) << std::endl;

	g_game_thread = std::thread{ GameUpdate, &g_LoopTimer, &g_Tanks };
}

int main( void )
{
	init_game_server();
	if( false == NetObj.InitNet(1, 1, 5050 ) )
	{
#ifdef _DEBUG
		log( "%s", NetObj.GetErrorMessage() );
#endif
		return 0;
	}
#ifdef _DEBUG
	log( "%s", "\n Server network initialized!" );
	log( "%s", "\n Network thread started! Ready to accept & receive the message." );
#endif
	Sleep( 1000 ); // Wait for a while to make sure everything is ok.

	g_LoopTimer.GetTimer_sec(); // To initialize the timer.
	struct CatNet::ProcessSession *ToProcessSession;
	while( 1 )
	{
		while( nullptr != ( ToProcessSession = NetObj.GetProcessList()->GetFirstSession() ) )
		{
			switch( ToProcessSession->m_SessionState )
			{
				case CatNet::SESSION_STATE_NEWCONNECTION:
				{ // New connection request arrived.
#ifdef _DEBUG
					log( "\n New connection connected: Index:%d. Total Connection now:%d", ToProcessSession->m_SessionIndex, NetObj.GetConnectedCount() );
#endif
					// TODO: Check if more than 3, then need deny
					// check if there is already 3 players
					// deny accept if alr got 3 players
					if (NetObj.GetConnectedCount() > 3)
					{
						//deny access
						SendPacketProcess_FullGame(ToProcessSession->m_SessionIndex);
					}
					else
					{
						SendPacketProcess_NewAccept(ToProcessSession->m_SessionIndex);
					}
				}
				break;

				case CatNet::SESSION_STATE_CLOSEREDY:
				{ // Connection closed arrived or communication error.
#ifdef _DEBUG
					log( "\n Received: Index %d wants to close or already closed.\n Total Connection now:%d",
							ToProcessSession->m_SessionIndex, NetObj.GetConnectedCount() );
#endif
					SendPacketProcess_Disconnect( ToProcessSession->m_SessionIndex);
				}
				break;

				case CatNet::SESSION_STATE_READPACKET:
				{ // Any packet data received.
					ReceivedPacketProcess( ToProcessSession );
				}
				break;
			}

			NetObj.GetProcessList()->DeleteFirstSession();
		}
		Sleep( 100 ); // You can check a timer to nomalize the looping speed (FPS).
	}

	return 0;
}