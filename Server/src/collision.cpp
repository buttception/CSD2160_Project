#include "collision.h"
#include <iostream>

bool Collision::CheckCollision(const Circle& obj1, const Circle& obj2)
{
    float dist = std::sqrt((obj1.x - obj2.x) * (obj1.x - obj2.x) + (obj1.y - obj2.y) * (obj1.y - obj2.y));
    float radSum = obj1.radius + obj2.radius;

    //std::cout << dist << "<-dist\n";
    //std::cout << dist << "<-radSum\n";

    if (dist <= radSum)
    {
        float relVelX = obj2.velX - obj1.velX;
        float relVelY = obj2.velY - obj1.velY;
    
        float collidingIn = -((relVelX * (obj2.x - obj1.x)) + (relVelY* (obj2.y - obj1.y))) / (relVelX * relVelX + relVelY* relVelY);

        if (collidingIn >= 0 && collidingIn <= 1)
            return true;
    }
	return false;
}
