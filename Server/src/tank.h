#pragma once

#include <queue>

struct Tank
{
	struct InputData
	{
		int movement_sequence_ID;
		int rotate;
		int throttle;
		float frametime;
	};

	struct TurretInputData
	{
		int turret_sequence_ID;
		float angle;
		bool missile_shot;
	};
	
	struct MissileData
	{
		int turret_sequence_ID;
		float angle;
		bool missile_shot;
	};

	bool active;
	bool connected;
	int client_id;
	int latest_sequence_ID;
	int latest_turret_seq_ID;

	mutable std::queue<InputData> input_queue;	// Queue of all to-be-processed sequence IDs for tank movement.
	mutable std::queue<TurretInputData> turret_input_queue;	// Queue of all to-be-processed sequence IDs for tank turret rotation.
	mutable std::queue<MissileData> missile_queue;	// Queue of all to-be-processed sequence IDs for tank turret rotation.

	float x, y, w;
	float velocity_x, velocity_y;
	float angular_velocity;
	float turret_rotation;
	float sprite_size_x, sprite_size_y;
	bool missile_shot;

	int hp;
	int max_hp;
	int score;
	float respawn_timer;
};