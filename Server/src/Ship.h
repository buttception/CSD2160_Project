#ifndef _SHIP_H
#define _SHIP_H
#pragma once


struct _Ship {
    bool connected;
    int  ship_type;

    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float server_velocity_x;
    float server_velocity_y;
    float angular_velocity;

    _Ship() : connected( false ), x( 0 ), y( 0 ), w( 0 ), ship_type( 0 ) {}
};


#endif