#ifndef ORBITAL_HPP_INCLUDED
#define ORBITAL_HPP_INCLUDED

#include <iostream>
#include <sfml/graphics.hpp>
#include <vec/vec.hpp>


typedef vec<2, double> vec2d;

static double dt_s = 12000 / 1;
static double dt_old = dt_s;

struct orbital
{
    double mass = 0;
    vec2d pos = {0,0};
    vec2d old_pos = {0,0};

    vec3f col = {1, 1, 1};

    ///so that we update atomically
    //vec2d next_pos = {0,0};

    vec2d acc = {0,0};

    double radius;

    void set_speed(double speed);

    void set_speed(double speed, vec2d dir);

    ///amount in ms
    void accelerate_relative_to_velocity(double amount, double angle_offset, double time_s);

    orbital(double _mass, double _distance, double _speed, double rad = 0);

    orbital();
};

#endif // ORBITAL_HPP_INCLUDED
