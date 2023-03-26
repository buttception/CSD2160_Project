#pragma once

struct AABBcollision
{
	struct AABB
	{
		float minX, minY, maxX, maxY;
	};

	static bool CheckCollision(const AABB& obj1, const AABB& obj2);
};
