#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "hge.h"

#include "globals.h"
#include "net\process_packet.h"
#include "net\send_packet.h"
#include "Application.h"

#include <cstdlib>
#include <ctime>

#ifdef _DEBUG
#include <io.h>
#endif

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

#define ABS(n) ( (n < 0) ? (-n) : (n) )

/** 
* Constuctor
*
* Creates an instance of the graphics engine and network engine
*/
Application::Application() : hge_(hgeCreate(HGE_VERSION))
{
	SetGameState( GAMESTATE_NONE );
}

/**
* Destructor
*
* Does nothing in particular apart from calling Shutdown
*/
Application::~Application() throw()
{
	Shutdown();
}

/**
* Initialises the graphics system
* It should also initialise the network system
*/
bool Application::Init()
{
	hge_->System_SetState(HGE_FRAMEFUNC, Application::Loop);
	hge_->System_SetState(HGE_WINDOWED, true);
	hge_->System_SetState(HGE_USESOUND, false);
	hge_->System_SetState(HGE_TITLE, "TankShooter");
	hge_->System_SetState(HGE_LOGFILE, "TankShooter.log");
	hge_->System_SetState( HGE_DONTSUSPEND, true );
	
	if( false == hge_->System_Initiate() )
	{
		return false;
	}
	
	srand( (unsigned int)time(NULL) );

	// Initialize and prepare the game data & systems.
	player = Tank();


	// Initialize the network with Network Library.
	if( !(Net::InitNetwork()) )
	{
		return false;
	}
	SetGameState( GAMESTATE_INITIALIZING );

	return true;
}

/**
* Update cycle
*
* Checks for keypresses:
*   - Esc - Quits the game
*   - Left - Rotates ship left
*   - Right - Rotates ship right
*   - Up - Accelerates the ship
*   - Down - Deccelerates the ship
*
* Also calls Update() on all the ships in the universe
*/
bool Application::Update()
{
	float timedelta = hge_->Timer_GetDelta();

	// Process the packet received from server.
	Net::ProcessPacket(this);

	if (GAMESTATE_INPLAY != GetGameState())
		return false;

	// Check key inputs and process the movements of spaceship.
	if (hge_->Input_GetKeyState(HGEK_ESCAPE))
		return true;

	player.Update(timedelta, player.sprite_->GetWidth(), player.sprite_->GetHeight());

	// TODO: Aim turret with mouse cursor.
	float mouseX, mouseY;
	hge_->Input_GetMousePos(&mouseX, &mouseY);
	float angle = 0.f;

	// Rotate tank left/right.
	if (hge_->Input_GetKeyState(HGEK_A))
	{
		player.rotate = -1;
	}
	else if (hge_->Input_GetKeyState(HGEK_D))
	{
		player.rotate = 1;
	}
	else
	{
		player.rotate = 0;
	}

	// Move tank forward/back.
	if (hge_->Input_GetKeyState(HGEK_S))
	{
		player.throttle = -1;
	}
	else if (hge_->Input_GetKeyState(HGEK_W))
	{
		player.throttle = 1;
	}
	else
	{
		player.throttle = 0;
	}

	// Store inputs into buffer.
	PKT_C2S_TankMovement movePkt;
	PKT_C2S_TankTurret turretPkt;
	movePkt.sequence_id = INT_MIN;		// Set seq ID to invalid state, to check if send_packet has actually set it or not.
	turretPkt.sequence_id = INT_MIN;	// Set seq ID to invalid state, to check if send_packet has actually set it or not.
	
	// Server should handle the simulation, client only sends input data.
	Net::send_packet_movement(player, timedelta, movePkt);
	//Net::send_packet_turret_angle(player, timedelta, angle, turretPkt);

	// Store inputs for (Truth and) Reconciliation.
	if(movePkt.sequence_id != INT_MIN)
		QueuedPlayerMovements.push_back(movePkt);
	if (turretPkt.sequence_id != INT_MIN)
		QueuedPlayerTurret.push_back(turretPkt);

	return false;
}

/**
* Render Cycle
*
* Clear the screen and render all the ships
*/
void Application::Render()
{
	hge_->Gfx_BeginScene();
	hge_->Gfx_Clear(0);

	// Render me.
	if (player.active)
		player.Render();

	hge_->Gfx_EndScene();
}

/** 
* Main game loop
*
* Processes user input events
* Supposed to process network events
* Renders the ships
*
* This is a static function that is called by the graphics
* engine every frame, hence the need to loop through the
* global namespace to find itself.
*/
bool Application::Loop()
{
	Global::application->Render();
	return Global::application->Update();
}

/**
* Shuts down the graphics and network system
*/
void Application::Shutdown()
{
	hge_->System_Shutdown();
	hge_->Release();
}

/** 
* Kick starts the everything, called from main.
*/
void Application::Start()
{
	if (Init())
	{
		hge_->System_Start();
	}
}

template <typename T1, typename T2>
bool Application::HasCollided( T1 &object, T2 &movable )
{
	hgeRect object_collidebox;
	hgeRect movable_Collidebox;

	object->sprite_->GetBoundingBox( object->get_x(), object->get_y(), &object_collidebox );
	movable->sprite_->GetBoundingBox( movable->get_x(), movable->get_y(), &movable_Collidebox );
	return object_collidebox.Intersect( &movable_Collidebox );
}

template <typename Mov, typename Tgt>
bool Application::CheckCollision( Mov &moving_object, Tgt &other, float timedelta )
{
	if( HasCollided( moving_object, other ) )
	{
		other->set_velocity_x( moving_object->get_velocity_x() );
		other->set_velocity_y( moving_object->get_velocity_y() );
		other->set_server_velocity_x( moving_object->get_server_velocity_x() );
		other->set_server_velocity_y( moving_object->get_server_velocity_y() );
		other->Update( timedelta, other->sprite_->GetWidth(), other->sprite_->GetHeight() );

		moving_object->set_velocity_x( moving_object->get_velocity_x() );
		moving_object->set_velocity_y( -moving_object->get_velocity_y() );
		moving_object->set_server_velocity_x( -moving_object->get_server_velocity_x() );
		moving_object->set_server_velocity_y( -moving_object->get_server_velocity_y() );
		moving_object->restore_xy();
		moving_object->Update( timedelta, moving_object->sprite_->GetWidth(), moving_object->sprite_->GetHeight() );

		if( (void *)moving_object == (void *)myship_ )
		{ // If I collided with others, need to send the message to the server.
			Net::send_packet_collided( (Ship *)moving_object );
		}
		else if (MOVABLE_OBJECT_TYPE_ASTEROID == moving_object->get_object_type()) {
			Net::send_packet_asteroid_collided((Asteroid*)moving_object);
		}

		if (MOVABLE_OBJECT_TYPE_ASTEROID == other->get_object_type()) {
			Net::send_packet_asteroid_collided((Asteroid*)other);
		}
		return true;
	}

	return false;
}