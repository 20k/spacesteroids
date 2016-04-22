#include <iostream>
#include <sfml/graphics.hpp>
#include <vec/vec.hpp>
#include <omp.h>

using namespace std;

#include "orbital.hpp"
#include "orbital_manager.hpp"
#include "spaceship.hpp"

#include "manoeuvre.hpp"


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
    settings.antialiasingLevel = 2;

    sf::RenderWindow win;
    win.create(sf::VideoMode(1200, 800), "hi", sf::Style::Default, settings);
    //win.setVerticalSyncEnabled(true);

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

    std::vector<orbital**> mainstream_orbitals = {&sun, &earth, &mercury, &venus, &mars, &jupiter, &saturn, &uranus, &neptune, &pluto};

    std::vector<orbital*> asteroids = populate_orbits_with_asteroids(jupiter, sun, 100);


    std::vector<orbital*> player_satellites;

    player_satellites.push_back(new orbital(*voyager_base));

    orbital* currently_in_control = player_satellites[0];

    //orbital* target_to_circularise = nullptr;

    //target_info current_target;

    //manoeuvre::manov current_manov;

    //current_manov.set_orbital(currently_in_control);

    manoeuvre::manov_list current_mlist;
    current_mlist.set_probe(currently_in_control);

    ///create new satellites, control them. Delete them as a debug, but in reality we have to crash them into something
    ///need for right click to work

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


            orbital* target = orbital_manager.get_nearest(orbital_manager.olist, m, wh * 2.);
            //orbital* target = orbital_manager.get_nearest(asteroids, m, wh * 2.);

            target->transitory_draw_col = {1, 0, 0};


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
                    current_mlist.uncapture();
                }

                if(once<sf::Mouse::Right>())
                {
                    current_mlist.man_list.clear();

                    current_mlist.make_single_trip(target);
                }

                if(once<sf::Mouse::Middle>())
                {
                    current_mlist.man_list.clear();

                    current_mlist.make_return_trip(target, earth);
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
                current_mlist.tick_pre(orbital_manager);

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

                current_mlist.tick_post(orbital_manager, sun);

                orbital_manager.tick_only_probes(dt_s, dt_old, asteroids, true);

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
            orbital_manager.draw_bulk(player_satellites, win, 2);

            orbital_manager.display(win);

            if(key.isKeyPressed(sf::Keyboard::F))
                orbital_manager.plot_orbit(currently_in_control, 1000, win);

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
