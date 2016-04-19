#ifndef ORBITAL_HPP_INCLUDED
#define ORBITAL_HPP_INCLUDED

#include <iostream>
#include <sfml/graphics.hpp>
#include <vec/vec.hpp>


typedef vec<2, double> vec2d;

extern double dt_s;
extern double dt_old;
extern double gravitational_constant;

struct orbital
{
    double mass = 0;
    vec2d pos = {0,0};
    vec2d old_pos = {0,0};

    vec3f col = {1, 1, 1};
    vec3f transitory_draw_col = {1, 1, 1};

    ///so that we update atomically
    //vec2d next_pos = {0,0};

    vec2d acc = {0,0};
    vec2d unconditional_acc = {0,0};

    double radius = 1;

    bool skip = false;

    void orbit_speed(double speed, vec2d parent);

    void set_speed(double speed);

    void set_speed(double speed, vec2d dir);

    ///amount in ms
    void accelerate_relative_to_velocity(double amount, double angle_offset, double time_s);

    double get_orbital_velocity(double distance);

    orbital(double _mass, double _distance, double _speed, double rad = 0);

    orbital();

    void manual_update(vec2d new_pos);

    orbital* clone();
};

#endif // ORBITAL_HPP_INCLUDED
