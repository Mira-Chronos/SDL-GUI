#include "Window.hpp"
#include "EventHandler.hpp"
#include "Dropdown.hpp"

Dropdown::Dropdown(std::shared_ptr<Parent> parent, std::string _placeholder, std::vector<std::string> _options)
:Widget(parent), placeholder(_placeholder), options(_options) {
    w = placeholder.length();
    if (w == 0)
        w = window->get_font_width();
    for (std::string &str : options) {
        if (str.length() > w)
            w = str.length();
    }
    w *= window->get_font_width();
    w += text_triangle_gap + triangle_size;
    h = window->get_font_height();
    dropdown_height = window->get_font_height() * (options.size() + 1);
    window->click_event_callbacks.push_back([this](){return handle_global_press();});
}

std::string Dropdown::get() {
    if (selected != nullptr)
        return *selected;
    std::string temp;
    return temp;
}

void Dropdown::set(unsigned int index) {
    if (options.empty())
        return;
    if (index >= options.size()) {
        index = options.size() - 1;
    }
    selected = &options[index];
}

void Dropdown::update_and_render(float dt) {
    if (open) {
        // draw the open box
        window->top_level.push_back([this](){
            window->draw_rect(x, y, w, dropdown_height, border);
            window->draw_rect(x+1, y+1, w-2, dropdown_height-2, background, true);
            window->draw_line(x+w-triangle_size-triangle_edge_gap, y+h*0.75f, x+w-triangle_size/2-triangle_edge_gap, y+h/2, {255, 255, 255});
            window->draw_line(x+w-triangle_size/2-triangle_edge_gap, y+h/2, x+w-triangle_edge_gap, y+h*0.75f, {255, 255, 255});
            
            int mouse_y_rel = EventHandler::get_mouse_y() - y; // relative mouse x
            int item = mouse_y_rel / window->get_font_height();
            if (item > 0 && item <= options.size()) // draw a coloured rect behind the hovered item
                window->draw_rect(x, y+item*window->get_font_height(), w, window->get_font_height(), hover_background, true);
            
            window->draw_text(x, y, placeholder.c_str(), foreground);
            int i = 1;
            for (std::string &str : options) {
                window->draw_text(x, y + i*window->get_font_height(), str.c_str(), foreground);
                i++;
            }
        });
    } else {
        window->draw_rect(x, y, w, h, border);
        window->draw_rect(x+1, y+1, w-2, h-2, background, true);
        window->draw_line(x+w-triangle_size-triangle_edge_gap, y+h/2, x+w-triangle_size/2-triangle_edge_gap, y+h*0.75f, {255, 255, 255});
        window->draw_line(x+w-triangle_size/2-triangle_edge_gap, y+h*0.75f, x+w-triangle_edge_gap, y+h/2, {255, 255, 255});
        // just draw the selected or the placeholder
        // this is ugly
        if (selected == nullptr) {
            window->draw_text(x, y, placeholder.c_str(), foreground);
        } else {
            window->draw_text(x, y, selected->c_str(), foreground);
        }
    }
    Widget::update_and_render(dt);
}

void Dropdown::on_press() {
    open = !open;
}

int Dropdown::handle_global_press() {

    if (open) {
        if (EventHandler::get_mouse_x() >= x
        && EventHandler::get_mouse_x() <=x+w
        && EventHandler::get_mouse_y() >= y
        && EventHandler::get_mouse_y() <= y+dropdown_height) {
            // clicked on an item in the dropdown
            int mouse_y_rel = EventHandler::get_mouse_y() - y; // relative mouse x
            int item = mouse_y_rel / window->get_font_height();
            if (item > 0) { // placeholder = 0
                item--; // adjust to remove the placeholder
                selected = &options[item];
            }
        }
        on_press();
        return 1; // do not pass the click event onto other widgets
    }
    return 0;
}

void Dropdown::on_deselect() {
    open = false;
}

void Dropdown::on_wheel_down() {
    if (options.empty()) return;
    int idx = 0;
    if (selected != nullptr) {
        idx = std::distance(options.data(), selected);
    }
    if (idx < static_cast<int>(options.size()) - 1) {
        selected = &options[idx + 1];
    }
}

void Dropdown::on_wheel_up() {
    if (options.empty()) return;
    int idx = 0;
    if (selected != nullptr) {
        idx = std::distance(options.data(), selected);
    }
    if (idx > 0) {
        selected = &options[idx - 1];
    }
}