#include <iostream>
#include <sfml/graphics.hpp>
#include <vec/vec.hpp>
#include <omp.h>

using namespace std;

#include "orbital.hpp"
#include "orbital_manager.hpp"
#include "spaceship.hpp"

#include "manoeuvre.hpp"

#include "game_manager.hpp"

#include "ui.hpp"

#include "asteroid_manager.hpp"


///has the button been pressed once, and only once
template<sf::Keyboard::Key k>
bool once()
{
    static bool last;

    sf::Keyboard key;

    if(key.isKeyPressed(k) && !last)
    {
        last = true;

        return true;
    }

    if(!key.isKeyPressed(k))
    {
        last = false;
    }

    return false;
}

template<sf::Mouse::Button b>
bool once()
{
    static bool last;

    sf::Mouse m;

    if(m.isButtonPressed(b) && !last)
    {
        last = true;

        return true;
    }

    if(!m.isButtonPressed(b))
    {
        last = false;
    }

    return false;
}

///when we name asteroids, we'll eventually dynamically calculated the provisional name based off when the planet actually discovered them

///is this just a planet descriptor, or a generic descriptor?
struct pds
{
    static int gid;
    int id = gid++;

    std::string name;

    pds(const std::string& _name)
    {
        name = _name;
    }

    pds()
    {

    }
};

int pds::gid;


void calculate_asteroid_names_with_random_discovery_year(int min_year, int max_year, const std::vector<orbital*>& orbitals, asteroid_manager& amanage)
{
    ///go to your doctor to get your roids sorted
    std::vector<std::pair<float, orbital*>> sorted_roids;


    for(auto& i : orbitals)
    {
        float year = randf_s((float)min_year, (float)max_year);

        sorted_roids.push_back({year, i});
    }

    for(auto& i : sorted_roids)
    {
        amanage.discover(i.second, i.first);
    }
}

///game design
///harvest asteroids for fuel + materials
///initial stages have to return physically to earth the resources
///later stages can fling resources to earth
///earth also produces materials at slow rate
///have to defend system from incoming threats
///network communication system, have to station relay satellites
///several ways to take down incoming threats
///satellite grapple and self destruct
///satellite grapple and redirect into a gas giant
///satellite grapple and redirect on an escape directory
///land base throws explosives, as well as generating new resources
///planet depletion?

///satellite based missiles?
///reposition planets

///we need a save/load
int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow win;
    win.create(sf::VideoMode(1200, 800), "hi", sf::Style::Default, settings);
    //win.setVerticalSyncEnabled(true);


    ui_manager ui_handler;

    manager orbital_manager;

    orbital* sun = orbital_manager.make_new();

    sun->mass = 1.989 * pow(10., 30.);

    sun->col = {1.f, 0.8, 0.4};

    sun->radius = 696342.f * pow(10, 3);


    ///change to array, reload me on load
    orbital* earth = orbital_manager.make_new(orbital(5.962 * pow(10., 24.), 147.09 * pow(10, 9), 30.29 * pow(10, 3), 6371 * pow(10, 3)));
    //orbital* earth = orbital_manager.make_new(orbital(5.962 * pow(10., 24.), 149.6 * pow(10, 9), 30. * pow(10, 3), 6371 * pow(10, 3)));

    earth->col = {0.2f, 0.2f, 1.f};

    std::cout << "pos " << earth->pos <<  " opos " << earth->old_pos << std::endl;

    ///we could and should totally ignore mercury venus mars and pluto from all gravity calculations, unless they are the target

    orbital* mercury = orbital_manager.make_new(orbital(3.3 * pow(10, 23), 46. * pow(10, 9), 58.98 * pow(10, 3), 2440 * pow(10, 3)));
    //orbital* mercury = orbital_manager.make_new(orbital(3.3 * pow(10, 23), 57.9 * pow(10, 9), 47. * pow(10, 3), 2440 * pow(10, 3)));

    orbital* venus = orbital_manager.make_new(orbital(4.87 * pow(10, 24), 107.48 * pow(10, 9), 35.26 * pow(10, 3), 6052 * pow(10, 3)));
    //orbital* venus = orbital_manager.make_new(orbital(4.87 * pow(10, 24), 108.2 * pow(10, 9), 35.02 * pow(10, 3), 6052 * pow(10, 3)));

    orbital* mars = orbital_manager.make_new(orbital(6.42 * pow(10, 23), 206.62 * pow(10, 9), 26.50 * pow(10, 3), 3390 * pow(10, 3)));
    //orbital* mars = orbital_manager.make_new(orbital(6.42 * pow(10, 23), 227.9 * pow(10, 9), 24.077 * pow(10, 3), 3390 * pow(10, 3)));

    ///orbits as we have them are not quite correct, jupiter/saturn should be in a 2/1 resonance

    orbital* jupiter = orbital_manager.make_new(orbital(1.9 * pow(10, 27), 740.52 * pow(10, 9), 13.72 * pow(10, 3), 69911 * pow(10, 3)));
    //orbital* jupiter = orbital_manager.make_new(orbital(1.9 * pow(10, 27), 778.3 * pow(10, 9), 13.07 * pow(10, 3), 69911 * pow(10, 3)));

    orbital* saturn = orbital_manager.make_new(orbital(5.69 * pow(10, 26), 1352.55 * pow(10, 9), 10.18 * pow(10, 3), 58232 * pow(10, 3)));
    //orbital* saturn = orbital_manager.make_new(orbital(5.69 * pow(10, 26), 1427. * pow(10, 9), 9.69 * pow(10, 3), 58232 * pow(10, 3)));

    ///hehe. I get to laugh at uranus because I wrote this
    orbital* uranus = orbital_manager.make_new(orbital(8.68 * pow(10, 25), 2741.30 * pow(10, 9), 7.11 * pow(10, 3), 25362 * pow(10, 3)));
    //orbital* uranus = orbital_manager.make_new(orbital(8.68 * pow(10, 25), 2861. * pow(10, 9), 6.81 * pow(10, 3), 25362 * pow(10, 3)));

    orbital* neptune = orbital_manager.make_new(orbital(1.02 * pow(10, 26), 4444.45 * pow(10, 9), 5.50 * pow(10, 3), 24622 * pow(10, 3)));
    //orbital* neptune = orbital_manager.make_new(orbital(1.02 * pow(10, 26), 4497.1 * pow(10, 9), 5.43 * pow(10, 3), 24622 * pow(10, 3)));

    ///we will define pluto at its closest point to the sun, ie smallest distance + largest velocity
    ///the eccentricity will not be correct relative the other planets, but as they're all perfect circles that's just dandy at the moment
    ///we need everything's rotation from the centre, /time offset, whatever its called
    ///nope, make it aphelion, otherwise its inside neptune
    ///this is the only one thats defined at aphelion
    orbital* pluto = orbital_manager.make_new(orbital(1.309 * pow(10, 22), 7375.93 * pow(10, 9), 3.71 * pow(10, 3), 1187 * pow(10, 3)));
    //orbital* pluto = orbital_manager.make_new(orbital(1.309 * pow(10, 22), 7375.93 * pow(10, 9), 3.71 * pow(10, 3), 1187 * pow(10, 3)));

    orbital* voyager_base = new orbital(5.962 * pow(10., 24.), 147.09 * pow(10, 9), 30.29 * pow(10, 3), 2);
    //orbital* voyager_base = orbital_manager.make_new(orbital(5.962 * pow(10., 24.), 147.09 * pow(10, 9), 30.29 * pow(10, 3), 2));

    voyager_base->mass = 721.9;
    voyager_base->col = {1, 0, 0};

    std::vector<pds> descriptor_list = {pds("sun"), pds("earth"), pds("mercury"), pds("venus"), pds("mars"), pds("jupiter"), pds("saturn"), pds("urectum"), pds("neptune"), pds("pluto")};

    std::vector<orbital**> mainstream_orbitals = {&sun, &earth, &mercury, &venus, &mars, &jupiter, &saturn, &uranus, &neptune, &pluto};

    ///small reduction in accuracy for probably quite a large speed boost
    ///earth, mercury venus mars, and pluto, the dwarf planet. Sorry pluto
    ///could probably build this list automatically, if mass < 10^25
    //orbital_manager.set_unimportant_planet_skip({1, 2, 3, 4, 9});

    std::vector<orbital*> asteroids = populate_orbits_with_asteroids(jupiter, sun, 1000);

    asteroid_manager amanage;

    calculate_asteroid_names_with_random_discovery_year(1960, 2017, asteroids, amanage);


    std::vector<orbital*> player_satellites;

    player_satellites.push_back(new orbital(*voyager_base));

    orbital* currently_in_control = player_satellites[0];

    //orbital* target_to_circularise = nullptr;

    //target_info current_target;

    //manoeuvre::manov current_manov;

    //current_manov.set_orbital(currently_in_control);

    manoeuvre::manov_list current_mlist;
    current_mlist.set_probe(currently_in_control);

    std::map<orbital*, manoeuvre::manov_list> manoeuvre_map;

    ///create new satellites, control them. Delete them as a debug, but in reality we have to crash them into something
    ///need for right click to work

    vec3f highlight_col = {1, 0, 1};
    vec3f hostile_highlight = {0, 1, 1};

    game_manager game_state;

    ///lets keep this purely for fluff reasons. The materials we use will rock
    #ifdef VERIFICATION_SATURN_V
    ship ss1;

    ///this is just the first stage!
    component launcher;

    //launcher.fuel_mass_burn_ps = (2290000 - 130000) / 165.f;
    launcher.fuel_mass_kg = (2290000 - 130000);

    //launcher.f_type.density = 900;
    launcher.f_type.mass_isp = 263 * 9.81;//294;

    launcher.non_fuel_mass_kg = 130000;

    ss1.stage_list.push_back(launcher);

    component s2;
    s2.fuel_mass_kg = (496200 - 40100);
    s2.f_type.mass_isp = 421 * 9.81;
    s2.non_fuel_mass_kg = 40100;

    ss1.stage_list.push_back(s2);

    component s3;

    s3.fuel_mass_kg = (123000 - 13500);
    s3.f_type.mass_isp = 421 * 9.81;
    s3.non_fuel_mass_kg = 13500;

    ss1.stage_list.push_back(s3);

    printf("DV %f\n", ss1.get_dv());
    #endif

    //orbital voyager_probe = *earth;
    //voyager_probe.mass = 721.9;
    //voyager_probe.col = {1, 0, 0};

    /*sf::RenderTexture orbits;
    orbits.create(win.getSize().x, win.getSize().y);

    orbital_manager.smear(10000, orbits);

    sf::Sprite spr(orbits.getTexture());*/


    ///earth->speed = 30km/s

    //vec2d start = 0;

    float day = 0;

    int tick = 0;

    sf::Event event;
    sf::Mouse mouse;

    sf::Keyboard key;

    int predefined_max_tick = 10000;

    vector<vector<vec2d>> last_experiment;

    bool toggled_going = false;

    ///thread the rendering
    while(win.isOpen())
    {
        sf::Clock ftime;

        float dmouse = 0;

        while(win.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                win.close();

            if(event.type == sf::Event::MouseWheelScrolled)
            {
                dmouse = event.mouseWheelScroll.delta;
            }
        }

        if(key.isKeyPressed(sf::Keyboard::LShift))
        {
            dmouse *= 5.f;
        }

        if(key.isKeyPressed(sf::Keyboard::LControl))
            dmouse *= 0.1f;

        if(dmouse < 0)
            dmouse *= manager::scale;
        else
            dmouse *= manager::scale/2;

        if(key.isKeyPressed(sf::Keyboard::Space))
            printf("%f %f\n", dmouse, manager::scale);

        if(win.hasFocus())
            manager::scale -= dmouse;

        if(manager::scale < 0.001)
            manager::scale = 0.001;

        float mscale = 0.1f;

        mscale *= manager::scale;

        if(key.isKeyPressed(sf::Keyboard::LShift))
            mscale *= 5;

        if(key.isKeyPressed(sf::Keyboard::W))
            manager::offset.v[1] += 1.f * mscale;

        if(key.isKeyPressed(sf::Keyboard::S))
            manager::offset.v[1] -= 1.f * mscale;

        if(key.isKeyPressed(sf::Keyboard::A))
            manager::offset.v[0] += 1.f * mscale;

        if(key.isKeyPressed(sf::Keyboard::D))
            manager::offset.v[0] -= 1.f * mscale;

        vec2d wh = {win.getSize().x/2., win.getSize().y/2.};

        vec2d m = {(double)mouse.getPosition(win).x, (double)mouse.getPosition(win).y};

        /*vec2d world_p = manager::screen2pos(m, wh);

        if(once<sf::Mouse::Left>() && win.hasFocus())
        {
            orbital* getrekd = orbital_manager.make_new();
            getrekd->mass = pow(10., 30.); ///10^32 = black hole,  10^27 = jupiter, 10^30 = sun
            getrekd->pos = world_p;
            getrekd->set_speed(13.07 * pow(10, 3) * 1, {0, -1});
        }*/

        ///for some reason, the circularisation is not working 100%, if transferring between two orbits

        ///need to fiddle with constant acceleration rather than impulse
        ///although then again, do I?
        #if 0
        if(once<sf::Mouse::Right>() && win.hasFocus())
        {
            /*orbital voyager_probe = *earth;
            //orbital voyager_probe = *currently_in_control;
            voyager_probe.mass = 721.9;
            voyager_probe.col = {1, 0, 0};*/

            #if 0

            orbital* probe = currently_in_control;

            sf::Clock clk;

            int tnum = 100000;

            //int earth_saturn_tick_time = 80000;

            ///keep this one simply because its awesome
            //auto info = orbital_manager.bisect(tnum, 40, 1., 10.0, 15, 3, &voyager_probe, saturn, {earth, sun, jupiter});

            ///include a time to fire variable, wait that long then fire
            ///need a variable timestep before that is viable

            orbital* target = saturn;

            #define MOUSE_TARGETTING
            #ifdef MOUSE_TARGETTING
            target = orbital_manager.get_nearest(orbital_manager.olist, m, wh * 2.);
            //target = orbital_manager.get_nearest(asteroids, m, wh * 2.);
            #endif // MOUSE_TARGETTING

            current_target.target = target;
            current_target.me = currently_in_control;

            ///still not working for small targets
            ///its possible that its because we're slightly using the wrong equation
            ///but it seems to be the sun thats fucking everything up
            ///or it seems to be the tick in between
            current_target.target_distance = 0;//1. * 1000 * 1000 * 1000;

            const double max_error_distance = 0.1 * 1000 * 1000 * 1000;

            ///relative to velocity
            ///accurate settings
            /*const float angle_offset = 0.f;
            const float front_half_angle_cone = M_PI/2.f;
            const float angle_subdivisions = 10;

            const int num_vel_subdivisions = 8;

            const int num_recursions = 4;*/

            const float angle_offset = 0.f;
            const float front_half_angle_cone = M_PI/2.f;
            const float angle_subdivisions = 3;

            const int num_vel_subdivisions = 3;

            const int num_recursions = 20;

            ///if mindist > 1
            ///redo after some amount of distance

            ///so we're optimising this then
            ///we need to pinpoint the minimum in at least the second/third iteration, then terminate after 10% (or w/e) more than that
            ///in terms of time after the tick
            ///may make it invalid though. Progressively refine - c == 0 || c == 1 -> 100%, after reduce by (100 - (c - 2) / (max_c - 2))?

            ///if the retrieve thing is target, we get a min_dist error
            auto info = orbital_manager.bisect_with_cache(tnum, dt_s, dt_s,
                                               0.1, 0.1, 2000.0,
                                               angle_offset, front_half_angle_cone, angle_subdivisions,
                                               num_vel_subdivisions, num_recursions, current_target.target_distance, max_error_distance, probe, target);


            //auto info = orbital_manager.bisect(tnum, timestep, dt_s, 10, 0.5, 10.0, 30, 4, &voyager_probe, uranus, {earth, sun, neptune, saturn, jupiter, mercury, venus, mars});

            printf("Time %f\n", clk.getElapsedTime().asMilliseconds() / 1000.f);;

            printf("time course takes %f years\n", info.mtick * dt_s / 60 / 60 / 24 / 365);

            system("pause");

            #endif


            orbital* target = orbital_manager.get_nearest(orbital_manager.olist, m, wh * 2.);
            //target = orbital_manager.get_nearest(asteroids, m, wh * 2.);

            ///must happen before tick
            current_manov.start_journey(orbital_manager, target, target, nullptr, earth);
        }
        #endif

        /*if(once<sf::Mouse::Middle>())
        {
            orbital* target = orbital_manager.get_nearest(orbital_manager.olist, m, wh * 2.);

            double rad = (currently_in_control->pos - target->pos).length();

            double orbital_velocity = target->get_orbital_velocity(rad);

            ///need to add orbital velocity of sun?

            currently_in_control->orbit_speed(sun->get_orbital_velocity((currently_in_control->pos - sun->pos).length()), sun->pos);

            vec2d diff = currently_in_control->pos - currently_in_control->old_pos;

            currently_in_control->orbit_speed(orbital_velocity, target->pos);

            if(target != sun)
                currently_in_control->old_pos = currently_in_control->old_pos - diff;
        }*/

        if(key.isKeyPressed(sf::Keyboard::Escape) && win.hasFocus())
            win.close();

        ///its all drawing time
        if(tick < predefined_max_tick)
        {
            orbital_manager.tick(dt_s, dt_s);

            orbital_manager.tick_only_probes(dt_s, dt_s, asteroids, true);

            day += (dt_s / 60) / 60 / 24;

            tick++;
        }
        else
        {
            //orbital_manager.get_nearest(orbital_manager.olist, m, wh * 2.)->transitory_draw_col = {1, 0, 0};


            //orbital* ntarget = orbital_manager.get_nearest(orbital_manager.olist, m, wh * 2.);
            orbital* ntarget = orbital_manager.get_nearest(asteroids, m, wh * 2.);
            orbital* nhostile = orbital_manager.get_nearest(game_state.olist, m, wh*2.);

            orbital* target = orbital_manager.get_nearest({ntarget, nhostile}, m, wh * 2.);

            float mod = 0.001f;

            if(win.hasFocus())
            {
                if(key.isKeyPressed(sf::Keyboard::Up))
                    currently_in_control->acc.v[1] = -1 * mod;

                if(key.isKeyPressed(sf::Keyboard::Down))
                    currently_in_control->acc.v[1] = 1 * mod;

                if(key.isKeyPressed(sf::Keyboard::Left))
                    currently_in_control->acc.v[0] = -1 * mod;

                if(key.isKeyPressed(sf::Keyboard::Right))
                    currently_in_control->acc.v[0] = 1 * mod;

                if(once<sf::Keyboard::C>())
                {
                    manoeuvre_map[currently_in_control].uncapture();
                }

                if(once<sf::Mouse::Right>())
                {
                    manoeuvre_map[currently_in_control].man_list.clear();

                    manoeuvre_map[currently_in_control].make_single_trip(target);
                }

                /*if(once<sf::Mouse::Middle>())
                {
                    //current_mlist.man_list.clear();

                    //current_mlist.make_return_trip(target, earth);

                    manoeuvre_map[currently_in_control].make_single_trip(earth);
                }*/

                ///if there's only one asteroid left, we'll continuously recapture it
                ///we also need some kind of mechanism to remove conflicts to stop multiple satellites intercepting the same asteroid
                ///either expand the orbital* class (seems bad), or have a global skip list in game_state?
                ///we can just have a std::map in game_state, then set it as private member to that
                ///save/load is getting pretty iffy though

                ///so, if something gets absorbed along the path, we'll not intercept it (or we'll probably do it wrong)
                ///we need to turn on absorption in the tick_with_probes thing
                if(once<sf::Mouse::Middle>())
                {
                    ///orbit arg1, ditch into arg2
                    manoeuvre_map[currently_in_control].hunt_for_asteroids(jupiter, saturn, &game_state.olist, &game_state.skip_hunting);
                }

                ///have an ignore list? Use in get_nearest?
                ///we want auto target recognition and disposal
                if(once<sf::Mouse::Left>())
                {
                    //current_mlist.man_list.clear();

                    manoeuvre_map[currently_in_control].capture_and_ditch(target, jupiter);
                    //current_mlist.make_single_trip(earth);
                }

                if(once<sf::Keyboard::Tab>())
                {
                    game_state.spawn_hostile_asteroid(earth, sun);
                }

                ///prev ship
                if(once<sf::Keyboard::Z>())
                {
                    int i;

                    for(i=0; i<player_satellites.size(); i++)
                    {
                        if(currently_in_control == player_satellites[i])
                            break;
                    }

                    i--;

                    i = clamp(i, 0, player_satellites.size()-1);

                    currently_in_control = player_satellites[i];
                }

                ///next ship
                if(once<sf::Keyboard::X>())
                {
                    int i;

                    for(i=0; i<player_satellites.size(); i++)
                    {
                        if(currently_in_control == player_satellites[i])
                            break;
                    }

                    i++;

                    i = clamp(i, 0, player_satellites.size()-1);

                    currently_in_control = player_satellites[i];
                }

                ///make new ship
                if(once<sf::Keyboard::Q>())
                {
                    player_satellites.push_back(new orbital(*voyager_base));
                }

                ///will not work on planets
                /*if(once<sf::Mouse::Left>())
                {
                    current_mlist.make_return_capture(target, earth);
                }*/
            }

            if(key.isKeyPressed(sf::Keyboard::G) && key.isKeyPressed(sf::Keyboard::F) && win.hasFocus())
                dt_s += 100.;

            if(key.isKeyPressed(sf::Keyboard::H) && key.isKeyPressed(sf::Keyboard::F) && win.hasFocus())
            {
                dt_s -= 100.;

                dt_s = max(dt_s, 2000.);
            }

            if(once<sf::Keyboard::R>() && win.hasFocus())
                toggled_going = !toggled_going;

            ///why the christ isn't orbital manager keeping track of its own last_tick dt?
            if((key.isKeyPressed(sf::Keyboard::F) && win.hasFocus()) || toggled_going)
            {
                ///split into calculate and apply so we can do everything atomically?
                //current_mlist.tick_pre(orbital_manager);

                for(auto& i : manoeuvre_map)
                {
                    i.second.set_probe(i.first);

                    i.second.tick_pre(orbital_manager);
                }

                orbital_manager.tick(dt_s, dt_old);


                ///we need to pick the correct retrograde
                /*if(current_target.target != nullptr)
                {
                    double diff = (currently_in_control->pos - current_target.target->pos).length();

                    //if(diff < current_target.target_distance * 1.2)

                    ///yup, this was the issue!
                    if(diff < current_target.target->radius * 100)
                    {
                        double rad = diff;

                        double orbital_velocity = current_target.target->get_orbital_velocity(rad);

                        vec2d diff = current_target.target->pos - current_target.target->old_pos;

                        currently_in_control->orbit_speed(orbital_velocity, current_target.target->pos);

                        if(current_target.target != sun)
                            currently_in_control->old_pos = currently_in_control->old_pos - diff;

                        current_target.target = nullptr;
                    }
                }*/

                //current_mlist.tick_post(orbital_manager, sun);

                for(auto& i : manoeuvre_map)
                {
                    i.second.tick_post(orbital_manager, sun);
                }

                orbital_manager.tick_only_probes(dt_s, dt_old, asteroids, true);
                orbital_manager.tick_only_probes(dt_s, dt_old, game_state.olist, true);
                orbital_manager.tick_only_probes(dt_s, dt_old, player_satellites, false);

                day += (dt_s / 60) / 60 / 24;
                tick++;
            }

            if(once<sf::Keyboard::Num1>() && win.hasFocus())
            {
                save_to_file("planets.txt", orbital_manager.olist, dt_s, dt_old);
                save_to_file("asteroids.txt", asteroids, dt_s, dt_old);
            }

            if(once<sf::Keyboard::Num0>() && win.hasFocus())
            {
                orbital_manager.olist = load_from_file("planets.txt", dt_s, dt_old);
                asteroids = load_from_file("asteroids.txt", dt_s, dt_old);

                ///fixme
                for(int i=0; i<mainstream_orbitals.size(); i++)
                {
                    *mainstream_orbitals[i] = orbital_manager.olist[i];
                }
            }


            orbital_manager.draw_bulk(asteroids, win, 1);
            orbital_manager.draw_bulk(game_state.olist, win, 1.5);

            orbital_manager.draw_bulk(player_satellites, win, 2);

            currently_in_control->transitory_draw_col = {0, 1, 0};

            orbital_manager.draw_bulk({currently_in_control}, win, 3);

            orbital_manager.display(win);

            if(target && target == ntarget)
                target->transitory_draw_col = highlight_col;
            if(target && target == nhostile)
                target->transitory_draw_col = hostile_highlight;

            orbital_manager.draw_bulk({target}, win, 2);


            if(key.isKeyPressed(sf::Keyboard::F))
                orbital_manager.plot_orbit(currently_in_control, 1000, win);

            //for(auto& i : orbital_manager.olist)
            for(int i=0; i<descriptor_list.size(); i++)
            {
                pds desc = descriptor_list[i];

                int id = desc.id;

                orbital* o = orbital_manager.olist[id];

                ///uuh.. why does this forget window transformations? Am I dumb?
                vec2d screen_pos = manager::pos2screen(o->pos) + (vec2d){win.getSize().x/2., win.getSize().y/2.};

                ui_element elem(ui::TEXT);
                elem.set_relative_pos(conv<double, float>(screen_pos + (vec2d){10, 5}));

                std::string capd = desc.name;

                if(capd.size() > 0)
                    capd[0] = toupper(capd[0]);

                elem.text = capd;

                elem.draw(win);
            }

            ///we need to be able to update the asteroid names manager
            //if(asteroid_names.count(target) > 0)
            if(amanage.get_name(target) != "")
            {
                //pds desc = asteroid_names[target];
                orbital* o = target;

                vec2d screen_pos = manager::pos2screen(o->pos) + (vec2d){win.getSize().x/2., win.getSize().y/2.};

                ui_element elem(ui::TEXT);
                elem.set_relative_pos(conv<double, float>(screen_pos + (vec2d){10, 5}));

                //elem.text = desc.name;

                elem.text = amanage.get_name(target);

                elem.draw(win);
            }

            //    orbital_manager.plot_orbit(voyager_base, 1000, win);

            win.display();
            win.clear(sf::Color(0,0,0));

            dt_old = dt_s;
        }

        //day += (dt_s / 60) / 60 / 24;

        if(key.isKeyPressed(sf::Keyboard::T))
        {
            printf("%f day\n", day);
        }

        //printf("%f day\n", day);

        if(key.isKeyPressed(sf::Keyboard::B))
        {
            printf("%f ftime\n", ftime.getElapsedTime().asMicroseconds() / 1000.f);
        }
    }

    cout << "Hello world!" << endl;
    return 0;
}
