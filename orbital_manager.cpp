#include "orbital_manager.hpp"

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
    for(int i=0; i<(int)olist.size(); i++)
    {
        orbital* o1 = olist[i];

        for(int j=i+1; j<(int)olist.size(); j++)
        {
            orbital* o2 = olist[j];

            ///0.0000000000674

            double G = 0.0000000000674;

            vec2d r12 = (o2->pos - o1->pos);

            double r12l = r12.length_d();

            ///???
            ///radius of jupiter
            if(r12l < 69911 * pow(10, 3))
                r12l = 69911 * pow(10, 3);


            vec2d F12 = - (o1->mass / r12l) * (o2->mass / r12l) * (r12.norm() * G);

            //vec2f df12 = conv<double, float, 2>(F12);

            vec2d df12 = F12;

            ///so its own mass cancels out in the change in velocity calvulations
            vec2d aF12 = -df12 / o1->mass;
            vec2d bF12 =  df12 / o2->mass;

            aF12 = aF12 * dt_cur * dt_cur;
            bF12 = bF12 * dt_cur * dt_cur;

            o1->acc += (aF12);
            o2->acc += (bF12);
        }
    }

    for(auto& i : olist)
    {
        vec2d o_pos = i->pos;

        i->pos = i->pos + (i->pos - i->old_pos) * (dt_cur / dt_old) + i->acc;

        i->old_pos = o_pos;

        i->acc = {0,0};
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

    vec3f col = o.col * 255.f;

    shape.setFillColor({col.v[0], col.v[1], col.v[2]});

    win.draw(shape);
}

void manager::display(sf::RenderTarget& win, float r)
{
    ///just assume
    for(auto& i : olist)
    {
        //draw(i->pos, i->col, win, r);
        draw(*i, win, r);
    }
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


int manager::get_minimum_distance(int o1, int o2, const vector<vector<vec2d>>& pos)
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


ret_info manager::bisect(int ticks, float dt_cur, float dt_old, float base_speed, float minimum, float maximum, int num_per_step, int depth, orbital* test_orbital, orbital* target_orbital, std::vector<orbital*> info_to_retrieve, int c)
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


        probe.accelerate_relative_to_velocity(speed, 0, 1200);


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
        }
    }

    printf("found_acc %f\n", found_mod);
    printf("found mindist %f\n", min_min / 1000 / 1000 / 1000);

    if(c+1 < depth)
    {
        return bisect(ticks, dt_cur, dt_old, base_speed, found_mod - step/2, found_mod + step/2, num_per_step, depth, test_orbital, target_orbital, info_to_retrieve, c+1);
    }

    return {backup, saved_mtick};
}

void manager::plot(const vector<vector<vec2d>>& elements, int which_element, int which_tick, sf::RenderWindow& win, vec3f col)
{
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
