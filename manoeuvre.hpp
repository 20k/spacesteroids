#ifndef MANOEUVRE_HPP_INCLUDED
#define MANOEUVRE_HPP_INCLUDED

#include "orbital.hpp"
#include "orbital_manager.hpp"

#include <map>
#include <future>
#include <thread>

///consider increasing dt_s
extern double dt_s;

struct manager;

///we also need a struct
namespace manoeuvre
{
    ///just have a delay manoueervere?
    enum manov_type : int32_t
    {
        NONE = 2 << 0,
        INTERCEPT = 2 << 1,
        ORBIT = 2 << 2,
        CAPTURE = 2 << 3,
        BE_NEAR = 2 << 4,
        WAIT = 2 << 5,
        UNCAPTURE = 2 << 6,
        THREAD_WAITER = 2 << 7,
    };

    ///lets totally scrap the manov structure

    struct manv
    {
        ret_info* inf = nullptr;
        std::shared_ptr<std::future<void>> intercept_future;

        std::thread* intercept_thread;

        double time_elapsed = 0;
        int ticks_elapsed = 0;

        bool fin = false;

        manov_type type = NONE;

        orbital* target = nullptr;

        bool blocking = true;

        std::map<std::string, double> args;


        manv(orbital* _target, manov_type _type);

        void set_arg(const std::string& a, double dat);

        bool does(manov_type e);

        std::vector<manv> tick_pre(manager& orbital_manager, orbital* probe, float dt);

        std::vector<manv> tick_post(manager& orbital_manager, orbital* probe, orbital* sun, float dt);

        bool complete();

        bool blocks();
    };

    struct manov_list
    {
        orbital* parent = nullptr;

        std::vector<manv> man_list;

        void set_probe(orbital* probe);

        void tick_pre(manager& orbital_manager);
        void tick_post(manager& orbital_manager, orbital* sun);

        void make_return_trip(orbital* target, orbital* home);
        void make_single_trip(orbital* target);
        void make_return_capture(orbital* target, orbital* home);

        void capture_and_ditch(orbital* target, orbital* ditch_into);

        void uncapture();
    };

    #if 0
    struct manov
    {
        manov_type type = NONE;
        double intercept_dist = 0;

        float hold_time = 100; ///seconds
        float current_elapsed_time = 0;

        int stage = 0;

        orbital* probe = nullptr;

        orbital* target = nullptr;
        orbital* orbit = nullptr;
        orbital* capture = nullptr;
        orbital* home = nullptr;

        orbital* child = nullptr;

        bool does(manov_type t)
        {
            return (type & t) > 0;
        }

        void set_orbital(orbital* _probe)
        {
            probe = _probe;
        }

        void update_child()
        {
            if(child)
            {
                child->pos = probe->pos;
                child->old_pos = probe->old_pos;
            }
        }

        void start_journey(manager& orbital_manager, orbital* _target = nullptr, orbital* _orbit = nullptr, orbital* _capture = nullptr, orbital* _home = nullptr)
        {
            target = _target;
            orbit = _orbit;
            capture = _capture;
            home = _home;

            if(!target)
                return;

            const double max_error_distance = 0.1 * 1000 * 1000 * 1000;

            sf::Clock clk;

            auto info = orbital_manager.bisect_with_cache(100000, dt_s, dt_s,
                                                          0.1, 0.1, 2000.0,
                                                          0., M_PI/2., 3,
                                                          3, 20, 0., max_error_distance, probe, target);

            printf("Time elapsed %f\n", clk.getElapsedTime().asMicroseconds() / 1000. / 1000.);

            printf("time course takes %f years\n", info.mtick * dt_s / 60 / 60 / 24 / 365);

            target = nullptr;
        }

        void tick(manager& orbital_manager, orbital* sun, float dt)
        {
            current_elapsed_time += dt;


            ///we need to pick the correct retrograde
            if(orbit != nullptr)
            {
                double diff = (probe->pos - orbit->pos).length();

                //if(diff < orbit_distance * 1.2)

                ///yup, this was the issue!
                if(diff < orbit->radius * 100)
                {
                    double rad = diff;

                    double orbital_velocity = orbit->get_orbital_velocity(rad);

                    vec2d diff = orbit->pos - orbit->old_pos;

                    probe->orbit_speed(orbital_velocity, orbit->pos);

                    if(orbit != sun)
                        probe->old_pos = probe->old_pos - diff;

                    orbit = nullptr;

                    current_elapsed_time = 0;
                }
            }

            if(capture != nullptr)
            {
                double diff = (probe->pos - capture->pos).length();

                if(diff < 0.1 * 1000 * 1000 * 1000)
                {
                    child = capture;

                    capture = nullptr;
                }
            }

            update_child();
        }

        ///if we want to initiate, it has to be in here
        void pre_mainstream_tick(manager& orbital_manager)
        {
            update_child();

            if(current_elapsed_time < hold_time * dt_s)
                return;

            ///return home
            if(target == nullptr && orbit == nullptr && capture == nullptr && home != nullptr)
            {
                start_journey(orbital_manager, home, home, nullptr, nullptr);
            }

            update_child();
        }
    };
    #endif

    //static manov_t asteroid_capture = (manov_t)(INTERCEPT | ORBIT | CAPTURE | RETURN);
    //static manov_t orbital_intercept = (manov_t)(INTERCEPT | ORBIT);
    //static manov_t recon = (manov_t)(INTERCEPT | RETURN);

    /*bool does(manov_t m, manov_t t)
    {
        return (m & t) > 0;
    }*/
}

#endif // MANOEUVRE_HPP_INCLUDED
