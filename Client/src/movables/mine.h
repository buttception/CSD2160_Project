#ifndef _MINE_H
#define _MiNE_H

#include "hge.h"
#include "hgerect.h"
#include "movables.h"
#include <memory>

class hgeSprite;

class Mine : public Movables
{
public:
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship

	int ownerid_;
	bool active;

public: // Constructors and Destructor.
	Mine(char* filename, float x, float y, float ownerId);
	~Mine();

public: // Interface functions.
	void Render();

};

#endif