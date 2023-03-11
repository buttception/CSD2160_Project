#include "..\globals.h"
#include "ship.h"
#include "hge.h"
#include "hgeSprite.h"
#include "hgefont.h"
#include <math.h>

#define RESPAWNTIME 10.f;

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif

/**
* Ship Constructor
*
* It will load the file specified into a sprite and intialise its
* hotspot to the center. Assumes a sprite size of 128*128 and a
* screen size of 800*600
*
* @param filename Name of the graphics file used to represent the ship
*/

Ship::Ship( int shiptype )
{
	HGE* hge = hgeCreate(HGE_VERSION);

    set_object_type( MOVABLE_OBJECT_TYPE_SPACESHIP );

    std::string shipfilename = "ship" + std::to_string( shiptype_ ) + ".png";
	tex_ = hge->Texture_Load(shipfilename.c_str());
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 64, 64));
	sprite_->SetHotSpot(32,32);

    set_x( 400 );
    set_y( 300 );
    set_w( 0 );
    set_server_xyw( 400, 300, 0 );
    set_client_xyw( 400, 300, 0 );
    set_ratio( 1.0f );
	modifier = 1;
	powered = false;
	powerTimer = 0;
	powerTime = 5;

    font_.reset( new hgeFont( "font1.fnt" ) );
    font_->SetScale( 0.5 );

    shipname_ = "MyShip";
    shipid_ = 0;
	active = true;
	respawnTime = RESPAWNTIME;
	respawnTimer = 0;
}

Ship::Ship( int shiptype, std::string shipname, float startx, float starty, float startw )
{
    HGE* hge = hgeCreate( HGE_VERSION );

    set_object_type( MOVABLE_OBJECT_TYPE_SPACESHIP );

    std::string shipfilename = "ship" + std::to_string( shiptype ) + ".png";
	tex_ = hge->Texture_Load(shipfilename.c_str());
    hge->Release();
    sprite_.reset( new hgeSprite( tex_, 0, 0, 64, 64 ) );
    sprite_->SetHotSpot( 32, 32 );

    shiptype_ = shiptype;
    set_x( startx );
    set_y( starty );
    set_w( startw );
    set_server_xyw( startx, starty, startw );
    set_client_xyw( startx, starty, startw );
    set_ratio( 1.0f );
	modifier = 1;
	powered = false;
	powerTimer = 0;
	powerTime = 5;

    font_.reset( new hgeFont( "font1.fnt" ) );
    font_->SetScale( 0.5 );

    shipname_ = shipname;
    shipid_ = 0;
	active = true;
	respawnTime = RESPAWNTIME;
	respawnTimer = 0;
}


/**
* Ship Destructor
*
* Frees the internal texture used by the sprite
*/
Ship::~Ship()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}


/**
* Render Cycle
*
* Renders the ship to the screen. Must be called between a
* Gfx_BeginScene an Gfx_EndScene, otherwise bad things will
* happen.
*/
void Ship::Render()
{
    // render the ship.
	sprite_->RenderEx( get_x(), get_y(), get_w());
    // print the ship name.
    font_->printf( get_x(), get_y()+20, HGETEXT_LEFT, "%s", shipname_.c_str() );
}

/**
* Accelerates a ship by the given acceleration (i.e. increases
* the ships velocity in the direction it is pointing in)
*
* @param acceleration How much to accelerate by in px/s^2
* @param timedelta Time passed since last frame
*/
void Ship::Accelerate(float acceleration, float timedelta)
{
    set_server_velocity_x( get_server_velocity_x() + ( acceleration * cosf( get_w() ) * timedelta * modifier) );
    set_server_velocity_y( get_server_velocity_y() + ( acceleration * sinf( get_w() ) * timedelta * modifier) );
}

void Ship::SetShipName( std::string shipname )
{
    shipname_ = shipname;
}

std::string Ship::GetShipName( void )
{
    return shipname_;
}

