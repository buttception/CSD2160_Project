#pragma once

#include <queue>

struct Tank
{
	bool connected;
	int client_id;

	mutable std::queue<int> movement_sequence_IDs;	// Queue of all to-be-processed sequence IDs for tank movement.
	mutable std::queue<int> turret_sequence_IDs;	// Queue of all to-be-processed sequence IDs for tank turret rotation.

	float x, y, w;
	float velocity_x, velocity_y;
	float angular_velocity;
	float turret_rotation;
	float half_extends;

	int hp;
	int max_hp;
};