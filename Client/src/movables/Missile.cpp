#include "Missile.h"

Missile::Missile(float _x, float _y, float _w, float _velX, float _velY, int _ownerID)
{
	HGE* hge = hgeCreate(HGE_VERSION);

	set_object_type(MOVABLE_OBJECT_TYPE_PROJECTILE);
	
	tex_ = hge->Texture_Load("bullet.png");
	//sprite_.reset(new hgeSprite(tex_, 0, 0, 64, 64));
	sprite_ = new hgeSprite(tex_, 0, 0, 40, 20);
	sprite_->SetHotSpot(20, 10);

	hge->Release();

	set_x(_x);
	set_y(_y);
	set_w(_w);
	set_server_xyw(_x, _y, _w);
	set_client_xyw(_x, _y, _w);
	set_ratio(0.5f);
}

void Missile::Render()
{
	// render the missile
	sprite_->RenderEx(get_x(), get_y(), get_w());
}
