#ifndef _ASTEROID_H_
#define _ASTEROID_H_
#pragma once


#include "hge.h"
#include "movables.h"
#include <memory>

class hgeSprite;

/**
* The Asteroid class represents a single asteroid object floating in space.
* The size of the current art is 40*40 pixels
*/
class Asteroid : public Movables
{
    public:
        int id_;
        HTEXTURE tex_; //!< Handle to the sprite's texture
        std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the asteroid

    public:
        Asteroid( char* filename );
        Asteroid( char* filename, float x, float y, float angular_velocity );
        ~Asteroid();

    public:
        void Render();
        void Accelerate( float acceleration, float timedelta );

    public:
        void set_id( int ID ) { id_ = ID; }
        int  get_id( void ) { return id_; }
};





#endif
