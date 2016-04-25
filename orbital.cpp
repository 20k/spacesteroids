#include "orbital.hpp"

///increase? Would lessen load on path finding components
double dt_s = 12000 / 1;
double dt_old = dt_s;

void orbital::orbit_speed(double speed, vec2d parent)
{
    vec2d rel = pos - parent;

    ///this is orbital tangent
    vec2d tangent = perpendicular(rel).norm();

    old_pos = tangent * speed * dt_s + pos;
}

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

double orbital::get_orbital_velocity(double distance)
{
    double vel = sqrt(gravitational_constant * mass / distance);

    return vel;
}

/// xi - xi-1 = vi-1 * dti-1 + 0.5 * ai-1 * dti-1 * dti-1

orbital::orbital(double _mass, double _distance, double _speed, double rad)
{
    mass = _mass;
    pos = {_distance, 0};

    set_speed(_speed);

    radius = rad;
}

orbital::orbital(){}

void orbital::manual_update(vec2d new_pos)
{
    old_pos = pos;

    pos = new_pos;
}

orbital* orbital::clone()
{
    return new orbital(*this);
}
