///////////////////////////////////////////////////////////////////////////////
// Simple message broadcasting server for SpaceShooter!                      //
///////////////////////////////////////////////////////////////////////////////
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "main.h"
#include "../../CatNet/ServerNetwork.h"
#include "send_packet.h"
#include "process_packet.h"
#include "Ship.h"
#include "asteroid.h"
#include "speedup.h"
#include "timer\timer.h"

#include <vector>

#ifdef _DEBUG
#include <io.h>
#endif

using namespace std;

CatNet::ServerNetwork NetObj;
struct _Ship g_ShipList[MAX_CLIENT_CONNECTION + 1]; // Ship list. Array Index number is same as SessionIndex number from Network Library.
std::vector <_Asteroid *> g_AsteroidList; // Asteroid list.
std::vector<SpeedUp*> powerUpList;

_Timer g_LoopTimer;


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

bool init_game_server( void )
{ // Initialize whatever you need.

    // Initialize two asteroid. This is for simple example.
    g_AsteroidList.push_back( new _Asteroid( 1, 100, 100, 1 ) );
    g_AsteroidList.push_back( new _Asteroid( 2, 700, 500, 1 ) );

    return true;
}

int main( void )
{
    if( false == init_game_server() )
    {
        return 0;
    }

    if( false == NetObj.InitNet(1, 1, 3456 ) )
    {
#ifdef _DEBUG
        log( "%s", NetObj.GetErrorMessage() );
#endif
        return 0;
    }
#ifdef _DEBUG
    log( "%s", "\n Server network initialized!" );
    log( "%s", "\n Network thread started! Ready to accept & recevie the message." );
#endif
    Sleep( 1000 ); // Wait for a while to make sure everything is ok.

    g_LoopTimer.GetTimer_sec(); // To initialize the timer.
    struct CatNet::ProcessSession *ToProcessSessoin;
    while( 1 )
    {
        while( nullptr != ( ToProcessSessoin = NetObj.GetProcessList()->GetFirstSession() ) )
        {
            switch( ToProcessSessoin->m_SessionState )
            {
                case CatNet::SESSION_STATE_NEWCONNECTION:
                { // New connection request arrived.
#ifdef _DEBUG
                    log( "\n New connection connected: Index:%d. Total Connection now:%d", ToProcessSessoin->m_SessionIndex, NetObj.GetConnectedCount() );
#endif
					///TODO: Check if more than 3, then need deny
					//check if there is already 3 players
					//deny accept if alr got 3 players
					if (NetObj.GetConnectedCount() > 3)
					{
						//deny access
						SendPacketProcess_FullGame(ToProcessSessoin->m_SessionIndex);
					}
					else
					{
						SendPacketProcess_NewAccept(ToProcessSessoin->m_SessionIndex);
					}
                }
                break;

                case CatNet::SESSION_STATE_CLOSEREDY:
                { // Connection closed arrived or communication error.
#ifdef _DEBUG
                    log( "\n Received: Index %d wants to close or already closed.\n Total Connection now:%d",
                            ToProcessSessoin->m_SessionIndex, NetObj.GetConnectedCount() );
#endif
                    SendPacketProcess_EnemyShipDisconnect( ToProcessSessoin->m_SessionIndex);
                }
                break;

                case CatNet::SESSION_STATE_READPACKET:
                { // Any packet data recevied.
                    ReceviedPacketProcess( ToProcessSessoin );
                }
                break;
            }

            NetObj.GetProcessList()->DeleteFirstSession();
        }

        // You can add your own server codes here. If there is any regular work.

        // Update the asteroid movement.
        float asteroid_prev_x, asteroid_prev_y;
        float time_delta = g_LoopTimer.GetTimer_sec();
        for( auto asteroid : g_AsteroidList )
        {
            asteroid_prev_x = asteroid->get_x();
            asteroid_prev_y = asteroid->get_y();
            asteroid->Update( time_delta, 30, 30 );
            if( ( asteroid_prev_x != asteroid->get_x() ) || ( asteroid_prev_y != asteroid->get_y() ) )
            {
                SendPacketProcess_AsteroidMovement( asteroid );
            }
        }

		//create a speed up every 5s
		static float timer = 0;
		timer += time_delta;
		if (timer >= 10.f)
		{
			timer = 0;
			static int id = 0;
			SpeedUp* speedup = new SpeedUp(id, 400, 300, 0);
			powerUpList.push_back(speedup);
			SendPacketProcess_NewSpeedUp(speedup);
			++id;
			printf("\npower up was created\n");
		}

		for (auto speedup : powerUpList)
		{
			speedup->Update(time_delta, 20, 20);
			SendPacketProcess_SpeedUpMovement(speedup);
		}
		for (std::vector<SpeedUp*>::iterator it = powerUpList.begin(); it != powerUpList.end(); ++it)
		{
			if ((*it)->active == false)
			{
				delete (*it);
				powerUpList.erase(it);
				break;
				printf("\npower up was deleted");
			}
		}
        Sleep( 100 ); // You can check a timer to nomalize the looping spped (FPS).
    }

    return 0;
}