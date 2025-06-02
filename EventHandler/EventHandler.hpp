#pragma once
#include <map>
#include "Window.hpp"
class EventHandler {
public:
	void Poll(float dt);
	static void register_window(Window* window) {
		get().windows[window->sdl_window_id] = window;
	}
	static EventHandler& get() {
		static EventHandler instance;
		return instance;
	}
	static unsigned int get_mouse_x() {
		return get().mouse_x;
	}
	static unsigned int get_mouse_y() {
		return get().mouse_y;
	}
	static void remove_window(int id) {
		get().windows.erase(id);
	}

	static void quit() {
		get()._quit();
	}

private:
	void _quit();
	std::map<int, Window*> windows;
	int focused_window_id = 1;
	int mouse_focused_window_id = 1;
	int mouse_x, mouse_y;
	bool running;
	std::shared_ptr<Widget> current_hover = nullptr;
	std::shared_ptr<Widget> selected_widget = nullptr;
};