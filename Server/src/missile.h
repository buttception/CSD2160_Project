#pragma once

struct Missile
{
	float x, y, w; // position of the missile
	float velocity_x, velocity_y;
	int owner_id;
	int id;
	
	// constructor that take the posiiton of the turret to set the intitial
	// position of the missile
	Missile(float _x, float _y, float _w, float _velX, float _velY, int _ownerID)
		: x{ _x }, y{ _y }, w{ _w }, velocity_x{ _velX }, velocity_y{ _velY }
		, owner_id{ _ownerID }, id{}
	{
		static int idCount;
		id = ++idCount;
	}
};

