#include <iostream>
#include <sfml/graphics.hpp>
#include <vec/vec.hpp>

using namespace std;

#include "orbital.hpp"
#include "orbital_manager.hpp"
#include "spaceship.hpp"


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

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow win;
    win.create(sf::VideoMode(1200, 800), "hi", sf::Style::Default, settings);
    //win.setVerticalSyncEnabled(true);

    manager orbital_manager;

    orbital* sun = orbital_manager.make_new();

    sun->mass = 1.989 * pow(10., 30.);

    sun->col = {1.f, 0.8, 0.4};

    sun->radius = 696342.f * pow(10, 3);


    orbital* earth = orbital_manager.make_new(orbital(5.962 * pow(10., 24.), 149.6 * pow(10, 9), 30. * pow(10, 3), 6371 * pow(10, 3)));

    earth->col = {0.2f, 0.2f, 1.f};

    std::cout << "pos " << earth->pos <<  " opos " << earth->old_pos << std::endl;

    orbital* mercury = orbital_manager.make_new(orbital(3.3 * pow(10, 23), 57.9 * pow(10, 9), 47. * pow(10, 3), 2440 * pow(10, 3)));

    orbital* venus = orbital_manager.make_new(orbital(4.87 * pow(10, 24), 108.2 * pow(10, 9), 35.02 * pow(10, 3), 6052 * pow(10, 3)));

    orbital* mars = orbital_manager.make_new(orbital(6.42 * pow(10, 23), 227.9 * pow(10, 9), 24.077 * pow(10, 3), 3390 * pow(10, 3)));


    orbital* jupiter = orbital_manager.make_new(orbital(1.9 * pow(10, 27), 778.3 * pow(10, 9), 13.07 * pow(10, 3), 69911 * pow(10, 3)));

    orbital* saturn = orbital_manager.make_new(orbital(5.69 * pow(10, 26), 1427. * pow(10, 9), 9.69 * pow(10, 3), 58232 * pow(10, 3)));

    ///hehe. I get to laugh at uranus because I wrote this
    orbital* uranus = orbital_manager.make_new(orbital(8.68 * pow(10, 25), 2861. * pow(10, 9), 6.81 * pow(10, 3), 25362 * pow(10, 3)));

    orbital* neptune = orbital_manager.make_new(orbital(1.02 * pow(10, 26), 4497.1 * pow(10, 9), 5.43 * pow(10, 3), 24622 * pow(10, 3)));

    ///we will define pluto at its closest point to the sun, ie smallest distance + largest velocity
    ///the eccentricity will not be correct relative the other planets, but as they're all perfect circles that's just dandy at the moment
    ///we need everything's rotation from the centre, /time offset, whatever its called
    ///nope, make it aphelion, otherwise its inside neptune
    orbital* pluto = orbital_manager.make_new(orbital(1.309 * pow(10, 22), 7375.93 * pow(10, 9), 3.71 * pow(10, 3), 1187 * pow(10, 3)));

    orbital* voyager_base = orbital_manager.make_new(orbital(5.962 * pow(10., 24.), 149.6 * pow(10, 9), 30. * pow(10, 3), 2));

    voyager_base->mass = 721.9;
    voyager_base->col = {1, 0, 0};


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

    int predefined_max_tick = 40000;

    vector<vector<vec2d>> last_experiment;

    while(win.isOpen())
    {
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

        vec2d world_p = manager::screen2pos(m, wh);

        /*if(once<sf::Mouse::Left>())
        {
            orbital* getrekd = orbital_manager.make_new();
            getrekd->mass = pow(10., 3.);
            getrekd->pos = world_p;
            getrekd->set_speed(13.07 * pow(10, 3) * 10, {0, -1});
        }*/

        ///need to fiddle with constant acceleration rather than impulse
        ///although then again, do I?
        if(once<sf::Mouse::Right>())
        {
            orbital voyager_probe = *earth;
            voyager_probe.mass = 721.9;
            voyager_probe.col = {1, 0, 0};

            float base_speed = dt_s;


            sf::Clock clk;

            int tnum = 100000;

            ///keep this one simply because its awesome
            //auto info = orbital_manager.bisect(tnum, 40, 1., 10.0, 15, 3, &voyager_probe, saturn, {earth, sun, jupiter});

            ///include a time to fire variable, wait that long then fire
            ///need a variable timestep before that is viable

            float timestep = dt_s * 3;

            ///way too expensive to solve directly
            auto info = orbital_manager.bisect(tnum, timestep, dt_s, 10, 0.5, 10.0, 30, 4, &voyager_probe, saturn, {earth, sun, jupiter});
            //auto info = orbital_manager.bisect(tnum, timestep, dt_s, 10, 0.5, 10.0, 30, 4, &voyager_probe, uranus, {earth, sun, neptune, saturn, jupiter, mercury, venus, mars});

            double min_dist = (info.ret[0][info.mtick] - info.ret[1][info.mtick]).length();

            printf("min_dist %f\n", min_dist / 1000 / 1000 / 1000);

            printf("Time %f\n", clk.getElapsedTime().asMilliseconds() / 1000.f);

            printf("time course takes %f years\n", info.mtick * timestep / 60 / 60 / 24 / 365);

            system("pause");


            ///tomorrow's homework
            ///shoot probes out in all directions, see which ones come closest to target
            for(int i=0; i<tnum; i++)
            {
                orbital_manager.plot_all(info.ret, i, win, {{0, 1, 0}, {1, 0, 0}, earth->col, sun->col});

                if(info.mtick == i)
                    system("pause");

                int every = 1;

                if(i % every == 0)
                {
                    //win.display();
                    //win.display();
                    win.display();
                    /*win.display();
                    win.display();
                    win.display();
                    win.display();
                    win.display();
                    win.display();*/
                    win.clear();
                }

                float dmouse = 0;

                while(win.pollEvent(event))
                {
                    if(event.type == sf::Event::Closed)
                        win.close();

                    if(event.type == sf::Event::MouseWheelScrolled)
                    {
                        dmouse += event.mouseWheelScroll.delta;
                    }
                }

                if(key.isKeyPressed(sf::Keyboard::LShift))
                {
                    dmouse *= 5.f;
                }

                manager::scale -= dmouse;

                if(manager::scale < 1)
                    manager::scale = 1;

            }

            /*
            ///voyager, mars
            int min_tick = orbital_manager.get_minimum_distance(0, 2, last_experiment);

            vec2d min_voyager = last_experiment[0][min_tick];
            vec2d min_mars = last_experiment[2][min_tick];

            if((min_mars - min_voyager).length() < mars->radius)
            {
                printf("HIT\n");
            }

            printf("min %i dist %f\n", min_tick, (min_mars - min_voyager).length() / 1000. / 1000. / 1000.);*/

            //int min_tick = orbital_manager.get_minimum_distance(10000, &voyager_probe, &mars);

            //printf("")

            //system("pause");

            /*for(int j=0; j<last_experiment[1].size(); j++)
            {
                orbital_manager.plot(last_experiment, 1, j, win);

                win.display();
                win.clear(sf::Color(0,0,0));
            }*/
        }

        if(key.isKeyPressed(sf::Keyboard::Escape))
            win.close();

        ///its all drawing time
        if(tick < predefined_max_tick)
            orbital_manager.tick(dt_s, dt_s);
        else
        {
            float mod = 0.001f;

            if(key.isKeyPressed(sf::Keyboard::Up))
                voyager_base->acc.v[1] = -1 * mod;

            if(key.isKeyPressed(sf::Keyboard::Down))
                voyager_base->acc.v[1] = 1 * mod;

            if(key.isKeyPressed(sf::Keyboard::Left))
                voyager_base->acc.v[0] = -1 * mod;

            if(key.isKeyPressed(sf::Keyboard::Right))
                voyager_base->acc.v[0] = 1 * mod;


            ///why the christ isn't orbital manager keeping track of its own last_tick dt?
            if(key.isKeyPressed(sf::Keyboard::F))
            {
                orbital_manager.tick(dt_s, dt_s);
            }

            orbital_manager.display(win);

            if(key.isKeyPressed(sf::Keyboard::F))
                orbital_manager.plot_orbit(voyager_base, 1000, win);


            win.display();
            win.clear(sf::Color(0,0,0));
        }

        /*if(true)
        {
            orbital_manager.display(win);

            win.display();
            win.clear(sf::Color(0,0,0));
        }*/

        tick++;

        day += (dt_s / 60) / 60 / 24;

        //printf("%f day\n", day);
    }

    cout << "Hello world!" << endl;
    return 0;
}
