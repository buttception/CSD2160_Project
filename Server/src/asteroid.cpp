#include "main.h"
#include "asteroid.h"

_Asteroid::_Asteroid()
{
    this->x = 0.0f;
    this->y = 0.0f;
    this->velocity_x = 0.0f;
    this->velocity_y = 0.0f;
    this->angular_velocity = 0.0f;
}

_Asteroid::_Asteroid( int ID, float x, float y, float angular_velocity )
{
    this->ID = ID;
    this->x = x;
    this->y = y;
    this->velocity_x = 0.0f;
    this->velocity_y = 0.0f;
    this->angular_velocity = angular_velocity;
}

_Asteroid::~_Asteroid()
{
}

void _Asteroid::Update( float timedelta, float spritewidth, float spriteheight )
{
    float pi = 3.141592654f * 2;

    set_x( get_x() + ( get_velocity_x() * timedelta ) );
    set_y( get_y() + ( get_velocity_y() * timedelta ) );

    float screenwidth = CLIENT_SCREEN_WIDTH;
    float screenheight = CLIENT_SCREEN_HEIGHT;

    if( get_x() < ( -spritewidth / 2 ) )
    {
        set_x( get_x() + ( screenwidth + spritewidth ) );
    }
    else if( get_x() > ( screenwidth + ( spritewidth / 2 ) ) )
    {
        set_x( get_x() - ( screenwidth + spritewidth ) );
    }

    if( get_y() < ( -spriteheight / 2 ) )
    {
        set_y( get_y() + ( screenheight + spriteheight ) );
    }
    else if( get_y() > ( screenheight + ( spriteheight / 2 ) ) )
    {
        set_y( get_y() - ( screenheight + spriteheight ) );
    }
}