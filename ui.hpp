#ifndef UI_HPP_INCLUDED
#define UI_HPP_INCLUDED

#include <vec/vec.hpp>
#include <string>

#include <SFML/Graphics.hpp>

namespace ui
{
    enum ui
    {
        NONE = 0,
        TEXT = 1,
        CIRCLE = 2,
        LINESNAP_45 = 3,
        BOX = 4
    };
}

typedef ui::ui ui_type;

///this should really be a tagged union, but that's a little too much work
struct ui_element
{
    ui_type type = ui::NONE;

    std::string text;
    int font_size = 12;
    float rad = 1;
    vec2f dim = {1, 1};
    bool centered = false;

    vec4f col = {1,1,1,1};

    ui_element* parent = nullptr;

    vec2f relative_pos = {0,0};

    vec2f calculated_absolute_pos = {0,0};

    void set_relative_pos(vec2f pos);
    void set_parent(ui_element* par);
    vec2f get_global_pos();
    vec2f get_local_pos();

    void draw(sf::RenderWindow& win);

    ui_element(ui_type _type);
};

struct ui_manager
{

};

#endif // UI_HPP_INCLUDED
