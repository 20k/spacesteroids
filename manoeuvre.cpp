#include "manoeuvre.hpp"

#include "orbital_manager.hpp"

using namespace manoeuvre;

manv::manv(orbital* _target, manov_type _type)
{
    target = _target;
    type = _type;
}

void manv::set_arg(const std::string& a, double dat)
{
    args[a] = dat;
}

bool manv::does(manov_type e)
{
    return (type & e) > 0;
}

std::vector<manv> manv::tick_pre(manager& orbital_manager, orbital* probe, float dt)
{
    if(does(INTERCEPT))
    {
        if(!target)
        {
            fin = true;

            return std::vector<manv>();
        }

        if(target->skip)
        {
            fin = true;
            return {};
        }


        ///696 * 1000 * 1000 is the radius of the sun
        ///so this is 100 * 1000 * 1000
        double max_error_distance = 0.1 * 1000 * 1000 * 1000;

        max_error_distance = max(max_error_distance, target->radius * 2);

        int max_len_yr = 20 * 365 * 24 * 60 * 60 / dt_s;

        printf("%f mly ticks\n", max_len_yr);

        sf::Clock clk;

        double travel_angle = (probe->pos - probe->old_pos).angle();

        double target_angle = (target->pos - probe->pos).angle();

        float test_offset = target_angle - travel_angle;

        ///do lots of really short trips?
        /*auto info = orbital_manager.bisect_with_cache(max_len_yr, dt_s, dt_s,
                    0.1, 0.1, 2000.0,
                    test_offset, M_PI/2., 3,
                    3, 16, 0., max_error_distance, probe, target);*/

        int tc;

        for(tc = 0; tc < orbital_manager.olist.size(); tc++)
        {
            if(orbital_manager.olist[tc] == target)
            {
                printf("found obj %i\n", tc);

                break;
            }
        }

        //printf("%f %f  n %f %f\n", EXPAND_2(probe->pos), EXPAND_2(target->pos));


        manager* oclone = orbital_manager.clone();
        orbital* pclone = probe->clone();

        orbital* tclone;

        if(tc < oclone->olist.size())
            tclone = oclone->olist[tc];
        else
            tclone = target->clone();

        //printf("nt %f %f %f %f\n", EXPAND_2(tclone->pos), tclone->mass, target->mass);


        //printf("Time elapsed %f\n", clk.getElapsedTime().asMicroseconds() / 1000. / 1000.);

        //printf("time course takes %f years\n", info.mtick * dt_s / 60 / 60 / 24 / 365);

        ///if mtick near max_ticks
        ///delay for longer

        fin = true;


        /*double dist = info.mdist;
        int tick = info.mtick;

        ///readjust at the halfway point
        double mtime = tick * dt_s / 10;

        mtime = std::max(mtime, 400. * dt_s);

        printf("tickt %i\n", tick);

        if(dist > max_error_distance)
        {
            manv delay(target, WAIT);
            manv adjust(target, INTERCEPT);

            delay.set_arg("time", mtime);

            return {delay, adjust};
        }*/


        double delay_ticks = 3000;

        bool in_mainstream = oclone->contains(tclone);

        for(int i=0; i<delay_ticks; i++)
        {
            oclone->tick(dt_s, dt_s);
            oclone->tick_only_probes(dt_s, dt_s, {pclone}, false);

            ///absorption?
            if(!in_mainstream)
                oclone->tick_only_probes(dt_s, dt_s, {tclone}, false);
        }

        ///will have target and probe available as arguments
        manv join(target, THREAD_WAITER);
        join.inf = new ret_info;

        join.set_arg("dticks", delay_ticks);

        ///tick oclone forwards by x ticks
        ///save that

        ///next add ticks into the future to resolve this
        ///then when time_elapsed == that, force block and execute
        ///should probably count ticks in main and pass it here so there's no discrepancy
        arg_s bargs = {max_len_yr, oclone, test_offset, max_error_distance, pclone, tclone, join.inf};

        //join.intercept_future = std::shared_ptr<std::future<void>>(new std::future<void>(std::async(std::launch::async, bisect_wrapper, bargs)));

        //auto fut = std::async(std::launch::deferred, bisect_wrapper, bargs);

        //fut.wait();

        join.intercept_thread = new std::thread(std::bind(bisect_wrapper, bargs));

        //join.intercept_thread->join();

        printf("launch\n");

        /*if(!join.intercept_future->valid())
        {
            printf("Not valid?!\n");
        }*/

        /*join.intercept_future->wait();

        probe->accelerate_relative_to_velocity(join.inf->found_speed, join.inf->next_angle_offset, 1200);*/

        //printf("%f %f\n", info.found_speed, info.next_angle_offset);

        //printf("Time %f\n", clk.getElapsedTime().asMicroseconds() / 1000. / 1000.);

        return {join};
    }

    if(does(THREAD_WAITER))
    {
        //printf("ticks %i %f\n", ticks_elapsed, args["dticks"]);

        if(ticks_elapsed < args["dticks"])
            return std::vector<manv>();

        printf("fin %i\n", ticks_elapsed);


        intercept_thread->join();

        printf("Years %f\n", inf->mtick * dt_s / 60 / 60 / 24 / 365);

        ///?
        probe->accelerate_relative_to_velocity(inf->found_speed, inf->next_angle_offset, 1200);


        ///relaunch if there's too much err
        double max_error_distance = 0.1 * 1000 * 1000 * 1000;

        max_error_distance = max(max_error_distance, target->radius * 2);

        double dist = inf->mdist;
        int tick = inf->mtick;

        ///readjust at the halfway point
        ///does not work if we time accelerate, need to save dt_s?
        //double mtime = tick * dt_s / 10;

        //mtime = std::max(mtime, 400. * dt_s);

        printf("tickt %i\n", tick);

        std::vector<manv> next_manouvers;

        if(dist > max_error_distance)
        {
            //manv delay(target, WAIT);
            manv adjust(target, INTERCEPT);

            //delay.set_arg("time", mtime);

            next_manouvers = {adjust};
        }

        delete inf;

        delete intercept_thread;

        fin = true;

        return next_manouvers;
    }

    ///need to be able to specify a separate ditch and guard target. Eg we might be stationed around neptune
    ///but want to ditch into jupiter
    ///similarly, we might simply want to parent to a relative point in space, eg a lagrange point, or simply just somewhere
    ///relative to a body
    ///but we'd need a proper ui for that
    if(does(AUTOMATIC_ASTEROID_HUNT))
    {
        if(hostile_targets == nullptr)
        {
            fin = true;
            return std::vector<manv>();
        }

        orbital* nearest = orbital_manager.get_nearest_with_skip_map(*hostile_targets, probe, *skip_hunting);

        ///we can make this an upgradable quantity per satellite
        ///as you get new tech, range becomes longer, auto killer satellites become more robust
        double max_dist = earth_sun_dist * 30.;

        ///hunt
        if(nearest && (nearest->pos - probe->pos).length() < max_dist)
        {
            (*skip_hunting)[nearest] = true;

            manv m1(nearest, INTERCEPT);
            manv m2(nearest, BE_NEAR);

            manv mcapture(nearest, CAPTURE);

            manv md(ditch_target, INTERCEPT);
            manv mw(ditch_target, WAIT);
            manv ditch(ditch_target, UNCAPTURE);

            manv ri(target, INTERCEPT);
            manv hm(target, ORBIT);
            manv asteroid_hunt(target, AUTOMATIC_ASTEROID_HUNT);

            mw.set_arg("time", 10 * dt_s);

            asteroid_hunt.hostile_targets = hostile_targets;
            asteroid_hunt.ditch_target = ditch_target;
            asteroid_hunt.skip_hunting = skip_hunting;

            fin = true;

            return {m1, m2, mcapture, md, mw, ditch, ri, hm, asteroid_hunt};
        }
    }

    if(does(BE_NEAR))
    {
        if(!target || target->skip)
        {
            fin = true;
            return {};
        }

        double diff = (target->pos - probe->pos).length();

        if(diff < 0.4 * 1000 * 1000 * 1000)
        {
            fin = true;
        }
    }

    if(does(WAIT))
    {
        //printf("wtime %f %f\n", args["time"], time_elapsed);

        if(time_elapsed >= args["time"])
            fin = true;
    }

    ///async tasks working badly
    ///if a new intercept gets brought into the fold, it gets blocked
    if(does(CAPTURE))
    {
        blocking = false;

        ///we need some way to uncapture
        /*vec2d diff = probe->pos - probe->old_pos;

        target->pos = target->old_pos + diff;*/

        if(!target || target->skip)
        {
            ///don't fin, itll break the uncapture
            //fin = true;
            return {};
        }

        target->pos = probe->pos;
        target->old_pos = probe->old_pos;
    }

    if(does(UNCAPTURE))
    {
        fin = true;
    }

    return std::vector<manv>();
}

std::vector<manv> manv::tick_post(manager& orbital_manager, orbital* probe, orbital* sun, float dt)
{
    if(does(ORBIT))
    {
        if(target == nullptr || target->skip)
        {
            fin = true;
            return {};
        }


        double diff = (probe->pos - target->pos).length();

        //if(diff < orbit_distance * 1.2)

        ///yup, this was the issue!
        if(diff < target->radius * 100)
        {
            double rad = diff;

            double orbital_velocity = target->get_orbital_velocity(rad);

            vec2d diff = target->pos - target->old_pos;

            probe->orbit_speed(orbital_velocity, target->pos);

            if(target != sun)
                probe->old_pos = probe->old_pos - diff;

            fin = true;
        }

        //printf("torbit\n");
    }

    time_elapsed += dt;
    ticks_elapsed++;

    return std::vector<manv>();
}

bool manv::complete()
{
    return fin;
}

bool manv::blocks()
{
    return blocking;
}


void manov_list::set_probe(orbital* probe)
{
    parent = probe;
}

void manov_list::tick_pre(manager& orbital_manager)
{
    for(int i=0; i<man_list.size(); i++)
    {
        auto add = man_list[i].tick_pre(orbital_manager, parent, dt_s);

        for(int j=0; j<add.size(); j++)
            man_list.insert(man_list.begin() + i + 1 + j, add[j]);

        if(man_list[i].does(UNCAPTURE))
        {
            uncapture();
            i--;
        }

        if(man_list[i].complete())
        {
            man_list.erase(man_list.begin() + i);
            i--;

            continue;
        }

        if(man_list[i].blocks())
            return;
    }
}

void manov_list::tick_post(manager& orbital_manager, orbital* sun)
{
    for(int i=0; i<man_list.size(); i++)
    {
        auto add = man_list[i].tick_post(orbital_manager, parent, sun, dt_s);

        for(int j=0; j<add.size(); j++)
            man_list.insert(man_list.begin() + i + 1 + j, add[j]);

        ///if we uncapture, itll break
        if(man_list[i].does(UNCAPTURE))
        {
            uncapture();
            i--; ///assumes a correct stack
        }

        if(man_list[i].complete())
        {
            man_list.erase(man_list.begin() + i);
            i--;

            continue;
        }

        if(man_list[i].blocks())
            return;
    }
}

void manov_list::make_return_trip(orbital* target, orbital* home)
{
    manv m1(target, INTERCEPT);
    manv mwait(target, ORBIT);
    manv mdelay(target, WAIT);
    manv m2(home, INTERCEPT);
    manv morb(home, ORBIT);

    mdelay.set_arg("time", 1000 * dt_s);

    man_list.push_back(m1);
    man_list.push_back(mwait);
    man_list.push_back(mdelay);
    man_list.push_back(m2);
    man_list.push_back(morb);
}

void manov_list::make_single_trip(orbital* target)
{
    manv m1(target, INTERCEPT);
    manv mwait(target, ORBIT);

    man_list.push_back(m1);
    man_list.push_back(mwait);
}

void manov_list::make_return_capture(orbital* target, orbital* home)
{
    manv m1(target, INTERCEPT);
    manv bn(target, BE_NEAR);
    //manv mwait(target, ORBIT);
    //manv mdelay(target, WAIT);
    manv mcapture(target, CAPTURE);
    manv m2(home, INTERCEPT);
    manv morb(home, ORBIT);

    //mdelay.set_arg("time", 1000 * dt_s);

    man_list.push_back(m1);
    man_list.push_back(bn);
    //man_list.push_back(mwait);
    //man_list.push_back(mdelay);
    man_list.push_back(mcapture);
    man_list.push_back(m2);
    man_list.push_back(morb);
}

///seems to be hard to ditch things into the sun
void manov_list::capture_and_ditch(orbital* target, orbital* ditch_into)
{
    manv m1(target, INTERCEPT);
    manv m2(target, BE_NEAR);

    manv mcapture(target, CAPTURE);

    manv md(ditch_into, INTERCEPT);
    manv mw(ditch_into, WAIT);
    manv ditch(ditch_into, UNCAPTURE);
    //manv morb(ditch_into, ORBIT);

    mw.set_arg("time", 10 * dt_s);

    man_list.push_back(m1);
    man_list.push_back(m2);

    man_list.push_back(mcapture);

    man_list.push_back(md);
    man_list.push_back(mw);
    man_list.push_back(ditch);
    //man_list.push_back(morb);
}

void manov_list::hunt_for_asteroids(orbital* parent, orbital* ditch_into, std::vector<orbital*>* hostile_asteroids, std::map<orbital*, bool>* hostile_skip_list)
{
    manv mo(parent, INTERCEPT);
    manv mr(parent, ORBIT);

    manv m1(parent, AUTOMATIC_ASTEROID_HUNT);
    m1.hostile_targets = hostile_asteroids;
    m1.ditch_target = ditch_into;
    m1.skip_hunting = hostile_skip_list;

    man_list.push_back(mo);
    man_list.push_back(mr);
    man_list.push_back(m1);
}


void manov_list::uncapture()
{
    for(int i=0; i<man_list.size(); i++)
    {
        if(man_list[i].does(CAPTURE))
        {
            man_list.erase(man_list.begin() + i);
            i--;
            return;
        }
    }
}

