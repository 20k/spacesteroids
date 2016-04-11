#ifndef SPACESHIP_HPP_INCLUDED
#define SPACESHIP_HPP_INCLUDED

#include <vec/vec.hpp>

#include <SFML/Graphics.hpp>

struct fuel_type
{
    float density = 1.f; ///kg/m3
    float mass_isp = 1.f; ///NS ///density impulse ///also ve apparently
};

struct rect
{
    vec2f tl;
    vec2f dim;
    vec3f col;

    rect(vec2f _tl, vec2f _dim, vec3f _col);

    rect();

    void draw(sf::RenderWindow& win);
};

struct component
{
    rect shape;
    fuel_type f_type;

    float non_fuel_mass_kg = 0.f;

    float fuel_mass_kg = 1.f;
    float fuel_mass_burn_ps = 1.f;
};

struct ship
{
    std::vector<component> stage_list;

    float get_dv(int stage);
    float get_dv();
};


///density impulse something different
///we want mass impulse, which is just isp * 9.81
///hydrazine density = 795
///hydrazine density impulse = 328 * 1000
///hydrazine isp = 303

///rp1/lox = 294 density impulse * 1000
///rp1/lox = 0.9 g/ml = 900 kg/m3
///rp1/lox = 289, or 263 according to saturn 5 rocket

///http://www.braeunig.us/space/propel.htm

#endif // SPACESHIP_HPP_INCLUDED
