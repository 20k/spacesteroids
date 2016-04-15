#ifndef ORBITAL_MANAGER_HPP_INCLUDED
#define ORBITAL_MANAGER_HPP_INCLUDED

#include "orbital.hpp"

using namespace std;


struct ret_info
{
    vector<vector<vec2d>> ret;
    int mtick;
};

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

    void draw(vec2d _pos, vec3f _col, sf::RenderTarget& win, float r = 4);

    void draw(orbital& o, sf::RenderTarget& win, float r = 2);

    void display(sf::RenderTarget& win, float r = 2);


    vector<vector<vec2d>> test(int ticks, float dt_cur, float dt_old, sf::RenderWindow* tex, bool render = true, orbital* test_orbital = nullptr, std::vector<orbital*> info_to_retrieve = std::vector<orbital*>());

    ///here the test orbital is a member of the manager
    vector<vec2d> test_with_adaptive_tick(int ticks, float dt_max, float dt_min, float dt_old, orbital* test_orbital);

    int get_minimum_distance(int o1, int o2, const vector<vector<vec2d>>& pos);

    ret_info bisect(int ticks, float dt_cur, float dt_old,
                    float base_speed, float minimum, float maximum,
                    float angle_offset, float half_angle_cone, float angle_subdivisions,
                    int num_per_step, int depth, orbital* test_orbital = nullptr, orbital* target_orbital = nullptr, std::vector<orbital*> info_to_retrieve = std::vector<orbital*>(), int c = 0);

    void plot(const vector<vector<vec2d>>& elements, int which_element, int which_tick, sf::RenderWindow& win, vec3f col = {1, 1, 1});

    void plot_all(const vector<vector<vec2d>>& elements, int which_tick, sf::RenderWindow& win, vector<vec3f> cols = vector<vec3f>());

    void plot_orbit(orbital* o, int ticks, sf::RenderWindow& tex);

    orbital* get_nearest(vec2d mouse_screen_pos, vec2d screen_dim);
};


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
