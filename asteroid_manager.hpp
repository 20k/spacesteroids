#ifndef ASTEROID_MANAGER_HPP_INCLUDED
#define ASTEROID_MANAGER_HPP_INCLUDED

#include <vector>
#include <utility>
#include <string>
#include <map>
#include <vec/vec.hpp>

struct orbital;

struct asteroid_manager
{
    int gid = 1;

    std::map<orbital*, int> id_map;
    std::map<int, std::string> cached_names;

    ///year -> half_month -> asteroids in half month -> {discovery_date, id}
    std::vector<std::vector<std::vector<std::pair<float, int>>>> discovery_dates;

    void discover(orbital* o, float year);

    std::string get_name(orbital* o);
};

#endif // ASTEROID_MANAGER_HPP_INCLUDED
