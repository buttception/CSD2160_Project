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

#define CLIENT_SCREEN_WIDTH  800
#define CLIENT_SCREEN_HEIGHT 600

namespace
{
	constexpr float TANK_ROT_SPEED = 5.f;
	constexpr float TANK_MOV_SPEED = 100.f;
	constexpr float MISSILE_COOLDOWN = 1.f;
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

#define ABS(n) ( (n < 0) ? (-n) : (n) )

namespace
{
	float Interpolate(float start, float end, float t)
	{
		if (t > 1.f)
			t = 1.f;
		if (t < 0.f)
			t = 0.f;

		return start + (end - start) * t;
	}
}

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
	hge_->System_SetState(HGE_HIDEMOUSE, false);

	if( false == hge_->System_Initiate() )
	{
		return false;
	}
	
	srand( (unsigned int)time(NULL) );

	// Initialize and prepare the game data & systems.
	for (int i{}; i < MECHANISMS::MCH_COUNT; ++i)
		isMechanism[i] = true;
	red = green = 0;

	red = SETR(red, 255);
	red = SETG(red, 0);
	red = SETB(red, 0);
	red = SETA(red, 255);

	green = SETR(green, 0);
	green = SETG(green, 255);
	green = SETB(green, 0);
	green = SETA(green, 255);

	color = ARGB(255, 125, 0, 0);

	button = new hgeSprite(0, 0, 0, 64, 32);
	button->SetTexture(0);

	buttonFont = new hgeFont("font1.fnt");
	buttonFont->SetScale(0.5);

	menuPos[0] = pos{ CLIENT_SCREEN_WIDTH * 0.5f, CLIENT_SCREEN_HEIGHT * 0.5f };
	menuPos[1] = pos{ CLIENT_SCREEN_WIDTH * 0.5f, CLIENT_SCREEN_HEIGHT * 0.5f  + 64};

	player = Tank();

	missile_cooldown = MISSILE_COOLDOWN;

	// Initialize the network with Network Library.
	if( !(Net::InitNetwork()) )
	{
		return false;
	}
	SetGameState( GAMESTATE_INITIALIZING );

	std::cout << "Client->Server packet sizes:\n";
	std::cout << "PKT_C2S_EnterGame    " << sizeof(PKT_C2S_EnterGame) << std::endl;
	std::cout << "PKT_C2S_TankMovement " << sizeof(PKT_C2S_TankMovement) << std::endl;
	std::cout << "PKT_C2S_TankTurret   " << sizeof(PKT_C2S_TankTurret) << std::endl;
	std::cout << "PKT_C2S_Disconnect   " << sizeof(PKT_C2S_Disconnect) << std::endl;
	std::cout << "PKT_C2S_ClickStart   " << sizeof(PKT_C2S_ClickStart) << std::endl;

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
	
	if (missile_cooldown <= MISSILE_COOLDOWN)
		missile_cooldown += timedelta;

	// Process the packet received from server.
	Net::ProcessPacket(this);

	// Check key inputs and process the movements of spaceship.
	if (hge_->Input_GetKeyState(HGEK_ESCAPE))
		return true;

	static bool isLClick = false;
	if (hge_->Input_GetKeyState(HGEK_LBUTTON) && !isLClick)
	{
		isLClick = true;
		if(GAMESTATE_INPLAY == GetGameState())
		{
			for (int i{}; i < 3; ++i)
			{
				float x = static_cast<float>(10 * i + i * 64), y = 10;
				if (ButtonCollision(x, y, 64, 32))
				{
					isMechanism[i] ^= true;
					if (i == MCH_CLIENT_PREDICTION && !isMechanism[MCH_CLIENT_PREDICTION])
						isMechanism[MCH_RECONCILIATION] = false;
					else if (i == MCH_RECONCILIATION && isMechanism[MCH_RECONCILIATION])
						isMechanism[MCH_CLIENT_PREDICTION] = true;
				}
			}
		}
		else if(GAMESTATE_MENU == GetGameState())
		{
			for(int i{}; i < 2; ++i)
			{
				if (ButtonCollision(menuPos[i].x, menuPos[i].y, 64, 32))
				{
					if(i == 0)
					{
						//enter game
						player.active = true;
						SetGameState(GAMESTATE_INPLAY);
						//send to server to say this client is in play state
						Net::send_packet_click_start(player);
						player.missile_shot = false;
					}
					else
					{
						return true; //quit
					}
				}
			}
		}
	}
	else if (!hge_->Input_GetKeyState(HGEK_LBUTTON) && isLClick)
	{
		isLClick = false;
	}

	if(GAMESTATE_MENU == GetGameState())
	{
		
	}
	else if (GAMESTATE_INPLAY == GetGameState())
	{
		if(!player.active)
		{
			player.respawn_timer -= timedelta;
			if (player.respawn_timer <= 0.f)
				player.respawn_timer = 2.f;
		}

		// set the flags for client prediction/recon/interpo
		static bool isDown1 = false;
		static bool isDown2 = false;
		static bool isDown3 = false;
		if (hge_->Input_GetKeyState(HGEK_1) && !isDown1)
		{
			isMechanism[MCH_CLIENT_PREDICTION] ^= true;
			if (!isMechanism[MCH_CLIENT_PREDICTION])
				isMechanism[MCH_RECONCILIATION] = false;
			isDown1 = true;
		}
		else if (isDown1 && !hge_->Input_GetKeyState(HGEK_1))
		{
			isDown1 = false;
		}
		if (hge_->Input_GetKeyState(HGEK_2) && !isDown2)
		{
			isMechanism[MCH_RECONCILIATION] ^= true;
			if (isMechanism[MCH_RECONCILIATION])
				isMechanism[MCH_CLIENT_PREDICTION] = true;
			isDown2 = true;
		}
		else if (isDown2 && !hge_->Input_GetKeyState(HGEK_2))
		{
			isDown2 = false;
		}
		if (hge_->Input_GetKeyState(HGEK_3) && !isDown3)
		{
			isMechanism[MCH_INTERPOLATE] ^= true;
			isDown3 = true;
		}
		else if (isDown3 && !hge_->Input_GetKeyState(HGEK_3))
		{
			isDown3 = false;
		}

		// Apply reconciliation.
		float rotW;
		float velX, velY;
		float posX, posY;
		posX = player.get_server_x();	// Extrapolate client's position based off server's authoritative "you are here" position.
		posY = player.get_server_y();	// Extrapolate client's position based off server's authoritative "you are here" position.
		rotW = player.get_server_w();	// Extrapolate client's rotation based off server's authoritative "you are here" rotation.

		// Apply queued movement inputs to player position.
		if (isMechanism[MCH_CLIENT_PREDICTION])
		{
			for (const auto& temp : this->QueuedPlayerMovements)
			{
				rotW += (float)temp.rotate * TANK_ROT_SPEED * temp.frameTime;
				velX = cos(rotW) * (float)temp.throttle;
				velY = sin(rotW) * (float)temp.throttle;
				posX += velX * TANK_MOV_SPEED * temp.frameTime;
				posY += velY * TANK_MOV_SPEED * temp.frameTime;
				if (posX > CLIENT_SCREEN_WIDTH)
					posX -= CLIENT_SCREEN_WIDTH;
				else if (posX < 0)
					posX = CLIENT_SCREEN_WIDTH - posX;
				if (posY > CLIENT_SCREEN_HEIGHT)
					posY -= CLIENT_SCREEN_HEIGHT;
				else if (posY < 0)
					posY = CLIENT_SCREEN_HEIGHT - posY;
			}
		}

		// This will be the new client prediction
		player.set_client_x(posX);
		player.set_client_y(posY);
		player.set_client_w(rotW);

		// Old predicted position is the current player position
		float newX{ posX }, newY{ posY }, newW{ rotW };
		if (isMechanism[MCH_INTERPOLATE])
		{
			newX = player.get_x();
			newY = player.get_y();
			newW = player.get_w();

			// X-axis.
			if (abs(newX - player.get_client_x()) > FLT_EPSILON)
			{
				newX = Interpolate(newX, player.get_client_x(), 0.2f);
			}
			// Y-Axis.
			if (abs(newY - player.get_client_y()) > FLT_EPSILON)
			{
				newY = Interpolate(newY, player.get_client_y(), 0.2f);
			}
			// Rotation.
			if (abs(newW - player.get_client_w()) > FLT_EPSILON)
			{
				newW = Interpolate(newW, player.get_client_w(), 0.2f);
			}
		}
		// Apply interpolated value.
		player.set_x(newX);
		player.set_y(newY);
		player.set_w(newW);

		// Aim turret with mouse cursor.
		float mouseX, mouseY;
		hge_->Input_GetMousePos(&mouseX, &mouseY);
		const float angle = atan2f(mouseY - player.get_y(), mouseX - player.get_x());
		// reconciliation
		float turrRot = player.server_turret_rot;
		if (isMechanism[MCH_CLIENT_PREDICTION])
		{
			for (const auto& temp : this->QueuedPlayerTurret)
			{
				turrRot = temp.angle;
			}
		}

		// client predication
		player.client_turret_rot = turrRot;

		// old prediction
		//float newTurrRot = player.turret_rotation;
		float newTurrRot = turrRot;
		// Turret Rotation.
		if (isMechanism[MCH_INTERPOLATE])
		{
			newTurrRot = player.turret_rotation;
			if (abs(newTurrRot - player.client_turret_rot) > FLT_EPSILON)
			{
				newTurrRot = Interpolate(newTurrRot, player.client_turret_rot, 0.2f);
			}
		}
		player.turret_rotation = newTurrRot;

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

		// Shoot missile.
		if(hge_->Input_GetKeyState(HGEK_SPACE) && missile_cooldown >= MISSILE_COOLDOWN)
		{
			player.missile_shot = true;
			missile_cooldown = 0.f;
		}
		//else
		//{
		//	player.missile_shot = false;
		//}

		// Store inputs into buffer.
		PKT_C2S_TankMovement movePkt;
		PKT_C2S_TankTurret turretPkt;
		movePkt.sequence_id = INT_MIN;		// Set seq ID to invalid state, to check if send_packet has actually set it or not.
		turretPkt.sequence_id = INT_MIN;	// Set seq ID to invalid state, to check if send_packet has actually set it or not.

		// Server should handle the simulation, client only sends input data.
		Net::send_packet_movement(player, angle, movePkt, turretPkt);

		// Store inputs for (Truth and) Reconciliation.
		if (movePkt.sequence_id != INT_MIN)
			QueuedPlayerMovements.push_back(movePkt);
		if (turretPkt.sequence_id != INT_MIN)
			QueuedPlayerTurret.push_back(turretPkt);


		// OTHER CLIENTS
		for (auto& tank : clients)
		{
			if(tank.active)
			tank.Update(timedelta, tank.sprite_->GetWidth(), tank.sprite_->GetHeight());
		}

		// OTHER MISSILES
		for(auto& missile : missiles)
		{
			if(missile.second.alive)
				missile.second.Update(timedelta, missile.second.sprite_->GetWidth(), missile.second.sprite_->GetHeight());
		}
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

	if (GAMESTATE_MENU == GetGameState())
	{
		button->SetColor(color);
		buttonFont->SetScale(1.f);
		buttonFont->printf(CLIENT_SCREEN_WIDTH * 0.5f - 32, CLIENT_SCREEN_HEIGHT * 0.5f - 128, HGETEXT_LEFT, "Tank & Tank");
		buttonFont->SetScale(0.75f);
		button->RenderEx(menuPos[0].x, menuPos[0].y, 0);
		button->RenderEx(menuPos[1].x, menuPos[1].y, 0);
		buttonFont->printf(menuPos[0].x, menuPos[0].y, HGETEXT_LEFT, "START");
		buttonFont->printf(menuPos[1].x, menuPos[1].y, HGETEXT_LEFT, "QUIT");
		buttonFont->SetScale(0.5f);
	}
	else if(GAMESTATE_INPLAY == GetGameState())
	{
		// Render me.
		if (player.active)
			player.Render();

		for (auto& tank : clients)
		{
			if (tank.active)
			tank.Render();
		}

		for (auto& missile : missiles)
		{
			if (missile.second.alive)
				missile.second.Render();
		}

		// draw the buttons (client prediction, reconciliation, entity interpolation)
		for (int i{}; i < 3; ++i)
		{
			if (isMechanism[i])
				button->SetColor(green);
			else
				button->SetColor(red);

			float x = static_cast<float>(10 * i + i * 64), y = 10;
			button->RenderEx(x, y, 0);
			if (i == MCH_CLIENT_PREDICTION)
				buttonFont->printf(x, y, HGETEXT_LEFT, "Client \nPrediction");
			else if (i == MCH_RECONCILIATION)
				buttonFont->printf(x, y, HGETEXT_LEFT, "Reconci \n-liation");
			else
				buttonFont->printf(x, y, HGETEXT_LEFT, "Interpo \n-lation");
		}
		buttonFont->SetScale(0.7f);
		buttonFont->printf(CLIENT_SCREEN_WIDTH * 0.5f - 32, 32, HGETEXT_LEFT, "Score: %d", player.score);
		buttonFont->SetScale(0.5f);

		if(!player.active)
		{
			buttonFont->printf(CLIENT_SCREEN_WIDTH * 0.5f - 32, 64, HGETEXT_LEFT, "Respawn: %.6f", player.respawn_timer);
		}
	}
	else
	{
		buttonFont->printf(CLIENT_SCREEN_WIDTH * 0.5f, CLIENT_SCREEN_HEIGHT * 0.5f, HGETEXT_LEFT, "Not in any state. \n Game is either full or an error has occured.");
	}

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
	Net::send_packet_disconnect(player);
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

bool Application::ButtonCollision(float px, float py, float w, float h)
{
	float mx, my;
	hge_->Input_GetMousePos(&mx, &my);
	//std::cout << mx << "," << my << "\n";
	if (mx <= px + w && mx >= px &&
		my <= py + h && my >= py)
		return true;

	return false;
}