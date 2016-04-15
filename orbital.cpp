#include "orbital.hpp"

void orbital::set_speed(double speed)
{
    old_pos = perpendicular(pos).norm() * speed * dt_s + pos;
}

void orbital::set_speed(double speed, vec2d dir)
{
    old_pos = pos - dir.norm() * speed * dt_s;
}

void orbital::accelerate_relative_to_velocity(double amount, double angle_offset, double time_s)
{
    vec2d vel_dir = (pos - old_pos).norm();

    ///the directional part of this is wrong
    //acc = acc + vel_dir.rot(angle_offset) * amount;// * time_s * time_s;


    unconditional_acc = unconditional_acc + vel_dir.rot(angle_offset).norm() * amount * time_s * time_s;
}

orbital::orbital(double _mass, double _distance, double _speed, double rad)
{
    mass = _mass;
    pos = {_distance, 0};

    set_speed(_speed);

    radius = rad;
}

orbital::orbital(){}
