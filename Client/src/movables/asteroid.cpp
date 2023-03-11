#include "..\globals.h"
#include "asteroid.h"
#include "hge.h"
#include "hgeSprite.h"
#include "hgefont.h"
#include <math.h>

/**
* Asteroid Constructor
*
* It will load the file specified into a sprite and intialise its
* hotspot to the center. Assumes a sprite size of 30*30 and a
* screen size of 800*600
*
* @param filename Name of the graphics file used to represent the Asteroid
*/

Asteroid::Asteroid( char* filename )
{
    HGE* hge = hgeCreate( HGE_VERSION );

    set_object_type( MOVABLE_OBJECT_TYPE_ASTEROID );

    tex_ = hge->Texture_Load( filename );
    hge->Release();
    sprite_.reset( new hgeSprite( tex_, 0, 0, 30, 30 ) );
    sprite_->SetHotSpot( 15, 15 );

    set_x( 200 );
    set_y( 200 );
}

Asteroid::Asteroid( char * filename, float x, float y, float angular_velocity )
{
    HGE* hge = hgeCreate( HGE_VERSION );

    set_object_type( MOVABLE_OBJECT_TYPE_ASTEROID );

    tex_ = hge->Texture_Load( filename );
    hge->Release();
    sprite_.reset( new hgeSprite( tex_, 0, 0, 30, 30 ) );
    sprite_->SetHotSpot( 15, 15 );

    set_x( x );
    set_y( y );
    set_server_xyw( x, y, angular_velocity );
    set_client_xyw( x, y, angular_velocity );
    set_angular_velocity( angular_velocity );
    set_ratio( 1.0f );
}


/**
* Asteroid Destructor
*
* Frees the internal texture used by the sprite
*/
Asteroid::~Asteroid()
{
    HGE* hge = hgeCreate( HGE_VERSION );
    hge->Texture_Free( tex_ );
    hge->Release();
}

/**
* Render Cycle
*
* Renders the Asteroid to the screen. Must be called between a
* Gfx_BeginScene an Gfx_EndScene, otherwise bad things will
* happen.
*/
void Asteroid::Render()
{
    sprite_->RenderEx( get_x(), get_y(), get_w() );
}

/**
* Accelerates a Asteroid by the given acceleration (i.e. increases
* the asteroids velocity in the direction it is pointing in)
*
* @param acceleration How much to accelerate by in px/s^2
* @param timedelta Time passed since last frame
*/
void Asteroid::Accelerate( float acceleration, float timedelta )
{
    set_velocity_x( get_velocity_x() + ( acceleration * cosf( get_w() ) * timedelta ) );
    set_velocity_y( get_velocity_y() + ( acceleration * sinf( get_w() ) * timedelta ) );
}

