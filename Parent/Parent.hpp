#pragma once
#include <vector>
#include <memory>
#include "Widget.hpp"

class Widget;
class Window;

enum class Placement {GRID, PLACE};
class Parent {
public:
	virtual std::shared_ptr<Widget> get_target_widget(int x, int y) {
		return ((*this).*(this->get_fp))(x, y);   // make this the main call for the window.cpp event handler !!!
	}
	virtual void grid(std::shared_ptr<Widget> widget, unsigned int row, unsigned int column);
	virtual void place(std::shared_ptr<Widget> widget, unsigned int x, unsigned int y, bool center = false);
	virtual std::shared_ptr<Widget> on_hover_grid(int x, int y);
	virtual std::shared_ptr<Widget> on_hover_place(int x, int y);
	virtual void off_hover(int x, int y) {}
	virtual Window* get_root() {
		return nullptr;
	}
	//    void remove_widget(Widget *widget); // use linear search using the x y and grid

	void compute_grid_sizes();
	void apply_grid_layout();
protected:
	std::vector<int> row_heights = {};
	std::vector<int> column_widths = {};
	std::vector<std::vector<std::shared_ptr<Widget>>> objects = {}; // vector of widget vectors
	virtual void update_and_render(float dt);
	int m_x = 0;
	int m_y = 0;
	Placement placement;
	bool placement_chosen;
	std::shared_ptr<Widget> (Parent::*get_fp)(int, int);

};