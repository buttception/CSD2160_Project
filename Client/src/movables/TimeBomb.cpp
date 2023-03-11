#include "TimeBomb.h"
#include <hge.h>
#include <hgeSprite.h>
#include <math.h>

TimeBomb::TimeBomb(char * filename, float x, float y, float ownerId)
{
	HGE* hge = hgeCreate(HGE_VERSION);

	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 240, 240));
	sprite_->SetHotSpot(120, 120);

	set_object_type(MOVABLE_OBJECT_TYPE_MINE);

	ownerid_ = ownerId;
	lifeTimer = 0;
	lifeTime = 3;

	set_x(x);
	set_y(y);
	set_w(0);
	set_server_xyw(x, y, 0);
	set_client_xyw(x, y, 0);
	set_ratio(1.0f);

	set_velocity_x(0);
	set_velocity_y(0);
	set_server_velocity_x(get_velocity_x());
	set_server_velocity_y(get_velocity_y());
}

TimeBomb::~TimeBomb()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}

void TimeBomb::Render()
{
	sprite_->RenderEx(get_x(), get_y(), get_w(), 0.083333);
}
