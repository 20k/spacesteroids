#ifndef ORBITAL_MANAGER_HPP_INCLUDED
#define ORBITAL_MANAGER_HPP_INCLUDED

#include "orbital.hpp"

using namespace std;


struct ret_info
{
    vector<vector<vec2d>> ret;
    int mtick;
};

const static double gravitational_constant = 0.0000000000674;

///manager* clone
struct manager
{
    std::vector<orbital*> olist;

    orbital* make_new(const orbital& orbit = orbital());

    void destroy(orbital* o);

    static vec2d offset;

    static double scale;

    static
    vec2d pos2screen(vec2d in)
    {
        vec2d pos = in;

        double base_scale = 1000 * 1000 * 1000;

        pos = pos;

        pos = pos / base_scale;

        return (pos + offset) / scale; /// + hw
    }

    static
    vec2d screen2pos(vec2d in, vec2d hw)
    {
        double base_scale = 1000 * 1000 * 1000;

        vec2d pos = in - hw;

        pos = pos * scale - offset;

        pos = pos * base_scale;

        return pos;
    }

    void tick(float dt_cur, float dt_old);

    void tick_only_probes(float dt_cur, float dt_old, const std::vector<orbital*>& probes);

    void draw(vec2d _pos, vec3f _col, sf::RenderTarget& win, float r = 4);

    void draw(orbital& o, sf::RenderTarget& win, float r = 2);

    void draw_bulk(const std::vector<orbital*>& orbitals, sf::RenderTarget& win, float r = 2);


    void display(sf::RenderTarget& win, float r = 2);


    vector<vector<vec2d>> test(int ticks, float dt_cur, float dt_old, sf::RenderWindow* tex, bool render = true, orbital* test_orbital = nullptr, std::vector<orbital*> info_to_retrieve = std::vector<orbital*>());
    vector<vector<vec2d>> test_with_cache(int ticks, float dt_cur, float dt_old, orbital* test_orbital, std::vector<std::vector<vec2d>>& cache, std::vector<orbital*> info_to_retrieve = std::vector<orbital*>());

    ///here the test orbital is a member of the manager
    vector<vec2d> test_with_adaptive_tick(int ticks, float dt_max, float dt_min, float dt_old, orbital* test_orbital);

    int get_minimum_distance(int o1, int o2, const vector<vector<vec2d>>& pos);

    ret_info bisect(int ticks, float dt_cur, float dt_old,
                    float base_speed, float minimum, float maximum,
                    float angle_offset, float half_angle_cone, float angle_subdivisions,
                    int num_per_step, int depth, orbital* test_orbital = nullptr, orbital* target_orbital = nullptr, std::vector<orbital*> info_to_retrieve = std::vector<orbital*>(), int c = 0);

    ///assumes the mass we're testing is negligable
    ret_info bisect_with_cache(int ticks, float dt_cur, float dt_old,
                    float base_speed, float minimum, float maximum,
                    float angle_offset, float half_angle_cone, float angle_subdivisions,
                    int num_per_step, int depth, orbital* test_orbital = nullptr, orbital* target_orbital = nullptr, std::vector<orbital*> info_to_retrieve = std::vector<orbital*>(), int c = 0, const std::vector<std::vector<vec2d>>& cache = std::vector<std::vector<vec2d>>());

    void plot(const vector<vector<vec2d>>& elements, int which_element, int which_tick, sf::RenderWindow& win, vec3f col = {1, 1, 1});

    void plot_all(const vector<vector<vec2d>>& elements, int which_tick, sf::RenderWindow& win, vector<vec3f> cols = vector<vec3f>());

    void plot_orbit(orbital* o, int ticks, sf::RenderWindow& tex);

    orbital* get_nearest(vec2d mouse_screen_pos, vec2d screen_dim);

    void restore_from_backup(const std::vector<orbital>& backup);

    std::vector<orbital> make_backup();
};

///real asteroid belt mass 3.00E+21 kg
///avg mass = 0.05% * that
///
static
std::vector<orbital*> populate_orbits_with_asteroids(orbital* o, orbital* parent, int num)
{
    std::vector<orbital*> asteroids;

    vec2d base = parent->pos;
    vec2d body = o->pos;

    double rad = (body - base).length();
    double angle = (body - base).angle();

    double mass_mod = 0.01;

    double total_mass = 3 * pow(10, 21) * mass_mod;
    double avg_mass = total_mass / num;

    double exclusion_radius = o->radius * 2.;

    double radius_mod = 0.9;

    double angle_mod = 0.01 * 2 * M_PI;

    for(int i=0; i<num; i++)
    {
        double fangle = (double)i/num;

        fangle *= 2 * M_PI;

        fangle += randf_s(-angle_mod, angle_mod);

        double ran_scale = randf_s(0., 1.);

        ran_scale *= ran_scale;

        ran_scale += randf_s(0., 0.1);

        if(randf_s(0., 1.) > 0.5)
            ran_scale = -ran_scale;

        ran_scale *= radius_mod;

        double frad = rad + ran_scale * rad;

        vec2d pos = radius_angle_to_vec(frad, fangle);

        vec2d global_pos = pos + base;

        if((global_pos - o->pos).length() < exclusion_radius)
            continue;

        double vel = sqrt(gravitational_constant * parent->mass / frad);

        ///v = sqrt G * mass / rad

        orbital* norbit = new orbital(avg_mass, frad, vel);

        norbit->pos = global_pos;

        norbit->set_speed(vel);

        asteroids.push_back(norbit);
    }

    return asteroids;
}


/*void simulate(int ticks, float dt_cur, float dt_old)
{
    for(int i=0; i<ticks; i++)
    {
        tick(dt_cur, dt_old);
    }
}*/

/*void smear(int ticks, sf::RenderTexture& tex)
{
    std::vector<orbital> old;

    for(auto& i : olist)
    {
        old.push_back(*i);
    }

    for(int i=0; i<ticks; i++)
    {
        tick();

        display(tex, 1);
    }

    for(int i=0; i<(int)olist.size(); i++)
    {
        *olist[i] = old[i];
    }

    tex.display();
}*/

#endif // ORBITAL_MANAGER_HPP_INCLUDED
