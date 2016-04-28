#include "ui.hpp"

sf::Font font;

void check_load_font()
{
    static bool loaded_font = false;

    if(!loaded_font)
    {
        font.loadFromFile("VeraMono.ttf");
        loaded_font = true;

        printf("loaded font\n");
    }
}

vec2f ui_element::get_local_pos()
{
    return relative_pos;
}

vec2f ui_element::get_global_pos()
{
    if(!parent)
        return relative_pos;

    return parent->get_global_pos() + relative_pos;
}

void ui_element::set_relative_pos(vec2f pos)
{
    relative_pos = pos;
}

void ui_element::set_parent(ui_element* par)
{
    parent = par;
}

void ui_element::draw(sf::RenderWindow& win)
{
    col = clamp(col, (vec4f){0,0,0,0}, (vec4f){1,1,1,1});

    check_load_font();

    vec2f global_pos = get_global_pos();

    sf::Color sfcol(col.v[0] * 255, col.v[1] * 255, col.v[2] * 255, col.v[3] * 255);

    if(type == ui::TEXT)
    {
        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(font_size);
        txt.setColor(sfcol);
        txt.setString(text.c_str());

        txt.setPosition({global_pos.v[0], global_pos.v[1]});

        win.draw(txt);

        //printf("hi %f %f %s\n", global_pos.v[0], global_pos.v[1], text.c_str());
    }

    if(type == ui::CIRCLE)
    {
        sf::CircleShape circle;
        circle.setRadius(rad);
        circle.setFillColor(sfcol);

        float alpha = col.v[3];

        float edge_alpha = alpha * 1.5;

        edge_alpha = std::min(edge_alpha, 1.f);

        circle.setOutlineColor(sf::Color(col.v[0] * 255, col.v[1] * 255, col.v[2] * 255, edge_alpha * 255));

        circle.setOutlineThickness(3);

        circle.setPosition({global_pos.v[0], global_pos.v[1]});

        win.draw(circle);
    }

    if(type == ui::LINESNAP_45)
    {

    }

    if(type == ui::BOX)
    {

    }
}

ui_element::ui_element(ui_type _type)
{
    type = _type;
}
