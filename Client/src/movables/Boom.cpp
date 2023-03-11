#include "Boom.h"

Boom::Boom(char * filename, float x, float y, float w)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	lifeTime = 2.f;
	lifeTimer = 0.f;
	active = true;

	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 40, 40));
	sprite_->SetHotSpot(20, 20);

	set_object_type(MOVABLE_OBJECT_TYPE_BOOM);

	set_x(x);
	set_y(y);
	set_w(w);
	set_server_xyw(x, y, w);
	set_client_xyw(x, y, w);
	set_ratio(1.0f);

	set_velocity_x(0);
	set_velocity_y(0);
	set_server_velocity_x(get_velocity_x());
	set_server_velocity_y(get_velocity_y());

	set_x(get_x() + (get_velocity_x() * 0.5f));
	set_y(get_y() + (get_velocity_y() * 0.5f));
}

Boom::~Boom()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}

void Boom::Render()
{
	sprite_->RenderEx(get_x(), get_y(), get_w());
}

void Boom::Update(float _dt)
{
	lifeTimer += _dt;
	if (lifeTimer > lifeTime)
	{
		//past lifetime, kill it
		active = false;
	}
}
