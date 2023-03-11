#ifndef _TIMEBOMB_H
#define _TIMEBOMB_H

#include <memory>

#include "hge.h"
#include "hgerect.h"
#include "movables.h"

class hgeSprite;

class TimeBomb : public Movables
{
public:
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::unique_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship

	float lifeTime;
	float lifeTimer;
	int ownerid_;

public: // Constructors and Destructor.
	TimeBomb(char* filename, float x, float y, float w);
	~TimeBomb();

public: // Interface functions.
	void Render();
};

#endif