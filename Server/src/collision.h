#pragma once
#include <cmath>

struct Collision
{
	struct Circle
	{
		float x, y, radius, velX, velY;
		Circle(float _x, float _y, float _radius, float _velX, float _velY)
			: x{_x}, y{_y}, radius{_radius}, velX{_velX}, velY{_velY} {}
	};

	static bool CheckCollision(const Circle& obj1, const Circle& obj2);
};
