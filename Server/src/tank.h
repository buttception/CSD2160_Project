#pragma once

struct Tank
{
	bool connected;
	float x, y, w;
	float velocity_x, velocity_y;
	float angular_velocity;
	float turret_rotation;
	float half_extends;

	int hp;
	int max_hp;
};