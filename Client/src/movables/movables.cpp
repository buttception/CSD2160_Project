#include "movables.h"

Movables::Movables()
{
    ZeroMemory( &now_, sizeof( now_ ) );
    ZeroMemory( &prev_, sizeof( prev_ ) );

    server_x_ = 0;
    server_y_ = 0;
    server_w_ = 0;
    server_velocity_x = 0;
    server_velocity_y = 0;
    client_x_ = 0;
    client_y_ = 0;
    client_w_ = 0;
    ratio_ = 1.0f;
    collided_with_me_ = false;
}

Movables::~Movables()
{
}

void Movables::stop_moving( void )
{
    set_velocity_x( 0.0f );
    set_velocity_y( 0.0f );
    set_server_velocity_x( 0.0f );
    set_server_velocity_y( 0.0f );
    set_angular_velocity( 0.0f );
}

void Movables::do_interpolate_update()
{
    client_x_ = now_.x_;
    client_y_ = now_.y_;
    client_w_ = now_.w_;
    set_velocity_x( server_velocity_x );
    set_velocity_y( server_velocity_y );
    ratio_ = 0;
}

bool Movables::Update( float timedelta, float spritewidth, float spriteheight )
{
    // Calculate the movement.
    HGE* hge = hgeCreate( HGE_VERSION );
    float pi = 3.141592654f * 2;

    float prev_x = get_x();
    float prev_y = get_y();
    float prev_w = get_w();

    // change the way angular velocity is updated (by ratio)
    set_server_w( get_server_w() + ( get_angular_velocity() * timedelta ) );
    if( get_server_w() > pi )   set_server_w( get_server_w() - pi );
    if( get_server_w() < 0.0f ) set_server_w( get_server_w() + pi );

    set_client_w( get_client_w() + ( get_angular_velocity() * timedelta ) );
    if( get_client_w() > pi )   set_client_w( get_client_w() - pi );
    if( get_client_w() < 0.0f ) set_client_w( get_client_w() + pi );

    set_w( ( get_ratio() * get_server_w() ) + ( ( 1 - get_ratio() ) * get_client_w() ) );
    if( get_w() > pi )   set_w( get_w() - pi );
    if( get_w() < 0.0f ) set_w( get_w() + pi );

    float screenwidth = static_cast<float>( hge->System_GetState( HGE_SCREENWIDTH ) );
    float screenheight = static_cast<float>( hge->System_GetState( HGE_SCREENHEIGHT ) );

    set_server_x( get_server_x() + ( get_server_velocity_x() * timedelta ) );
    set_server_y( get_server_y() + ( get_server_velocity_y() * timedelta ) );

    // change the way movement is updated by using ratio.
    if( get_server_x() < -spritewidth / 2 )
        set_server_x( get_server_x() + ( screenwidth + spritewidth ) );
    else if( get_server_x() > screenwidth + spritewidth / 2 )
        set_server_x( get_server_x() - ( screenwidth + spritewidth ) );

    if( get_server_y() < -spriteheight / 2 )
        set_server_y( get_server_y() + ( screenheight + spriteheight ) );
    else if( get_server_y() > screenheight + spriteheight / 2 )
        set_server_y( get_server_y() - ( screenheight + spriteheight ) );

    set_client_x( get_client_x() + ( get_velocity_x() * timedelta ) );
    set_client_y( get_client_y() + ( get_velocity_y() * timedelta ) );

    if( get_client_x() < -spritewidth / 2 )
        set_client_x( get_client_x() + ( screenwidth + spritewidth ) );
    else if( get_client_x() > screenwidth + spritewidth / 2 )
        set_client_x( get_client_x() - ( screenwidth + spritewidth ) );

    if( get_client_y() < -spriteheight / 2 )
        set_client_y( get_client_y() + ( screenheight + spriteheight ) );
    else if( get_client_y() > screenheight + spriteheight / 2 )
        set_client_y( get_client_y() - ( screenheight + spriteheight ) );

    if( ( ( get_server_x() < -spritewidth / 2 ) && ( get_client_x() > ( screenwidth + ( spritewidth / 2 ) ) ) ) ||
        ( ( get_server_x() > screenwidth + ( spritewidth / 2 ) ) && ( get_client_x() < -spritewidth / 2 ) ) )
    {
        set_x( get_server_x() );
    }
    else
    {
        set_x( ( get_ratio() * get_server_x() ) + ( ( 1 - get_ratio() ) * get_client_x() ) );
    }

    if( ( ( get_server_y() < -spriteheight / 2 ) && ( get_client_y() > ( screenheight + ( spriteheight / 2 ) ) ) ) ||
        ( ( get_server_y() > screenheight + ( spriteheight / 2 ) ) && ( get_client_y() < -spriteheight / 2 ) ) )
    {
        set_y( get_server_y() );
    }
    else
    {
        set_y( ( get_ratio() * get_server_y() ) + ( ( 1 - get_ratio() ) * get_client_y() ) );
    }

    if( get_ratio() < 1 )
    {
        // interpolating ratio step
        set_ratio( get_ratio() + ( timedelta * 4 ) );
        if( get_ratio() > 1 ) set_ratio( 1 );
    }

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

    if( ( get_x() != prev_x ) || ( get_y() != prev_y ) ) return true; // something updated.

    return false; // nothing updated.
}

bool Movables::compare_xyw( void )
{
    if( ( now_.x_ != prev_.x_ ) || ( now_.y_ != prev_.y_ ) || ( now_.w_ != prev_.w_ ) )
        return false;
    return true;
}