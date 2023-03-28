#include "Tank.h"

Tank::Tank(std::string tankSprite)
{
	HGE* hge = hgeCreate(HGE_VERSION);

	set_object_type(MOVABLE_OBJECT_TYPE_TANK);

	//std::string shipfilename = "ship" + std::to_string(shiptype_) + ".png";
	tex_ = hge->Texture_Load(tankSprite.c_str());
	//sprite_.reset(new hgeSprite(tex_, 0, 0, 64, 64));
	sprite_ = new hgeSprite(tex_, 0, 0, 64, 64);
	sprite_->SetHotSpot(32, 32);

	// turret tex
	turretTex_ = hge->Texture_Load("turret.png");
	turretSprite_ = new hgeSprite(turretTex_, 0, 0, 64, 64);
	turretSprite_->SetHotSpot(32, 32);

	//font_.reset(new hgeFont("font1.fnt"));
	font_ = new hgeFont("font1.fnt");
	font_->SetScale(0.5);
	player_name = "Me";
	hge->Release();

	set_x(400);
	set_y(300);
	set_w(0);
	set_server_xyw(400, 300, 0);
	set_client_xyw(400, 300, 0);
	set_ratio(1.0f);

	turret_rotation = server_turret_rot = client_turret_rot = 0.f;
	active = false;
}

void Tank::Render()
{
	// render the ship.
	sprite_->RenderEx(get_x(), get_y(), get_w());
	turretSprite_->RenderEx(get_x(), get_y(), turret_rotation);
	// print the ship name.
	font_->printf(get_x(), get_y() + 30, HGETEXT_LEFT, "%s", player_name.c_str());
}

bool Tank::Update(float timedelta, float spritewidth, float spriteheight)
{
	float prev_rot = turret_rotation;
	const float ratio = get_ratio();
	turret_rotation = ratio * server_turret_rot + (1 - ratio) * client_turret_rot;

	// call base class update
	bool ret = Movable::Update(timedelta, spritewidth, spriteheight);

	if (prev_rot != turret_rotation) ret = true;

	return ret;
}
