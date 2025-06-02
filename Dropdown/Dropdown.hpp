#pragma once
#include "Widget.hpp"
class Dropdown : public Widget {

public:
	Dropdown(std::shared_ptr<Parent> parent, std::string placeholder, std::vector<std::string> options);
	SDL_Color foreground = Theme::DROPDOWN_FOREGROUND;
	SDL_Color background = Theme::DROPDOWN_BACKGROUND;
	SDL_Color border = Theme::DROPDOWN_BORDER;
	SDL_Color hover_background = Theme::DROPDOWN_HOVER_BACKGROUND;
	std::string get();
	void set(unsigned int index);
	void update_and_render(float dt);
	void on_press();
	void on_deselect();
	void on_wheel_down() override;
	void on_wheel_up() override;
private:
	unsigned int dropdown_height;
	int text_triangle_gap = 20;
	int triangle_size = 10;
	int triangle_edge_gap = 10;
	bool open = false;
	std::string *selected = nullptr;
	std::vector<std::string> options;
	std::string placeholder;

	int handle_global_press();
};