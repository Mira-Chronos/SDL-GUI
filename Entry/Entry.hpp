#pragma once
#include "Widget.hpp"
class Entry : public Widget {
public:
	Entry(std::shared_ptr<Parent> parent);
	void set_placeholder(std::string text);
	void set(std::string text);
	inline std::string get() {
		return contents;
	}
	void update_and_render(float dt);
	void on_hover();
	void off_hover();
	void on_select();
	void on_deselect();
	void on_press();
	void on_key_press(SDL_Scancode key);
	void on_text_input(char* text);
	void set_max_chars(unsigned int value);

private:
	int text_width = 1;
	int text_height = 1;
	unsigned int max_characters = -1;
	int characters = 0;
	std::string contents;
	std::string visible_text;
	std::string placeholder;
	unsigned int cursor_x = 2;
	unsigned int cursor_position = 0;
	bool typing = false;


	int scroll_right = 0;
	void move_cursor_right();
	void move_cursor_left();
	void calc_visible_text();
};