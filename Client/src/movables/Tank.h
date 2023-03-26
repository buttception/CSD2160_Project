#pragma once

#include <string>

#include "movables/Movable.h"

class hgeSprite;
class hgeFont;

class Tank : public Movable
{
public:
	Tank();
	~Tank() = default;

	float turret_rotation;
	float fire_rate, fire_timer;
	int tank_id;
	int rotate, throttle;
	bool active;
	std::string player_name;

	void Render();

private:
	HTEXTURE turretTex_; //!< Handle to the sprite's texture
	hgeSprite* turretSprite_;
};