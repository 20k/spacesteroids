#include "orbital_manager.hpp"

double gravitational_constant = 0.0000000000674;

double manager::scale = 12.;
vec2d manager::offset;

orbital* manager::make_new(const orbital& orbit)
{
    orbital* o = new orbital(orbit);

    olist.push_back(o);

    return o;
}

void manager::destroy(orbital* o)
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

void manager::tick(float dt_cur, float dt_old)
{
    int osize = olist.size();

    for(int i=0; i<osize; i++)
    {
        orbital* o1 = olist[i];

        for(int j=i+1; j<osize; j++)
        {
            orbital* o2 = olist[j];

            ///0.0000000000674

            vec2d r12 = (o2->pos - o1->pos);

            double r12l = r12.length();

            vec2d r12N = r12 / max(r12l, 1.);

            const double rad = 69911 * pow(10, 3);

            ///???
            ///radius of jupiter
            if(r12l < rad)
                r12l = rad;

            double NFac = gravitational_constant / (r12l * r12l);

            ///so its own mass cancels out in the change in velocity calvulations

            ///we can factor out the multiplication by the gravitational constant
            ///and put that below
            ///probably wont make all too much difference
            ///might improve accuracy a bit
            vec2d aF12 = ( o2->mass * NFac) * r12N;
            vec2d bF12 = (-o1->mass * NFac) * r12N;

            o1->acc += (aF12);
            o2->acc += (bF12);
        }
    }

    double dtf = dt_cur / dt_old;
    double dta = dt_cur * (dt_cur + dt_old)/2.;

    for(auto& i : olist)
    {
        vec2d o_pos = i->pos;

        //i->pos = i->pos + (i->pos - i->old_pos) * (dt_cur / dt_old) + i->acc * dt_cur * dt_cur + i->unconditional_acc;
        i->pos = i->pos + (i->pos - i->old_pos) * dtf + i->acc * dta + i->unconditional_acc;

        i->old_pos = o_pos;

        i->acc = {0,0};

        i->unconditional_acc = {0,0};
    }
}

///if a < radius 2, lower mass attached to higher mass
///I think there might be insufficient sampling when going really fast
///ie through a planet
///which is what is leading to these drastic accelerations
///could clamp to some reasonable orbital distance, but even then
///tick needs to return a new dt
///we need to improve absorption
///id like to be able to dump asteroids into earth

///if we gradually increase the dt from dt_s -> dt_s * 2, we can remove the inaccuracy
///while drastically speed up computation

///accuracy when we hit the center of something is less important in the tick_only_probe function

void manager::tick_only_probes(float dt_cur, float dt_old, const std::vector<orbital*>& probes, bool absorption)
{
    int osize = olist.size();

    for(int i=0; i<osize; i++)
    {
        orbital* o1 = olist[i];

        int psize = probes.size();

        for(int j=0; j<psize; j++)
        {
            orbital* o2 = probes[j];

            if(o2->skip)
                continue;

            vec2d r12 = (o2->pos - o1->pos);

            double r12l = r12.length();

            vec2d r12N = r12 / max(r12l, 1.);

            ///???
            ///radius of jupiter

            if(!absorption)
            {
                if(r12l < 69911 * pow(10, 3))
                    r12l = 69911 * pow(10, 3);
            }
            else
            {
                /*if(r12l < o1->radius * 4.)
                {
                    o2->skip = true;

                    continue;
                }*/

                vec2d line_to_hit = point2line_shortest(o2->pos, o2->pos - o2->old_pos, o1->pos);

                vec2d line_point = line_to_hit + o1->pos;

                double basis_length = (o2->pos - o2->old_pos).length();

                bool valid_skip_candidate = false;

                ///deliberately lax condition
                if((line_point - o2->pos).length() < basis_length*4. || (line_point - o2->old_pos).length() < basis_length*4.)
                {
                    valid_skip_candidate = true;
                }

                ///need to limit to perp
                if(line_to_hit.squared_length() < o1->radius*o1->radius * 64 && valid_skip_candidate)
                {
                    o2->skip = true;
                    continue;
                }
            }

            ///eliminating the norm/min/etc would be a lot faster
            double NFac = gravitational_constant / (r12l * r12l);

            ///so its own mass cancels out in the change in velocity calvulations

            vec2d bF12 = (-o1->mass * NFac) * r12N;

            o2->acc += (bF12);
        }
    }

    double dtf = dt_cur / dt_old;
    double dta = dt_cur * (dt_cur + dt_old)/2.;

    for(auto& i : probes)
    {
        if(i->skip)
            continue;

        vec2d o_pos = i->pos;

        i->pos = i->pos + (i->pos - i->old_pos) * dtf + i->acc * dta + i->unconditional_acc;

        i->old_pos = o_pos;

        i->acc = {0,0};

        i->unconditional_acc = {0,0};
    }
}

void manager::draw(vec2d _pos, vec3f _col, sf::RenderTarget& win, float r)
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

void manager::draw(orbital& o, sf::RenderTarget& win, float r)
{
    vec2d rad_pos = o.pos + (vec2d){o.radius, 0.f};

    vec2d s_rad_pos = pos2screen(rad_pos);

    sf::CircleShape shape;
    //shape.setSize({r, r});
    shape.setRadius(r);

    vec2d pos = pos2screen(o.pos); ///- projection

    vec2f epos = {(float)pos.v[0] + win.getSize().x/2, (float)pos.v[1] + win.getSize().y/2};

    if(epos.v[0] < 0 || epos.v[0] >= win.getSize().x || epos.v[1] < 0 || epos.v[1] >= win.getSize().y)
        return;

    float real_rad = fabs(s_rad_pos.v[0] - pos.v[0]);

    if(real_rad > r)
        shape.setRadius(real_rad);

    shape.setOrigin({shape.getRadius(), shape.getRadius()});

    shape.setPosition({epos.v[0], epos.v[1]});

    vec3f col = o.transitory_draw_col * 255.f;

    shape.setFillColor({col.v[0], col.v[1], col.v[2]});

    win.draw(shape);

    o.transitory_draw_col = o.col;
}

void manager::draw_bulk(const std::vector<orbital*>& orbitals, sf::RenderTarget& win, float r)
{
    sf::CircleShape shape;

    shape.setRadius(r);
    shape.setOrigin({shape.getRadius(), shape.getRadius()});
    shape.setFillColor({255, 255, 255});
    shape.setPointCount(6);

    vec3f last_col = {0,0,0};

    for(auto& o : orbitals)
    {
        vec2d pos = pos2screen(o->pos);

        vec2f epos = {(float)pos.v[0] + win.getSize().x/2, (float)pos.v[1] + win.getSize().y/2};

        if(epos.v[0] < 0 || epos.v[0] >= win.getSize().x || epos.v[1] < 0 || epos.v[1] >= win.getSize().y)
            continue;

        if(any_nan(epos))
            continue;

        if(o->skip)
            continue;

        vec3f col = o->transitory_draw_col * 255.f;

        if(col.v[0] != last_col.v[0] || col.v[1] != last_col.v[1] || col.v[2] != last_col.v[2])
            shape.setFillColor({col.v[0], col.v[1], col.v[2]});

        shape.setPosition({epos.v[0], epos.v[1]});

        win.draw(shape);

        o->transitory_draw_col = o->col;
    }
}

void draw_bulk_complex(manager& orbital_manager, sf::RenderTarget& win, float r = 2)
{
    sf::CircleShape shape;

    shape.setOrigin({shape.getRadius(), shape.getRadius()});

    shape.setFillColor({255, 255, 255});

    for(auto& o : orbital_manager.olist)
    {
        vec2d pos = orbital_manager.pos2screen(o->pos);

        vec2f epos = {(float)pos.v[0] + win.getSize().x/2, (float)pos.v[1] + win.getSize().y/2};

        if(epos.v[0] < 0 || epos.v[0] >= win.getSize().x || epos.v[1] < 0 || epos.v[1] >= win.getSize().y)
            continue;

        if(any_nan(epos))
            continue;

        vec2d rad_pos = o->pos + (vec2d){o->radius, 0.f};

        vec2d s_rad_pos = orbital_manager.pos2screen(rad_pos);

        float real_rad = fabs(s_rad_pos.v[0] - pos.v[0]);

        if(real_rad > r)
            shape.setRadius(real_rad);
        else
            shape.setRadius(r);

        shape.setOrigin({shape.getRadius(), shape.getRadius()});

        vec3f col = o->transitory_draw_col * 255.f;

        shape.setFillColor({col.v[0], col.v[1], col.v[2]});

        shape.setPosition({epos.v[0], epos.v[1]});

        win.draw(shape);

        o->transitory_draw_col = o->col;
    }
}

void manager::display(sf::RenderTarget& win, float r)
{
    draw_bulk_complex(*this, win, r);

    ///just assume
    /*for(auto& i : olist)
    {
        //draw(i->pos, i->col, win, r);
        draw(*i, win, r);
    }*/
}

vector<vector<vec2d>> manager::test(int ticks, float dt_cur, float dt_old, sf::RenderWindow* tex, bool render, orbital* test_orbital, std::vector<orbital*> info_to_retrieve)
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
        if(i == 0)
            tick(dt_cur, dt_old);
        else
            tick(dt_cur, dt_cur);

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

            //printf("%f %f\n", my_test->pos.v[0], my_test->pos.v[1]);
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

    if(!test_orbital)
    {
        test_ret.erase(test_ret.begin());
    }

    for(int i=0; i<olist.size(); i++)
    {
        *olist[i] = old[i];
    }

    return test_ret;
}

///return min dist and tick instead
void manager::test_with_cache(int ticks, float dt_cur, float dt_old, double target_distance, int& min_tick, double& min_dist, orbital* test_orbital, orbital* target_orbital, const std::vector<orbital*>& to_insert_into_stream, std::vector<std::vector<vec2d>>& cache)
{
    const bool weight = true;

    std::vector<orbital*> to_insert = to_insert_into_stream;

    to_insert.push_back(test_orbital);

    //const float dt_max = dt_cur;
    //const float dt_min = dt_cur;

    int tick_max_size = cache.size();

    int mtick = -1;
    double mdist = DBL_MAX;

    ///we want to simulate
    ///tick
    ///tick_only_probes

    for(int i=0; i<ticks-1; i++)
    {
        ///do a tick here
        ///so cache[0] is before tick
        ///cache[1] is after tick
        for(int j=0; j<olist.size(); j++)
        {
            orbital* o1 = olist[j];

            ///dont need to access cache for both of these
            o1->old_pos = cache[i][j];
            o1->pos = cache[i+1][j];
        }

        tick_only_probes(dt_cur, dt_old, to_insert);

        double len = (test_orbital->pos - target_orbital->pos).length();

        if(fabs(len - target_distance) < mdist)
        {
            mdist = fabs(len - target_distance);
            mtick = i;
        }

        dt_old = dt_cur;
        //dt_cur = dt_min + (dt_max - dt_min) * (float)i / tick_max_size;
    }

    ///we're forgetting to reset pos and old_pos
    ///that is the issue

    min_tick = mtick;
    min_dist = mdist;
}

int manager::get_minimum_distance(int o1, int o2, const vector<vector<vec2d>>& pos)
{
    int min_tick = -1;

    double min_len = DBL_MAX;

    int len = pos[o1].size();

    ///requires a whole pass over the data!!! For something we're already calculating!!!
    for(int tnum=0; tnum < len; tnum++)
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


ret_info manager::bisect(int ticks, float dt_cur, float dt_old,
                         float base_speed, float minimum, float maximum,
                         float angle_offset, float half_angle_cone, float angle_subdivisions,
                         int num_per_step, int depth, orbital* test_orbital, orbital* target_orbital, std::vector<orbital*> info_to_retrieve, int c)
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

        which_id = 1;
    }


    float next_angle_offset = angle_offset;
    float next_half_angle = half_angle_cone;
    const float next_angle_subdivisions = angle_subdivisions;

    double step = (maximum - minimum) / num_per_step;

    double min_min = DBL_MAX;
    int saved_mtick = -1;
    double found_speed = 1;
    double found_mod = minimum;

    vector<vector<vec2d>> backup;

    for(int j=0; j<(int)angle_subdivisions; j++)
    {
        float frac = (float)j/angle_subdivisions;

        frac -= 0.5f;

        frac *= 2.f;

        float angle_offset_offset = half_angle_cone * frac;

        float real_offset = angle_offset_offset + angle_offset;

        for(int i=0; i<num_per_step; i++)
        {
            orbital probe = *test_orbital;
            probe.unconditional_acc = {0,0};

            float ifrac = (float)i / num_per_step;

            double cur = (maximum - minimum) * ifrac + minimum;

            //double speed = base_speed / cur;

            double speed = base_speed * cur;

            //if(fabs(cur) < 0.000001)
            //    speed = 0;


            probe.accelerate_relative_to_velocity(speed, real_offset, 1200);

            auto last_experiment = this->test(ticks, dt_cur, dt_old, nullptr, false, &probe, info_to_retrieve);

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
                next_angle_offset = real_offset;
            }
        }
    }

    next_half_angle = next_half_angle / angle_subdivisions;

    ///wtf, found mod is a dividor, not a multiplier
    ///higher mod = slower speed
    ///?????
    printf("found_acc %f\n", found_mod);
    printf("found mindist %f\n", min_min / 1000 / 1000 / 1000);
    printf("found angle %f\n", next_angle_offset);

    if(c+1 < depth)
    {
        return bisect(ticks, dt_cur, dt_old, base_speed, found_mod - step/2, found_mod + step/2, next_angle_offset, next_half_angle, next_angle_subdivisions, num_per_step, depth, test_orbital, target_orbital, info_to_retrieve, c+1);
    }

    return {backup, saved_mtick, min_min};
}

///we need a -> clone
std::vector<std::vector<vec2d>> get_object_cache(manager& orbital_manager, int tick_num, float dt_cur, float dt_old)
{
    std::vector<orbital> old_orbitals;

    for(auto& i : orbital_manager.olist)
    {
        old_orbitals.push_back(*i);
    }

    std::vector<std::vector<vec2d>> object_cache;

    object_cache.resize(tick_num);

    for(auto& i : object_cache)
    {
        i.reserve(orbital_manager.olist.size());
    }

    //const float dt_max = dt_cur;
    //const float dt_min = dt_cur;


    for(int i=0; i<tick_num; i++)
    {
        std::vector<vec2d>& this_tick_cache = object_cache[i];

        const std::vector<orbital*>& cur_orbitals = orbital_manager.olist;

        for(auto& j : cur_orbitals)
        {
            this_tick_cache.push_back(j->pos);
        }

        orbital_manager.tick(dt_cur, dt_old);

        dt_old = dt_cur;
        //dt_cur = dt_min + (dt_max - dt_min) * (float)i / tick_num;
    }

    for(int i=0; i<old_orbitals.size(); i++)
    {
        *(orbital_manager.olist[i]) = old_orbitals[i];
    }

    return object_cache;
}

///we dont need any info to retrieve
///maybe I should start with a really low tick, and allow it to grow up to _max
///if we enable probe absorption, itll probably fix a few issues
ret_info manager::bisect_with_cache(int ticks, float dt_cur, float dt_old,
                         float base_speed, float minimum, float maximum,
                         float angle_offset, float half_angle_cone, float angle_subdivisions,
                         int num_per_step, int depth, double target_distance, double max_error_distance, orbital* test_orbital, orbital* target_orbital,
                         std::vector<orbital*> info_to_retrieve, int c,
                         const std::vector<std::vector<vec2d>>& passed_cache,
                         int last_found_minimum_tick, int last_last_found_minimum_tick)
{
    int which_id = -1;

    ///need to fold target_orbital into olist if it doesn't already exist there
    ///target orbital needs to be manually reset if this is the case
    ///with the advent of save/load/etc, we can no longer safely pass in earth sun moon
    ///without at least redefining them first

    ///add target orbital if its not found
    if(which_id == -1)
    {
        info_to_retrieve.insert(info_to_retrieve.begin(), target_orbital);

        which_id = 1;
    }

    std::vector<std::vector<vec2d>> cache = passed_cache;

    if(c == 0)
        cache = get_object_cache(*this, ticks, dt_cur, dt_old);

    int this_ticks = ticks;

    int tick_reduction_start = 1;

    bool short_path_opt = last_found_minimum_tick < 0.1f * ticks;

    ///do better tick reduction based on past, itll probably be more accurate
    if(c >= tick_reduction_start)
    {
        int tick_differential = last_found_minimum_tick - last_last_found_minimum_tick;

        int atick_diff = fabs(tick_differential);

        //printf("T1 %i\n", atick_diff);

        int sampled_minimum = last_found_minimum_tick;

        float extension_max_frac = 0.2f;
        float extension_minimum_frac = 0.05f;

        float current_frac = (float)(c - tick_reduction_start) / (depth - tick_reduction_start);

        ///start at 1, go to 0
        current_frac = 1.f - current_frac;

        ///extension_max_frac -> 0
        float extension_frac = (extension_max_frac - extension_minimum_frac) * current_frac + extension_minimum_frac;

        if(c == 1 && !short_path_opt)
            extension_frac = 0.5f;

        int tick_extension = ticks * extension_frac;

        int tick_differential_extension = last_found_minimum_tick + atick_diff * 8 + tick_extension * 0.01 + 50;// * extension_frac;

        this_ticks = last_found_minimum_tick + tick_extension;

        //printf("%i found tick, allowed max tick %i, tick_differential %i\n", last_found_minimum_tick, last_found_minimum_tick + tick_extension, tick_differential_extension);

        this_ticks = min(this_ticks, tick_differential_extension);

        this_ticks = min(this_ticks, ticks-1);
    }

    bool is_target_part_of_mainstream_list = false;

    for(auto& i : olist)
    {
        if(i == target_orbital)
        {
            is_target_part_of_mainstream_list = true;
            break;
        }
    }

    const float distance_weight_at_max_time = 1.1;

    std::vector<orbital*> stream_add;

    if(!is_target_part_of_mainstream_list)
        stream_add.push_back(target_orbital);

    std::vector<orbital> orbital_backup = make_backup();

    orbital target_backup = *target_orbital;

    float next_angle_offset = angle_offset;
    float next_half_angle = half_angle_cone;
    const float next_angle_subdivisions = angle_subdivisions;

    double step = (maximum - minimum) / num_per_step;

    double min_min = DBL_MAX;
    int saved_mtick = -1;
    double found_speed = 1;
    double found_mod = minimum;

    vector<vector<vec2d>> backup;

    sf::Clock c0clock;

    ///do intra stage tick reduction for c == 0 && c == 1
    ///I suspect these are a big reason for slowness
    for(int j=0; j<(int)angle_subdivisions; j++)
    {
        float frac = (float)j/angle_subdivisions;

        frac -= 0.5f;

        frac *= 2.f;

        float angle_offset_offset = half_angle_cone * frac;

        float real_offset = angle_offset_offset + angle_offset;

        for(int i=0; i<num_per_step; i++)
        {
            orbital probe = *test_orbital;
            probe.unconditional_acc = {0,0};

            float ifrac = (float)i / num_per_step;

            double cur = (maximum - minimum) * ifrac + minimum;

            double speed = base_speed * cur;

            probe.accelerate_relative_to_velocity(speed, real_offset, 1200);

            //double target_distance = 10. * 1000 * 1000 * 1000;

            int mtick;
            double mdist;

            this->test_with_cache(this_ticks, dt_cur, dt_old, target_distance, mtick, mdist, &probe, target_orbital, stream_add, cache);
            //auto last_experiment = this->test_with_cache(this_ticks, dt_cur, dt_old, &probe, stream_add, cache, info_to_retrieve);

            restore_from_backup(orbital_backup);

            if(!is_target_part_of_mainstream_list)
                *target_orbital = target_backup;

            ///give a minor weighting to time to pick a shorter journey
            float weighted_distance = mdist + (distance_weight_at_max_time - 1.f) * mdist * (float)mtick / ticks;

            ///eliminate this call
            //int mtick = this->get_minimum_distance(0, which_id, last_experiment);


            /*vec2d min_voyager = last_experiment[0][mtick];
            vec2d min_mars = last_experiment[which_id][mtick];

            if((min_mars - min_voyager).length() < target_orbital->radius)
            {
                printf("HIT\n");
            }*/

            //if(mdist < target)

            //if((min_mars - min_voyager).length() < min_min)
            //if((mdist - target_distance) < min_min)
            if(weighted_distance < min_min)
            {
                min_min = weighted_distance;
                //min_min = (min_mars - min_voyager).length();
                //f_acc = cur;
                //backup = last_experiment;
                saved_mtick = mtick;
                found_speed = speed;
                found_mod = cur;
                next_angle_offset = real_offset;
            }
        }
    }

    ///c0 and c1 comprise a huge amount of clock time
    ///intra stage opt
    ///more aggressive downticking from c0 -> c1?
    ///we also need terminate early if x ticks fail to optimise (probably 3, with <5% mindist deviation)
    //if(c <= 1)
    {
        printf("C0clk %f\n", c0clock.getElapsedTime().asMicroseconds()/1000.f);
    }

    next_half_angle = next_half_angle / angle_subdivisions;

    ///wtf, found mod is a dividor, not a multiplier
    ///higher mod = slower speed
    ///?????
    //printf("found_acc %f\n", found_mod);
    printf("found mindist %f\n", min_min / 1000 / 1000 / 1000);
    //printf("found angle %f\n", next_angle_offset);

    if(c+1 < depth && min_min >= max_error_distance)
    {
        return bisect_with_cache(ticks, dt_cur, dt_old, base_speed, found_mod - step/2, found_mod + step/2, next_angle_offset, next_half_angle, next_angle_subdivisions, num_per_step, depth, target_distance, max_error_distance, test_orbital, target_orbital, info_to_retrieve, c+1, cache, saved_mtick, last_found_minimum_tick);
    }

    test_orbital->accelerate_relative_to_velocity(found_speed, next_angle_offset, 1200);

    return {backup, saved_mtick, min_min};
}

void manager::plot(const vector<vector<vec2d>>& elements, int which_element, int which_tick, sf::RenderWindow& win, vec3f col)
{
    if(which_tick >= elements[which_element].size())
        return;

    vec2d pos = elements[which_element][which_tick];

    draw(pos, col, win, 4);
}

void manager::plot_all(const vector<vector<vec2d>>& elements, int which_tick, sf::RenderWindow& win, vector<vec3f> cols)
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

double get_min_dist(std::vector<orbital*>& orbs, orbital* test_orbital)
{
    double min_dist = DBL_MAX;

    for(auto& i : orbs)
    {
        if(i == test_orbital)
            continue;

        double len = (i->pos - test_orbital->pos).length();

        if(len < min_dist)
            min_dist = len;
    }

    return min_dist;
}

bool manager::contains(orbital* o)
{
    for(auto& i : olist)
    {
        if(i == o)
            return true;
    }

    return false;
}

///gunna have to progressively refine over several frames
vector<vec2d> manager::test_with_adaptive_tick(int ticks, float dt_max, float dt_min, float dt_old, orbital* test_orbital)
{
    double earth_sun_distance = 149.6 * pow(10, 9);

    std::vector<orbital> old;

    for(auto& i : olist)
    {
        old.push_back(*i);
    }

    bool in_mainstream_tick = contains(test_orbital);

    orbital backup = *test_orbital;

    std::vector<vec2d> test_ret;

    float dt_current = dt_min;
    float dt_last = dt_old;

    for(int i=0; i<ticks; i++)
    {
        tick(dt_current, dt_last);

        if(!in_mainstream_tick)
        {
            tick_only_probes(dt_current, dt_last, {test_orbital}, false);
        }

        test_ret.push_back(test_orbital->pos);

        dt_last = dt_current;

        double cmin = get_min_dist(olist, test_orbital);

        ///need to refine on velocity too

        double frac = cmin / (earth_sun_distance / 2);

        frac = max(frac, 0.2);

        frac *= 5.f;

        //if(frac > 1)
        //    frac *= frac;

        double next_dt = frac * dt_min;

        if(next_dt > dt_max)
            next_dt = dt_max;

        dt_current = next_dt;

        ///this is the skip for above
        dt_current = dt_min;

    }

    for(int i=0; i<olist.size(); i++)
    {
        *olist[i] = old[i];
    }

    *test_orbital = backup;

    return test_ret;
}


///so, we definitely need to use an adaptive timestep when testing
void manager::plot_orbit(orbital* o, int ticks, sf::RenderWindow& tex)
{
    //vector<vector<vec2d>> test_res = test(ticks, dt_s * 2, dt_s, nullptr, false, nullptr, {o});

    //printf("ns %i %i\n", test_res.size(), test_res.front().size());

    vector<vec2d> test_res = test_with_adaptive_tick(ticks, dt_s*100, dt_s, dt_s, o);

    vec2d last_pos = {-10, -10};

    for(auto& i : test_res)
    {
        if((i - last_pos).length() >= 1)
        {
            draw(i, o->col, tex, 1);

            last_pos = i;
        }
    }
}

orbital* manager::get_nearest(const std::vector<orbital*>& orbitals, vec2d mouse_screen_pos, vec2d screen_dim)
{
    vec2d world_pos = screen2pos(mouse_screen_pos, screen_dim/2.);

    double min_dist = DBL_MAX;
    orbital* ret = nullptr;

    for(auto& i : orbitals)
    {
        if(!i)
            continue;

        if(i->skip)
            continue;

        double ndist = (i->pos - world_pos).length();

        if(ndist < min_dist)
        {
            min_dist = ndist;
            ret = i;
        }
    }

    return ret;
}

void manager::restore_from_backup(const std::vector<orbital>& backup)
{
    if(backup.size() != olist.size())
    {
        //throw std::runtime_error("uuh restore from backup");
    }

    for(int i=0; i<backup.size(); i++)
    {
        *olist[i] = backup[i];
    }
}

std::vector<orbital> manager::make_backup()
{
    std::vector<orbital> backup;

    backup.reserve(olist.size());

    for(auto& i : olist)
    {
        backup.push_back(*i);
    }

    return backup;
}

///hmm. We'll also need to own the probes and stuff
///maybe we
manager* manager::clone()
{
    manager* nmanager = new manager(*this);

    nmanager->olist.clear();

    for(auto& i : olist)
    {
        nmanager->make_new(*i);
    }

    return nmanager;
}

/*void manager::set_unimportant_planet_skip(const std::vector<int>& skippable_ids)
{
    can_skip_for_optimisation = skippable_ids;
}*/

/*
///eh, will break if any major orbitals are set to skip
///we need to take into account proximity!
///we should really not use a skip system, this is dumb
///remove them from population, use backups to restore
///cant, breaks ptrs
///generate valid list, use that
///maybe have dynamic skip list? that can't work

///this is a dud optimisation, we'd need to know what comes near the path
///impossible
///without degrading the quality of the simulation more than I'd like
void manager::skip_skippable(orbital* base, orbital* except)
{
    bool skipc = except->skip;

    for(auto& i : can_skip_for_optimisation)
    {
        if(i < olist.size() && i >= 0)
        {
            double len = (base->pos - olist[i]->pos).length();

            if(len < earth_sun_dist * 3.)
                continue;

            olist[i]->skip = true;
        }
        else
        {
            printf("Warning, invalid skip %i\n", i);
        }
    }

    except->skip = skipc;
}

void manager::unskip_skippable()
{
    for(auto& i : can_skip_for_optimisation)
    {
        if(i < olist.size() && i >= 0)
        {
            olist[i]->skip = false;
        }
        else
        {
            printf("Warning, invalid skip %i\n", i);
        }
    }
}
*/
