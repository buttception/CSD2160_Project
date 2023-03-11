#ifndef _BOOM_H
#define _BOOM_H

#include "hge.h"
#include "hgerect.h"
#include "movables.h"
#include <memory>

class hgeSprite;

class Boom : public Movables
{
public:
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship

	float lifeTime;
	float lifeTimer;
	bool active;

public: // Constructors and Destructor.
	Boom(char* filename, float x, float y, float w);
	~Boom();

public: // Interface functions.
	void Render();
	void Update(float _dt);
};

#endif