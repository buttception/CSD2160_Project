#ifndef _ASTEROID_H
#define _ASTEROID_H


class _Asteroid {
    private:
        int ID;
        float x;
        float y;
        float velocity_x;
        float velocity_y;
        float angular_velocity;

    public:
        _Asteroid();
        _Asteroid( int ID, float x, float y, float angular_velocity );
        ~_Asteroid();

    public:
        void set_ID( int ID ) { this->ID = ID; }
        void set_x( float x ) { this->x = x; }
        void set_y( float y ) { this->y = y; }
        void set_velocity_x( float velocity_x ) { this->velocity_x = velocity_x; }
        void set_velocity_y( float velocity_y ) { this->velocity_y = velocity_y; }
        void set_angular_velocity( float angular_velocity ) { this->angular_velocity = angular_velocity; }

        int   get_ID( void ) { return ID; }
        float get_x( void ) { return x; }
        float get_y( void ) { return y; }
        float get_velocity_x( void ) { return velocity_x; }
        float get_velocity_y( void ) { return velocity_y; }
        float get_angular_velocity( void ) { return angular_velocity; }

    public:
        void Update( float timedelta, float spritewidth, float spriteheight );

};


#endif