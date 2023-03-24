#include "Tank.h"

Tank::Tank()
{
    HGE* hge = hgeCreate(HGE_VERSION);

    set_object_type(MOVABLE_OBJECT_TYPE_TANK);

    //std::string shipfilename = "ship" + std::to_string(shiptype_) + ".png";
    tex_ = hge->Texture_Load("ship1.png");
    sprite_.reset(new hgeSprite(tex_, 0, 0, 64, 64));
    sprite_->SetHotSpot(32, 32);

    font_.reset(new hgeFont("font1.fnt"));
    font_->SetScale(0.5);
    player_name = "Me";
    hge->Release();

    set_x(400);
    set_y(300);
    set_w(0);
    set_server_xyw(400, 300, 0);
    set_client_xyw(400, 300, 0);
    set_ratio(1.0f);
}

void Tank::Render()
{
    // render the ship.
    sprite_->RenderEx(get_x(), get_y(), get_w());
    // print the ship name.
    font_->printf(get_x(), get_y() + 20, HGETEXT_LEFT, "%s", player_name);
}