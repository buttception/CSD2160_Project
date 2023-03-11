#ifndef _SHIP_H_
#define _SHIP_H_

#include "hge.h"
#include "movables.h"
#include <memory>
#include <iostream>
#include <string>

class hgeSprite;
class hgeFont;

/**
* The Ship class represents a single spaceship floating in space.
* The size of the current art is 128*128 pixels
*/
class Ship : public Movables
{
    private:
        std::string shipname_;
        int shipid_;           //!< Same number of SessionIndex (from Network Library).
        int shiptype_;

    public:
    	HTEXTURE tex_; //!< Handle to the sprite's texture
    	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship
        std::auto_ptr<hgeFont> font_;
		bool active;
		float respawnTimer;
		float respawnTime;

		float powerTime;
		float powerTimer;
		bool powered;

		int modifier;

    public:
    	Ship( int shiptype = 1 );
        Ship( int shiptype, std::string shipname, float startx = 400, float starty = 300, float startw = 0 );
    	~Ship();

    public:
    	void Render();
    	void Accelerate(float acceleration, float timedelta);

        void        SetShipName            ( std::string shipname );
        std::string GetShipName            ( void                 );
        void        SetShipID              ( int shipid           ) { shipid_ = shipid; }
        int         GetShipID              ( void                 ) { return shipid_;   }
        void        SetShipType            ( int shiptype         ) { shiptype_ = shiptype;  }
        int         GetShipType            ( void                 ) { return shiptype_; }
};

#endif