#include <iostream>
#include <sfml/graphics.hpp>
#include <vec/vec.hpp>

using namespace std;


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


typedef vec<2, double> vec2d;

double dt_s = 12000 / 5;
double dt_old = dt_s;


struct orbital
{
    double mass = 0;
    vec2d pos = {0,0};
    vec2d old_pos = {0,0};

    vec3f col = {1, 1, 1};

    ///so that we update atomically
    //vec2d next_pos = {0,0};

    vec2d acc = {0,0};

    double radius;

    void set_speed(double speed)
    {
        old_pos = perpendicular(pos).norm() * speed * pow(dt_s, 1.) + pos;
    }

    void set_speed(double speed, vec2d dir)
    {
        old_pos = pos - dir.norm() * speed * dt_s;
    }

    ///amount in ms
    void accelerate_relative_to_velocity(double amount, double angle_offset, double time_s)
    {
        vec2d vel_dir = (pos - old_pos).norm();

        ///the directional part of this is wrong
        acc = acc + vel_dir.rot(angle_offset) * amount * time_s * time_s;
    }

    orbital(double _mass, double _distance, double _speed, double rad = 0)
    {
        mass = _mass;
        pos = {_distance, 0};

        set_speed(_speed);

        radius = rad;
    }

    orbital()
    {

    }
};

struct manager
{
    std::vector<orbital*> olist;

    orbital* make_new(const orbital& orbit = orbital())
    {
        orbital* o = new orbital(orbit);

        olist.push_back(o);

        return o;
    }

    void destroy(orbital* o)
    {
        for(int i=0; i<(int)olist.size(); i++)
        {
            if(o == olist[i])
            {
                delete o;
                olist.erase(olist.begin() + i);
                return;
            }
        }
    }

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

    void tick()
    {
        for(int i=0; i<(int)olist.size(); i++)
        {
            orbital* o1 = olist[i];

            for(int j=i+1; j<(int)olist.size(); j++)
            {
                orbital* o2 = olist[j];

                ///0.0000000000674

                double G = 0.0000000000674;

                vec2d r12 = o2->pos - o1->pos;

                double r12l = r12.length_d();

                ///???
                ///radius of jupiter
                if(r12l < 69911 * pow(10, 3))
                    r12l = 69911 * pow(10, 3);

                vec2d F12 = - (o1->mass / r12l) * (o2->mass / r12l) * (r12.norm() * G);

                ///so its own mass cancels out in the change in velocity calvulations
                vec2d aF12 = -F12 / o1->mass;
                vec2d bF12 =  F12 / o2->mass;

                aF12 = aF12 * dt_s * dt_s;
                bF12 = bF12 * dt_s * dt_s;

                o1->acc += aF12;
                o2->acc += bF12;
            }
        }

        for(auto& i : olist)
        {
            vec2d o_pos = i->pos;

            i->pos = i->pos + (i->pos - i->old_pos) + i->acc;

            i->old_pos = o_pos;

            i->acc = {0,0};
        }
    }

    void draw(vec2d _pos, vec3f _col, sf::RenderTarget& win, float r = 4)
    {
        sf::RectangleShape shape;
        shape.setSize({r, r});
        shape.setOrigin({shape.getSize().x/2, shape.getSize().y/2});

        vec2d pos = pos2screen(_pos); ///- projection

        vec2f epos = {(float)pos.v[0] + win.getSize().x/2, (float)pos.v[1] + win.getSize().y/2};

        if(epos.v[0] < 0 || epos.v[0] >= win.getSize().x || epos.v[1] < 0 || epos.v[1] >= win.getSize().y)
            return;

        shape.setPosition({epos.v[0], epos.v[1]});

        vec3f col = _col * 255.f;

        shape.setFillColor({col.v[0], col.v[1], col.v[2]});

        win.draw(shape);
    }

    void display(sf::RenderTarget& win, float r = 4)
    {

        ///just assume

        vec2d sun;

        for(auto& i : olist)
        {
            draw(i->pos, i->col, win, r);
        }
    }

    void simulate(int ticks)
    {
        for(int i=0; i<ticks; i++)
        {
            tick();
        }
    }

    void smear(int ticks, sf::RenderTexture& tex)
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
    }

    vector<vector<vec2d>> test(int ticks, sf::RenderWindow* tex, bool render = true, orbital* test_orbital = nullptr, std::vector<orbital*> info_to_retrieve = std::vector<orbital*>())
    {
        std::vector<orbital> old;

        for(auto& i : olist)
        {
            old.push_back(*i);
        }

        orbital* my_test;

        if(test_orbital)
            my_test = make_new(*test_orbital);

        vector<vector<vec2d>> test_ret;

        //if(test_orbital)
        {
            test_ret.resize(info_to_retrieve.size() + 1);

            for(auto& i : test_ret)
            {
                i.reserve(ticks);
            }
        }

        for(int i=0; i<ticks; i++)
        {
            tick();

            if(render)
                display(*tex);

            if(i % 1 == 0 && render)
            {
                tex->display();
                tex->clear();
            }

            if(test_orbital)
            {
                test_ret[0].push_back(my_test->pos);
            }

            for(auto& j : olist)
            {
                for(int k=0; k<info_to_retrieve.size(); k++)
                {
                    if(j == info_to_retrieve[k])
                    {
                        test_ret[k + 1].push_back(j->pos);
                    }
                }
            }
        }

        if(test_orbital)
        {
            destroy(my_test);
        }

        for(int i=0; i<olist.size(); i++)
        {
            *olist[i] = old[i];
        }

        return test_ret;
    }

    ///returns the tick where the two objects are closest
    /*int get_minimum_distance(int ticks, orbital* test_orbital, orbital* target_orbital)
    {
        vector<vector<vec2d>> result = test(ticks, nullptr, false, test_orbital, {target_orbital});

        int min_tick = -1;

        double min_len = DBL_MAX;

        for(int tnum=0; tnum<result[0].size(); tnum++)
        {
            ///test
            vec2d d1 = result[0][tnum];
            ///target
            vec2d d2 = result[1][tnum];

            if((d1 - d2).length() < min_len)
            {
                min_len = (d1 - d2).length();
                min_tick = tnum;
            }
        }

        return min_tick;
    }*/

    int get_minimum_distance(int o1, int o2, const vector<vector<vec2d>>& pos)
    {
        int min_tick = -1;

        double min_len = DBL_MAX;

        for(int tnum=0; tnum<pos[o1].size(); tnum++)
        {
            ///test
            vec2d d1 = pos[o1][tnum];
            ///target
            vec2d d2 = pos[o2][tnum];

            if((d1 - d2).length() < min_len)
            {
                min_len = (d1 - d2).length();
                min_tick = tnum;
            }
        }

        return min_tick;
    }

    struct ret_info
    {
        vector<vector<vec2d>> ret;
        int mtick;
    };

    ret_info bisect(int ticks, float base_speed, float minimum, float maximum, int num_per_step, int depth, orbital* test_orbital = nullptr, orbital* target_orbital = nullptr, std::vector<orbital*> info_to_retrieve = std::vector<orbital*>(), int c = 0)
    {
        int which_id = -1;

        for(int i=0; i<info_to_retrieve.size(); i++)
        {
            if(info_to_retrieve[i] == target_orbital)
            {
                which_id = i + 1;
            }
        }

        ///add target orbital if its not found
        if(which_id == -1)
        {
            info_to_retrieve.insert(info_to_retrieve.begin(), target_orbital);

            //info_to_retrieve.push_back(target_orbital);
            //which_id = info_to_retrieve.size();

            which_id = 1;
        }

        double step = (maximum - minimum) / num_per_step;

        double min_min = DBL_MAX;
        int saved_mtick = -1;
        double found_speed = 1;
        double found_mod = minimum;

        vector<vector<vec2d>> backup;

        for(int i=0; i<num_per_step; i++)
        {
            orbital probe = *test_orbital;

            float ifrac = (float)i / num_per_step;

            double cur = (maximum - minimum) * ifrac + minimum;

            double speed = base_speed / cur;


            probe.accelerate_relative_to_velocity(1.f, 0, speed);


            auto last_experiment = this->test(ticks, nullptr, false, &probe, info_to_retrieve);

            int mtick = this->get_minimum_distance(0, which_id, last_experiment);


            vec2d min_voyager = last_experiment[0][mtick];
            vec2d min_mars = last_experiment[which_id][mtick];

            if((min_mars - min_voyager).length() < target_orbital->radius)
            {
                printf("HIT\n");
            }

            if((min_mars - min_voyager).length() < min_min)
            {
                min_min = (min_mars - min_voyager).length();
                //f_acc = cur;
                backup = last_experiment;
                saved_mtick = mtick;
                found_speed = speed;
                found_mod = cur;
            }
        }

        printf("found_acc %f\n", found_mod);
        printf("found mindist %f\n", min_min / 1000 / 1000 / 1000);

        if(c+1 < depth)
        {
            return bisect(ticks, base_speed, found_mod - step/2, found_mod + step/2, num_per_step, depth, test_orbital, target_orbital, info_to_retrieve, c+1);
        }

        return {backup, saved_mtick};
    }

    void plot(const vector<vector<vec2d>>& elements, int which_element, int which_tick, sf::RenderWindow& win, vec3f col = {1, 1, 1})
    {
        vec2d pos = elements[which_element][which_tick];

        draw(pos, col, win, 4);
    }

    void plot_all(const vector<vector<vec2d>>& elements, int which_tick, sf::RenderWindow& win, vector<vec3f> cols = vector<vec3f>())
    {
        //for(auto& i : elements)
        for(int i=0; i<elements.size(); i++)
        {
            vec3f col = {1,1,1};

            if(i < cols.size())
                col = cols[i];

            plot(elements, i, which_tick, win, col);
        }
    }
};

double manager::scale = 12.;
vec2d manager::offset;

int main()
{
    sf::RenderWindow win;
    win.create(sf::VideoMode(1200, 800), "hi");

    manager orbital_manager;

    orbital* sun = orbital_manager.make_new();

    sun->mass = 1.989 * pow(10., 30.);

    sun->col = {1.f, 0.8, 0.4};


    orbital* earth = orbital_manager.make_new(orbital(5.962 * pow(10., 24.), 149.6 * pow(10, 9), 30. * pow(10, 3), 6371 * pow(10, 3)));

    earth->col = {0.2f, 0.2f, 1.f};

    //earth->mass = 5.972 * pow(10., 24.);

    //earth->pos = {149.6 * 1000 * 1000 * 1000, 0};

    //const double earth_speed_ms = 30. * 1000;

    //earth->set_speed(earth_speed_ms);

    std::cout << "pos " << earth->pos <<  " opos " << earth->old_pos << std::endl;

    orbital* mercury = orbital_manager.make_new(orbital(3.3 * pow(10, 23), 57.9 * pow(10, 9), 47. * pow(10, 3)));

    orbital* venus = orbital_manager.make_new(orbital(4.87 * pow(10, 24), 108.2 * pow(10, 9), 35.02 * pow(10, 3)));

    orbital* mars = orbital_manager.make_new(orbital(6.42 * pow(10, 23), 227.9 * pow(10, 9), 24.077 * pow(10, 3), 3390 * pow(10, 3)));


    orbital* jupiter = orbital_manager.make_new(orbital(1.9 * pow(10, 27), 778.3 * pow(10, 9), 13.07 * pow(10, 3)));

    orbital* saturn = orbital_manager.make_new(orbital(5.69 * pow(10, 26), 1427. * pow(10, 9), 9.69 * pow(10, 3)));

    ///hehe. I get to laugh at uranus because I wrote this
    orbital* uranus = orbital_manager.make_new(orbital(8.68 * pow(10, 25), 2861. * pow(10, 9), 6.81 * pow(10, 3)));

    orbital* neptune = orbital_manager.make_new(orbital(1.02 * pow(10, 26), 4497.1 * pow(10, 9), 5.43 * pow(10, 3)));


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

        if(once<sf::Mouse::Right>())
        {
            orbital voyager_probe = *earth;
            voyager_probe.mass = 721.9;
            voyager_probe.col = {1, 0, 0};

            float base_speed = 12000;

            /*voyager_probe.accelerate_relative_to_velocity(1.f, 0, base_speed / 1.5);

            int tnum = 15000;

            last_experiment = orbital_manager.test(tnum, &win, false, &voyager_probe, {earth, mars});*/

            /*int tnum = 30000;

            int test_nums = 100;


            double min_min = DBL_MAX;
            double f_acc = 0.;
            int saved_mtick = -1;

            //float min_acc = 1.0;
            //float max_acc = 2.0;

            float min_acc = 1.193;
            float max_acc = 1.195;

            vector<vector<vec2d>> backup;

            for(int i=0; i<test_nums; i++)
            {
                orbital voyager_probe_this = voyager_probe;

                float ifrac = (float)i / test_nums;

                double cur = (max_acc - min_acc) * ifrac + min_acc;

                double speed = base_speed / cur;


                voyager_probe_this.accelerate_relative_to_velocity(1.f, 0, speed);


                last_experiment = orbital_manager.test(tnum, &win, false, &voyager_probe_this, {earth, mars});

                int mtick = orbital_manager.get_minimum_distance(0, 2, last_experiment);


                vec2d min_voyager = last_experiment[0][mtick];
                vec2d min_mars = last_experiment[2][mtick];

                if((min_mars - min_voyager).length() < mars->radius)
                {
                    printf("HIT\n");
                }

                if((min_mars - min_voyager).length() < min_min)
                {
                    min_min = (min_mars - min_voyager).length();
                    f_acc = cur;
                    backup = last_experiment;
                    saved_mtick = mtick;
                }
            }

            printf("%f min_min %f speed\n", min_min / 1000. / 1000. / 1000., f_acc);
            printf("%f target radius in millionkm\n", mars->radius / 1000. / 1000. / 1000);*/

            int tnum = 40000;

            auto info = orbital_manager.bisect(tnum, 12000, 0.5, 5.0, 20, 5, &voyager_probe, mars, {earth, sun});

            double min_dist = (info.ret[0][info.mtick] - info.ret[1][info.mtick]).length();

            printf("min_dist %f\n", min_dist / 1000 / 1000 / 1000);


            ///tomorrow's homework
            ///shoot probes out in all directions, see which ones come closest to target
            for(int i=0; i<tnum; i++)
            {
                orbital_manager.plot_all(info.ret, i, win, {{0, 1, 0}, {1, 0, 0}, earth->col, sun->col});


                if(info.mtick == i)
                    system("pause");

                int every = 2;

                if(i % every == 0)
                {
                    win.display();
                    win.clear();
                }
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
            orbital_manager.tick();
        else
        {
            orbital_manager.display(win);

            //win.draw(spr);

            win.display();
            win.clear(sf::Color(0,0,0));
        }

        tick++;

        day += (dt_s / 60) / 60 / 24;

        //printf("%f day\n", day);
    }

    cout << "Hello world!" << endl;
    return 0;
}
