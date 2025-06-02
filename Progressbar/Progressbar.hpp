#pragma once

#include "Widget.hpp"

class Progressbar : public Widget {
public:
	Progressbar(std::shared_ptr<Parent> parent, int min, int max);
	void update_and_render(float dt);
    void set(int v) { value = std::max(min, std::min(v, max)); }
    int get() const { return value; }
	void on_press() override;
private:
    int min = 0;
    int max = 100;
    int value = 0;
};
