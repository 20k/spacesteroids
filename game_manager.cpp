#include "game_manager.hpp"

///ooh. FoW aots style, sensors to peek into it
///direct sensors
///aoe short range
void game_manager::spawn_hostile_asteroid(orbital* earth, orbital* sun)
{
    ///F = gmm/r2
    ///A = g(theirmass)/r2

    ///dmomentum/dt = G(theirmass)/r2

    ///variable is r?
    ///dv/dt definitely = GM/r2

    ///we need to define the distance (r) in terms of t
    ///eh

    double es_dist = (earth->pos - sun->pos).length();

    double evil_spawn_dist = es_dist * 50;

    double rangle = randf_s(-M_PI, M_PI);

    vec2d pos = sun->pos + (vec2d){0, 1}.rot(rangle) * evil_spawn_dist;

    vec2d to_sun = (sun->pos - pos).norm();

    double speed = 1 * dt_s;

    orbital* o = new orbital(100, evil_spawn_dist, speed, 100);

    o->old_pos = pos;
    o->pos = pos + to_sun * speed;

    o->col = {1, 0, 0};

    olist.push_back(o);
}
