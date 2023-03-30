#ifndef _MOVABLES_H_
#define _MOVABLES_H_

#include <memory>

#include "hge.h"
#include "hgesprite.h"
#include "hgeFont.h"
#include <vector>

/**
* The Movable class represents a super class for any of movable object
* which is floating in space. It obeys 2D physics in therms of displacement,
* velocity and acceleration, as well as angular position and displacement.
*/

enum MOVABLE_OBJECT_TYPE
{
	MOVABLE_OBJECT_TYPE_EMPTY = 0,

	MOVABLE_OBJECT_TYPE_TANK,
	MOVABLE_OBJECT_TYPE_PROJECTILE,

	MOVABLE_OBJECT_TYPE_MAX
};

class Movable
{
private:
	int object_type_;
	struct _values
	{
		// These variables are for local rendering
		float x_; //!< The x-ordinate of the ship
		float y_; //!< The y-ordinate of the ship
		float w_; //!< The angular position of the ship
		float velocity_x_; //!< The resolved velocity of the ship along the x-axis
		float velocity_y_; //!< The resolved velocity of the ship along the y-axis
		float angular_velocity_;
	} now_, prev_;

	// Add for interpolation
	float server_x_;
	float server_y_;
	float server_w_;
	float server_velocity_x;
	float server_velocity_y;
	float client_x_;
	float client_y_;
	float client_w_;
	float ratio_;
	float frametime_;
	float timer_;
	hgeRect collidebox;
	bool collided_with_me_;

public:
	HTEXTURE tex_; //!< Handle to the sprite's texture
	//std::shared_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship
	//std::shared_ptr<hgeFont> font_;
	hgeSprite* sprite_;
	hgeFont* font_;

	Movable();
	~Movable();
	
	void  set_object_type(int object_type) { this->object_type_ = object_type; }
	int   get_object_type(void) { return this->object_type_; }

	float get_x(void) { return this->now_.x_; }
	void  set_x(float x) {
		this->prev_.x_ = this->now_.x_;
		this->now_.x_ = x;
	}
	float get_y(void) { return this->now_.y_; }
	void  set_y(float y) {
		this->prev_.y_ = this->now_.y_;
		this->now_.y_ = y;
	}
	float get_w(void) { return this->now_.w_; }
	void  set_w(float w) {
		this->prev_.w_ = this->now_.w_;
		this->now_.w_ = w;
	}
	float get_velocity_x(void) { return this->now_.velocity_x_; }
	void  set_velocity_x(float velocity_x) {
		this->prev_.velocity_x_ = this->now_.velocity_x_;
		this->now_.velocity_x_ = velocity_x;
	}
	float get_velocity_y(void) { return this->now_.velocity_y_; }
	void  set_velocity_y(float velocity_y) {
		this->prev_.velocity_y_ = this->now_.velocity_y_;
		this->now_.velocity_y_ = velocity_y;
	}
	float get_angular_velocity(void) { return this->now_.angular_velocity_; }
	void  set_angular_velocity(float angular_velocity) {
		this->prev_.angular_velocity_ = this->now_.angular_velocity_;
		this->now_.angular_velocity_ = angular_velocity;
	}

	void restore_xy(void) {
		this->now_.x_ = this->prev_.x_;
		this->now_.y_ = this->prev_.y_;
	}
	void restore_w(void) { this->now_.w_ = this->prev_.w_; }
	void restore_velocity(void) {
		this->now_.velocity_x_ = this->prev_.velocity_x_;
		this->now_.velocity_y_ = this->prev_.velocity_y_;
	}
	void restore_angular_velocity(void) { this->now_.angular_velocity_ = this->prev_.angular_velocity_; }

	void  set_server_xyw(float x, float y, float w) { server_x_ = x; server_y_ = y; server_w_ = w; }
	void  set_server_x(float x) { server_x_ = x; }
	void  set_server_y(float y) { server_y_ = y; }
	void  set_server_w(float w) { server_w_ = w; }
	float get_server_x(void) { return server_x_; }
	float get_server_y(void) { return server_y_; }
	float get_server_w(void) { return server_w_; }

	void  set_server_velocity_x(float velocity) { server_velocity_x = velocity; }
	void  set_server_velocity_y(float velocity) { server_velocity_y = velocity; }

	float get_server_velocity_x(void) { return server_velocity_x; }
	float get_server_velocity_y(void) { return server_velocity_y; }

	void  set_client_xyw(float x, float y, float w) { client_x_ = x; client_y_ = y; client_w_ = w; }
	void  set_client_x(float x) { client_x_ = x; }
	void  set_client_y(float y) { client_y_ = y; }
	void  set_client_w(float w) { client_w_ = w; }
	float get_client_x(void) { return client_x_; }
	float get_client_y(void) { return client_y_; }
	float get_client_w(void) { return client_w_; }

	void  set_ratio(float r) { ratio_ = r; }
	float get_ratio(void) { return ratio_; }

	void  set_frametime(float r) { frametime_ = r; }
	float get_frametime(void) { return frametime_; }

	void  set_timer(float r) { timer_ = r; }
	float get_timer(void) { return timer_; }

	void set_collided_with_me(bool collided) { collided_with_me_ = collided; }
	bool get_collided_with_me(void) { return collided_with_me_; }

public:
	void do_interpolate_update();
	bool Update(float timedelta, float spritewidth, float spriteheight);
	void stop_moving(void);
	bool compare_xyw(void);
};

#endif

