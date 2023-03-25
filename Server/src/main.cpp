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

#include <array>
#include <thread>

#ifdef _DEBUG
#include <io.h>
#endif

CatNet::ServerNetwork NetObj;
std::array<Tank, MAX_CLIENT_CONNECTION + 1> g_Tanks;

_Timer g_LoopTimer;

namespace
{
	constexpr float TANK_ROT_SPEED = 5.f;
	constexpr float TANK_MOV_SPEED = 100.f;
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
		timer += framet.GetTimer_sec();
		if(timer > frame_time)
		{
			//lock the mutex then work on updating all the clients
			for(auto& it : tanks)
			{
				if(it.connected)
				{
					//update the clients based on their input queues
					while(!it.input_queue.empty())
					{
						//just clear the queue and teleport the player
						Tank::InputData data = it.input_queue.front();
						it.angular_velocity = (float)data.rotate * TANK_ROT_SPEED;
						it.w += it.angular_velocity * data.frametime;
						it.velocity_x = cos(it.w) * (float)data.throttle;
						it.velocity_y = sin(it.w) * (float)data.throttle;
						it.x += it.velocity_x * TANK_MOV_SPEED * data.frametime;
						it.y += it.velocity_y * TANK_MOV_SPEED * data.frametime;
						//wrap the positions
						if (it.x > CLIENT_SCREEN_WIDTH)
							it.x -= CLIENT_SCREEN_WIDTH;
						else if (it.x < 0)
							it.x = CLIENT_SCREEN_WIDTH - it.x;
						if (it.y > CLIENT_SCREEN_HEIGHT)
							it.y -= CLIENT_SCREEN_HEIGHT;
						else if (it.y < 0)
							it.y = CLIENT_SCREEN_HEIGHT - it.x;
						it.latest_sequence_ID = data.movement_sequence_ID;
						it.input_queue.pop();
					}
				}
			}
			timer = 0.f;
		}
		server_timer += framet.GetTimer_sec();
		if(server_timer > tickrate)
		{
			//send update packets to all clients
			for(const auto& it : tanks)
			{
				if(it.connected)
				{
					SendPacketProcess_TankMovement(it);
				}
			}
			server_timer = 0.f;
		}
	}
}

void init_game_server( void )
{
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