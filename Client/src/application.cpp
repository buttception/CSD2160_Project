#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "hge.h"

#include "globals.h"
#include "net\process_packet.h"
#include "net\send_packet.h"
#include "Application.h"
#include "movables\ship.h"
#include "movables\asteroid.h"
#include "movables\Boom.h"
#include "movables\mine.h"
#include "movables\TimeBomb.h"
#include "movables\SpeedUp.h"

#include <stdlib.h>
#include <time.h>

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
Application::Application() : 
	hge_(hgeCreate(HGE_VERSION))
{
    SetGameState( GAMESTATE_NONE );

	mymissile = NULL;
	keydown_enter = false;

	myMine = NULL;
	keydown_space = false;

	myBomb = NULL;
	keydown_q = false;
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
	hge_->System_SetState(HGE_TITLE, "SpaceShooter");
	hge_->System_SetState(HGE_LOGFILE, "SpaceShooter.log");
    hge_->System_SetState( HGE_DONTSUSPEND, true );
	
    if( false == hge_->System_Initiate() )
    {
        return false;
    }
    
    srand( (unsigned int)time(NULL) );

    // Initialize and prepare the game data & systems.
    // Initialize my own spaceship.
    int shiptype = ( rand() % 4 ) + 1;
    float startx = (float)(( rand() % 600 ) + 100);
    float starty = (float)(( rand() % 400 ) + 100);
    float startw = 0.0f;
    myship_ = new Ship( shiptype, "MyShip", startx, starty, startw );

    // Initialize asteroids.
    //asteroids_.push_back( new Asteroid( "asteroid.png", 100, 100, 1 ) );
    //asteroids_.push_back( new Asteroid( "asteroid.png", 700, 500, 1.5 ) );

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
    Net::ProcessPacket( this );

    if( GAMESTATE_INPLAY == GetGameState() )
    {
		// Update my space ship.
		bool AmIMoving = true;
        // Check key inputs and process the movements of spaceship.
        if( hge_->Input_GetKeyState( HGEK_ESCAPE ) )
            return true;

		//only if the ship is active
		if (myship_->active)
		{
			myship_->set_angular_velocity(0.0f);
			if (hge_->Input_GetKeyState(HGEK_LEFT))
			{
				myship_->set_angular_velocity(myship_->get_angular_velocity() - DEFAULT_ANGULAR_VELOCITY);
			}
			if (hge_->Input_GetKeyState(HGEK_RIGHT))
			{
				myship_->set_angular_velocity(myship_->get_angular_velocity() + DEFAULT_ANGULAR_VELOCITY);
			}
			if (hge_->Input_GetKeyState(HGEK_UP))
			{
				myship_->Accelerate(DEFAULT_ACCELERATION, timedelta);
			}
			if (hge_->Input_GetKeyState(HGEK_DOWN))
			{
				myship_->Accelerate(-DEFAULT_ACCELERATION, timedelta);
			}
			if (hge_->Input_GetKeyState(HGEK_0))
			{
				myship_->stop_moving();
			}
			if (hge_->Input_GetKeyState(HGEK_W)) {
				if (!keydown_enter) {
					CreateMissile(myship_->get_x(), myship_->get_y(), myship_->get_w(), myship_->GetShipID());
					keydown_enter = true;
				}
			}
			else {
				if (keydown_enter) keydown_enter = false;
			}
			if (hge_->Input_GetKeyState(HGEK_E)) {
				if (!keydown_space) {
					CreateMine(myship_->get_x(), myship_->get_y(), myship_->GetShipID());
					keydown_space = true;
				}
			}
			else
				if (keydown_space) keydown_space = false;
			if (hge_->Input_GetKeyState(HGEK_Q))
			{
				if (!keydown_q)
				{
					if (myBomb)
					{
						delete myBomb;
						myBomb = NULL;
					}
					myBomb = new TimeBomb("timebomb.png", myship_->get_x(), myship_->get_y(), myship_->get_w());
					myBomb->ownerid_ = myship_->GetShipID();
					//send packet abt bomb creatin
					Net::send_packet_new_boom(myBomb);
					keydown_q = true;
				}
			}
			else
				if (keydown_q) keydown_q = false;

			if (true == (AmIMoving = myship_->Update(timedelta, myship_->sprite_->GetWidth(), myship_->sprite_->GetHeight())))
			{
				for (auto itr_asteroid : asteroids_)
				{
					if (true == CheckCollision(myship_, itr_asteroid, timedelta))
					{
						myship_->set_collided_with_me(true);
					}
				}
			}
		}
		else
		{
			//when ship not active, do the respawn thing
			myship_->respawnTimer += timedelta;
			if (myship_->respawnTimer > (int)myship_->respawnTimer - timedelta
				&& myship_->respawnTimer < (int)myship_->respawnTimer + timedelta)
				printf("\nRespawning in %d", (int)myship_->respawnTime - (int)myship_->respawnTimer);
			if (myship_->respawnTimer >= myship_->respawnTime)
			{
				myship_->active = true;
				myship_->respawnTimer = 0;
				//send to everyone i respawned
				Net::send_packet_respawn(myship_->GetShipID());
			}
		}

		// Update my missile
		if (mymissile) 
		{
			if (mymissile->Update(timedelta, mymissile->sprite_->GetWidth(), mymissile->sprite_->GetHeight())) {
				//collision check with asteroids
				for (auto itr_asteroid : asteroids_) {
					if (HasCollided(mymissile, itr_asteroid)) {
						//send data to delete other missile
						Net::send_packet_delete_missile(mymissile);
						//make explosion
						Boom* newBoom = new Boom("Boom.png", mymissile->get_x(), mymissile->get_y(), mymissile->get_w());
						particleList.push_back(newBoom);
						Net::send_packet_new_boom(newBoom);
						delete mymissile;
						mymissile = NULL;
						break;
					}
				}
				//check with enemy ships
				if (mymissile) {
					for (auto itr_enemyship : enemyships_) {
						if (HasCollided(mymissile, itr_enemyship)) {
							//send data to delete other missile
							Net::send_packet_delete_missile(mymissile);
							//make explosion
							Boom* newBoom = new Boom("Boom.png", mymissile->get_x(), mymissile->get_y(), mymissile->get_w());
							particleList.push_back(newBoom);
							Net::send_packet_new_boom(newBoom);
							delete mymissile;
							mymissile = NULL;
							//needa tell the enemy i hit him
							Net::send_packet_ship_hit(itr_enemyship->GetShipID(), itr_enemyship->get_x(), itr_enemyship->get_y());
							//set enemy to inactive
							itr_enemyship->active = false;
							break;
						}
					}
				}
			}
		}

		if (myBomb)
		{
			myBomb->lifeTimer += timedelta;
			if (myBomb->lifeTimer >= myBomb->lifeTime) {
				
				Boom* newBoom = new Boom("boom.png", myBomb->get_x(), myBomb->get_y(), myBomb->get_w());
				Net::send_packet_new_boom(newBoom);
				for (auto it : enemyships_)
				{
					if (it->active)
					{
						float x = it->get_x() - myBomb->get_x();
						float y = it->get_y() - myBomb->get_y();
						if (sqrt(x*x + y*y) <= 100.f) {
							it->active = false;
							Net::send_packet_ship_hit(it->GetShipID(), it->get_x(), it->get_y());
						}
					}
				}
				//send bomb died to all
				Net::send_packet_delete_time_bomb(myBomb);
				delete myBomb;
				myBomb = NULL;
			}
		}

        // Update asteroids.
        for( AsteroidList::iterator itr_asteroid = asteroids_.begin(); itr_asteroid != asteroids_.end(); ++itr_asteroid )
        {
            if( true == ( *itr_asteroid )->Update( timedelta, ( *itr_asteroid )->sprite_->GetWidth(), ( *itr_asteroid )->sprite_->GetHeight() ) )
            {
                // Collision check with other asteroids.
                for( AsteroidList::iterator next_asteroid = ( itr_asteroid + 1 ); next_asteroid != asteroids_.end(); ++next_asteroid )
                {
                    CheckCollision( ( *itr_asteroid ), ( *next_asteroid ), timedelta );
                }

                // Collision check with my ship.
                if( false == AmIMoving )
                {
                    if( false == myship_->get_collided_with_me() )
                        CheckCollision( ( *itr_asteroid ), myship_, timedelta );
                    else
                        myship_->set_collided_with_me( false );
                }
            }
        }

        // Update enemy ships.
        for( ShipList::iterator itr_enemyship = enemyships_.begin(); itr_enemyship != enemyships_.end(); ++itr_enemyship )
        {
            ( *itr_enemyship )->Update( timedelta, ( *itr_enemyship )->sprite_->GetWidth(), ( *itr_enemyship )->sprite_->GetHeight() );
        }
		// Update enemy missiles
		for (auto itr_missile : enemymissiles_)
		{
			itr_missile->Update(timedelta, itr_missile->sprite_->GetWidth(),
				itr_missile->sprite_->GetHeight());
		}

		//update particles
		for (ParticleList::iterator it = particleList.begin(); it != particleList.end(); ++it)
		{
			(*it)->Update(timedelta);
			if ((*it)->active == false)
			{
				Boom* temp = (*it);
				it = particleList.erase(it);
				if (it == particleList.end())
					break;
				delete temp;
			}
		}

		if (myMine)
		{
			//check collision for mine
			for (ShipList::iterator it = enemyships_.begin(); it != enemyships_.end(); ++it)
			{
				if ((*it)->active)
				{
					if (myMine)
					{
						Ship* enemy = *it;
						float x = enemy->get_x() - myMine->get_x();
						float y = enemy->get_y() - myMine->get_y();
						//check if ship within dist
						if (sqrt(x*x + y*y) < 100.f)
						{
							//if within the proximity
							enemy->active = false;
							Net::send_packet_ship_hit(enemy->GetShipID(), enemy->get_x(), enemy->get_y());
							//create a boom effect
							Boom* newBoom = new Boom("boom.png", myMine->get_x(), myMine->get_y(), myMine->get_w());
							particleList.push_back(newBoom);
							//send to all
							Net::send_packet_new_boom(newBoom);
							//send to everyone to remove this mine
							Net::send_packet_delete_mine(myMine);
							delete myMine;
							myMine = NULL;
						}
					}
				}
			}
		}

		for (SpeedUpList::iterator it = speedups.begin(); it != speedups.end(); ++it)
		{
			SpeedUp* sp = (*it);
			float x = myship_->get_x() - sp->get_x();
			float y = myship_->get_y() - sp->get_y();
			if (sqrt(x * x + y * y) <= 50.f)
			{
				myship_->powered = true;
				speedups.erase(it);
				Net::send_packet_delete_speed_up(sp);
				delete sp;
				sp = NULL;
				break;
			}
		}

		if (myship_->powered) {
			myship_->powerTimer += timedelta;
			myship_->modifier = 2;
			if (myship_->powerTimer >= myship_->powerTime)
			{
				printf("\nBOOSTO ENDO %f", myship_->powerTimer);
				myship_->powerTimer = 0;
				myship_->powered = false;
				myship_->modifier = 1;
			}
			printf("\nBOOSTO %f", myship_->powerTimer);
		}

        Net::send_packet_myship_movement( myship_ );
    }

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

    // Render my space ship.
	if (myship_->active)
		myship_->Render();

    // Render enemy ships.
    for( auto enemyship : enemyships_ ) if(enemyship->active) enemyship->Render();

    // Render asteroids.
    for( auto asteroid : asteroids_ ) asteroid->Render();

	// Render enemy missiles
	for (auto enemymissile : enemymissiles_) enemymissile->Render();

	// Render my missile
	if (mymissile) mymissile->Render();

	for (auto it : enemymines_) it->Render();
	if (myMine) myMine->Render();

	//render the booms
	for (auto boom : particleList) boom->Render();

	for (auto bomb : enemybombs_) bomb->Render();
	if (myBomb) myBomb->Render();

	for (auto it : speedups) it->Render();

	hge_->Gfx_EndScene();
}

void Application::CreateMissile(float x, float y, float w, int id)
{
	if (mymissile)
	{ // delete existing missile
		delete mymissile;
		mymissile = 0;
	}
	// add a new missile based on the following parameter coordinates
	mymissile = new Missile("missile.png", x, y, w, id);

	// send new missile information to the server
	Net::send_packet_new_missile(mymissile);
}

void Application::CreateMine(float x, float y, int id)
{
	if (myMine)
	{
		//delete existing mineo
		delete myMine;
		myMine = NULL;
	}
	myMine = new Mine("mine.png", x, y, (float)id);
	Net::send_packet_new_mine(myMine);
}

Ship * Application::FindEnemyShip( int ShipID )
{
    for( auto enemyship : enemyships_ )
    {
        if( ShipID == enemyship->GetShipID() )
        {
            return enemyship;
        }
    }

    return nullptr;
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