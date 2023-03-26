#pragma once
#include "movables/Movable.h"

class hgeSprite;
class hgeFont;

struct Missile : public Movable
{
	int missile_id;
	int owner_id;

	Missile(float _x, float _y, float _w, float _velX, float _velY, int _ownerID);
	~Missile() = default;

	void Render();
};
