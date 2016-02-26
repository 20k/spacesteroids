#include <iostream>
#include <sfml/graphics.hpp>
#include <vec/vec.hpp>

using namespace std;

typedef vec<2, double> vec2d;

struct orbital
{
    double mass = 0;
    vec2d pos = {0,0};
    vec2d old_pos = {0,0};

    ///so that we update atomically
    vec2d next_pos = {0,0};
};

struct manager
{
    constexpr static double dt_s = 1200;

    std::vector<orbital*> olist;

    orbital* make_new()
    {
        orbital* o = new orbital();

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

    void tick(sf::RenderWindow& win)
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

                //std::cout << "Dist: " << r12l << std::endl;

                vec2d F12 = - (o1->mass / r12l) * (o2->mass / r12l) * (r12.norm() * G);

                vec2d aF12 = -F12 / o1->mass;
                vec2d bF12 =  F12 / o2->mass;

                aF12 = aF12 * dt_s * dt_s;
                bF12 = bF12 * dt_s * dt_s;

                vec2d next_o1 = o1->pos + (o1->pos - o1->old_pos) + aF12;
                vec2d next_o2 = o2->pos + (o2->pos - o2->old_pos) + bF12;

                o1->next_pos = next_o1;
                o2->next_pos = next_o2;

                //printf("%f %f %f %f\n", F12.v[0], F12.v[1], o1->pos.v[0], o1->pos.v[1]);

                //printf("diff %f %f\n", bF12.v[0], bF12.v[1]);

                //printf("len %f\n", r12l);
            }
        }

        for(auto& i : olist)
        {
            i->old_pos = i->pos;
            i->pos = i->next_pos;
        }

        for(auto& i : olist)
        {
            vec2d pos = i->pos;

            double scale = 1000 * 1000 * 1000;

            pos = pos / scale;

            sf::RectangleShape shape;
            shape.setSize({10, 10});

            shape.setOrigin({5, 5});
            shape.setPosition({(float)pos.v[0] + 400, (float)pos.v[1] + 400});

            win.draw(shape);

            //printf("%f %f\n", pos.v[0], pos.v[1]);
        }
    }
};

int main()
{
    sf::RenderWindow win;
    win.create(sf::VideoMode(800, 600), "hi");

    manager orbital_manager;

    orbital* sun = orbital_manager.make_new();

    sun->mass = 1.989 * pow(10., 30.);


    orbital* earth = orbital_manager.make_new();

    earth->mass = 5.972 * pow(10., 24.);

    earth->pos = {149.6 * 1000 * 1000 * 1000, 0};

    const double earth_speed_ms = 30. * 1000;

    earth->old_pos = perpendicular(earth->pos).norm() * earth_speed_ms * pow(manager::dt_s, 1.) + earth->pos; ///need to times this by dt

    std::cout << "pos " << earth->pos <<  " opos " << earth->old_pos << std::endl;


    ///earth->speed = 30km/s


    float day = 0;

    int tick = 0;

    sf::Event event;

    while(win.isOpen())
    {
        while(win.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                win.close();
        }

        orbital_manager.tick(win);

        win.display();
        win.clear(sf::Color(0,0,0));

        tick++;

        day += (manager::dt_s / 60) / 60 / 24;

        printf("%f day\n", day);
    }

    cout << "Hello world!" << endl;
    return 0;
}
