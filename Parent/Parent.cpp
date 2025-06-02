#include "Parent.hpp"
#include "EventHandler.hpp"
#include <assert.h>
#include <iostream>

std::shared_ptr<Widget> Parent::on_hover_grid(int x, int y)
{
	int temp_x = m_x;
	int temp_y = m_y;
	int row = 0;
	int column = 0;
	for (int height : row_heights) {
		if (y >= temp_y && y <= temp_y + height) {
			// found the row
			break;
		}
		temp_y += height;
		row++;
	}
	for (int width : column_widths) {
		if (x >= temp_x && x <= temp_x + width) {
			// found the row
			break;
		}
		temp_x += width;
		column++;
	}
	if (row < objects.size() && column < objects[row].size()) {
		if (x <= objects[row][column]->get_x() + objects[row][column]->get_w() &&
		        x >= objects[row][column]->get_x() &&
		        y <= objects[row][column]->get_y() + objects[row][column]->get_h() &&
		        y >= objects[row][column]->get_y()) {
			return objects[row][column]->get_target_widget(x, y);
		}
	}
	return nullptr;
}

std::shared_ptr<Widget> Parent::on_hover_place(int x, int y)
{
	for (int i = objects.back().size()-1; i >= 0; i--) {
		if (x <= objects.back()[i]->get_x() + objects.back()[i]->get_w() &&
		        x >= objects.back()[i]->get_x() &&
		        y <= objects.back()[i]->get_y() + objects.back()[i]->get_h() &&
		        y >= objects.back()[i]->get_y()) {
			return objects.back()[i]->get_target_widget(x, y);
		}
	}
	return nullptr;
}

void Parent::update_and_render(float dt)
{
	for (int i = 0; i < objects.size(); i++) {
		for (int j = 0; j < objects[i].size(); j++) {
			objects[i][j]->update_and_render(dt);
		}
	}

}

void Parent::place(std::shared_ptr<Widget> widget, unsigned int x, unsigned int y, bool center)
{
	placement = Placement::PLACE;
	get_fp = &Parent::on_hover_place;
	if(objects.empty()) {
		objects.push_back({});
	}
	if (center) {
		widget->set_x(x - widget->get_w() / 2);
		widget->set_y(y - widget->get_h() / 2);
	}
	else {
		widget->set_x(x);
		widget->set_y(y);
	}
	objects.back().push_back(widget);
}

void Parent::grid(std::shared_ptr<Widget> widget, unsigned int row, unsigned int column)
{
	placement = Placement::GRID;
	get_fp = &Parent::on_hover_grid;
	// Assure la taille de la grille
	while (objects.size() <= row) {
		objects.push_back({});
	}
	while (objects[row].size() <= column) {
		objects[row].push_back(nullptr);
	}
	objects[row][column] = widget;
	widget->set_row(row);
	widget->set_column(column);
}

void Parent::compute_grid_sizes()
{
	// Redimensionne les vecteurs
	row_heights.assign(objects.size(), 0);
	column_widths.assign(objects.empty() ? 0 : objects[0].size(), 0);

	for (size_t i = 0; i < objects.size(); ++i) {
		for (size_t j = 0; j < objects[i].size(); ++j) {
			std::shared_ptr<Widget> w = objects[i][j];
			if (w) {
				if (w->get_h() > row_heights[i]) row_heights[i] = w->get_h();
				if (w->get_w() > column_widths[j]) column_widths[j] = w->get_w();
			}
		}
	}
}

void Parent::apply_grid_layout()
{
	int y = m_y;
	for (size_t i = 0; i < objects.size(); ++i) {
		int x = m_x;
		for (size_t j = 0; j < objects[i].size(); ++j) {
			std::shared_ptr<Widget> w = objects[i][j];
			if (w) {
				w->set_x(x);
				w->set_y(y);
			}
			x += column_widths[j];
		}
		y += row_heights[i];
	}
}