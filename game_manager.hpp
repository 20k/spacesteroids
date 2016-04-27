#ifndef GAME_MANAGER_HPP_INCLUDED
#define GAME_MANAGER_HPP_INCLUDED

#include "orbital.hpp"
#include "manoeuvre.hpp"

///spawn asteroids towards sun, target at earth
struct game_manager
{
    std::vector<orbital*> olist;
    std::map<orbital*, bool> skip_hunting;

    void spawn_hostile_asteroid(orbital* earth, orbital* sun);
};


#endif // GAME_MANAGER_HPP_INCLUDED
