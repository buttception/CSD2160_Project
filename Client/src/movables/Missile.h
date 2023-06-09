﻿#pragma once
#include "movables/Movable.h"

class hgeSprite;
class hgeFont;

struct Missile : public Movable
{
	int missile_id;
	int owner_id;
	bool alive;

	Missile(float _x, float _y, float _w, float _velX, float _velY, int missile_id, int _ownerID);
	~Missile() = default;

	void Render();
};
