#include <iostream>
#include <algorithm>
#include "SDL_GUI.hpp"
#include "Theme.hpp"

int main()
{
	auto root = std::make_shared<Window>();
	auto d = std::make_shared<Dropdown>(root, "placeholder", std::vector<std::string> {"1", "2", "3"});
	d->grid(0, 0);

	auto e = std::make_shared<Entry>(root);
	e->grid(2, 0);

	std::make_shared<Button>(root, "set to 1", [d]() {
		d->set(0);
	})->grid(0, 1);

	auto t = std::make_shared<Textbox>(root, 10, 20);
	t->grid(1, 1);

	std::make_shared<Button>(root, "Display", [t]() {
		std::cout << t->get() << std::endl;
	})->grid(1, 0);

	std::make_shared<Button>(root, "Display", [e]() {
		std::cout << e->get() << std::endl;
	})->grid(2, 1);

	root->run();
}