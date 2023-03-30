#pragma once

#include <string>

#include "movables/Movable.h"

class hgeSprite;
class hgeFont;

class Tank : public Movable
{
public:
	Tank(std::string tankSprite = "tank2.png");
	~Tank() = default;

	float turret_rotation;
	float server_turret_rot;
	float client_turret_rot;
	float frametime;
	float fire_rate, fire_timer;
	int tank_id;
	int rotate, throttle;
	int hp;
	bool active;
	bool missile_shot;
	std::string player_name;

	//int hp;
	int score;
	float respawn_timer;

	void Render();
	bool Update(float timedelta, float spritewidth, float spriteheight);
private:
	HTEXTURE turretTex_; //!< Handle to the sprite's texture
	hgeSprite* turretSprite_;
};