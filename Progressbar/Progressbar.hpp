#pragma once

#include "Widget.hpp"

class Progressbar : public Widget {
public:
	Progressbar(std::shared_ptr<Parent> parent, int max);
	Progressbar(std::shared_ptr<Parent> parent, int min, int max);
	void update_and_render(float dt);
private:
};
