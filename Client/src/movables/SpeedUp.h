#ifndef _SPEEDUP_H
#define _SPEEDUP_H

#include <memory>

#include "hge.h"
#include "hgerect.h"
#include "movables.h"

class hgeSprite;

class SpeedUp : public Movables
{
public:
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::unique_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship
	int id;

public: // Constructors and Destructor.
	SpeedUp(char* filename, float x, float y, float w);
	~SpeedUp();

public: // Interface functions.
	void Render();
};

#endif