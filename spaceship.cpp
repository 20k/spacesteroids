#include "spaceship.hpp"

rect::rect(vec2f _tl, vec2f _dim, vec3f _col)
{
    tl = _tl;
    dim = _dim;
    col = _col;
}

void rect::draw(sf::RenderWindow& win)
{
    sf::RectangleShape rect;

    rect.setSize({dim.v[0], dim.v[1]});
    rect.setPosition({tl.v[0], tl.v[1]});

    vec3f ncol = col * 255.f;

    rect.setFillColor({ncol.v[0], ncol.v[1], ncol.v[2]});

    win.draw(rect);
}

rect::rect(){}

float ship::get_dv(int stage)
{
    float total_mass = 0.f;
    float dry_mass = 0.f;

    int n = 0;

    for(component& i : stage_list)
    {
        if(n == stage)
        {
            total_mass += i.fuel_mass_kg + i.non_fuel_mass_kg;
            dry_mass += i.non_fuel_mass_kg;
        }
        else if(n > stage)
        {
            total_mass += i.fuel_mass_kg + i.non_fuel_mass_kg;
            dry_mass += i.fuel_mass_kg + i.non_fuel_mass_kg;
        }
        else if(n < stage)
        {

        }

        n++;
    }

    float rocket_exhaust = stage_list[stage].f_type.mass_isp;

    float dv = 0.f;

    if(dry_mass >= 0.001f)
        dv = rocket_exhaust * log(total_mass / dry_mass);

    return dv;

    /*float mass = stage_list[stage].fuel_mass_kg + stage_list[stage].non_fuel_mass_kg;
    float dry_mass = stage_list[stage].non_fuel_mass_kg;

    float ve = stage_list[stage].f_type.force_per_kg;

    if(dry_mass >= 0.001f)
        return ve * log(mass / dry_mass);

    return 0.f;*/
}

float ship::get_dv()
{
    int n = 0;

    float dv = 0;

    for(auto& i : stage_list)
    {
        dv += get_dv(n++);
    }

    return dv;
}
