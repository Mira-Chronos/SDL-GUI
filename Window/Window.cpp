#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <locale.h>
#include "EventHandler.hpp"
#include "Window.hpp"

#ifdef _WIN32
#define LOCALE ".UTF8"
#define W_LOCALE L".UTF8"
#else
#define LOCALE ""
#endif

#define GET_KEY(x) SDL_GetKeyName(SDL_GetKeyFromScancode(x))
int Window::window_count = 0;
int Window::windows_created = 0;

Window::Window()
{
	if(SDL_Init(SDL_INIT_VIDEO)!=0) {
		SDL_LogError(0, "Failed to initialize SDL_VIDEO: %s", SDL_GetError());
		return;
	}

	if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)) {
		SDL_LogError(0, "Failed to initialize SDL_IMG: %s", TTF_GetError());
		return;
	}
	if (TTF_Init()) {
		SDL_LogError(0, "Failed to initialize TTF: %s", SDL_GetError());
		return;
	}
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
	window = SDL_CreateWindow("SDL-GUI Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
	sdl_window_id = SDL_GetWindowID(window);
	internal_window_id = windows_created++;
	//windows[window_id] = this;
	if (window == nullptr) {
		SDL_LogError(0, "Failed to create window: %s", SDL_GetError());
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // | SDL_RENDERER_PRESENTVSYNC)
	if (renderer == nullptr) {
		SDL_LogError(0, "Failed to create renderer, sdl_window_id=%d: %s", sdl_window_id, SDL_GetError());
		return;
	}

	font = TTF_OpenFont("font.ttf", 14);
	if (!font) {
		SDL_LogError(0, "Failed to load font: %s", TTF_GetError());
		return;
	}
	TTF_SizeText(font, "a", &font_width, &font_height);

	cursor_normal = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	cursor_hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	cursor_type = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);

	window_count++;
}

void Window::run()
{
	this->compute_grid_sizes();
	this->apply_grid_layout();

	if (!custom_dimension) {
		width = 0;
		height = 0;
		for (int h : row_heights)
			height += h;
		for (int w : column_widths)
			width += w;
		dimensions(width, height);
	}
	EventHandler::register_window(this);
	if (internal_window_id == 0) {
		setlocale(LC_ALL, LOCALE);
		#ifdef _WIN32
		_wsetlocale(LC_ALL, W_LOCALE);
		#endif
		// start the event handler
		float last_time = 0.0f;
		float delta_time = 0.0f;
		while (running) {
			EventHandler::get().Poll(delta_time);
			update_and_render(delta_time);

			delta_time = SDL_GetTicks() - last_time;
			delta_time /= 1000;
			last_time = SDL_GetTicks();
		}
		clean();
		EventHandler::quit();
		SDL_Quit();
	}
}

void Window::clean()
{
	objects.clear();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Window::update_and_render(float dt)
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, 255); // background
	SDL_RenderClear(renderer);
	Parent::update_and_render(dt); // update children
	for (unsigned int i = 0; i < top_level.size(); i++) {
		top_level[i]();
	}
	top_level.clear();
	#ifdef DEBUG
	int total = 0;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (int w : column_widths) {
		SDL_RenderDrawLine(renderer, w + total, 0, w + total, height);
		total += w;
	}
	total = 0;
	for (int h : row_heights) {
		SDL_RenderDrawLine(renderer, 0, h + total, width, h + total);
		total += h;
	}
	#endif

	SDL_RenderPresent(renderer);
}

void Window::title(const char *title)
{
	SDL_SetWindowTitle(window, title);
}
void Window::quit()
{
	window_count--;
	running = false;
	if (internal_window_id != 0) {
		clean();
	}
}

// rendering

void Window::draw_text(int x, int y, const char* text, SDL_Color color)
{
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect src = {0, 0, surface->w, surface->h};
	int w = x;
	int h = y;
	SDL_Rect dst = {w, h, src.w, src.h};
	SDL_RenderCopy(renderer, texture, &src, &dst);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}


void Window::draw_rect(int x, int y, int w, int h, SDL_Color color, bool fill)
{
	SDL_Rect rect = {x, y, w, h};
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	if (fill)
		SDL_RenderFillRect(renderer, &rect);
	else
		SDL_RenderDrawRect(renderer, &rect);
}

void Window::draw_rounded_rect(int x, int y, int w, int h, int r, SDL_Color color, bool fill)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	if (fill) {
		DrawFillCircleTopLeft(x + r, y + r, r);
		DrawFillCircleBottomLeft(x + r, y + h - r, r);
		DrawFillCircleTopRight(x + w - r, y + r, r);
		DrawFillCircleBottomRight(x + w - r, y + h - r, r);
		// // connect them
		SDL_Rect temp_rect = {x + r, y, w - r*2, r};
		SDL_RenderFillRect(renderer, &temp_rect); // top
		temp_rect = {x, y+r, r, h - r*2};
		SDL_RenderFillRect(renderer, &temp_rect); // left
		temp_rect = {x + w - r, y + r, r+1, h - r*2};
		SDL_RenderFillRect(renderer, &temp_rect); // right
		temp_rect = {x + r, y + h - r, w - r*2, r+1};
		SDL_RenderFillRect(renderer, &temp_rect); // bottom
		temp_rect = {x + r, y + r, w - r*2, h - r*2+1};
		SDL_RenderFillRect(renderer, &temp_rect); // middle
	}
	else {
		DrawTopLeftCircle(x + r, y + r, r);
		DrawBottomLeftCircle(x + r, y + h - r, r);
		DrawTopRightCircle(x + w - r, y + r, r);
		DrawBottomRightCircle(x + w - r, y + h - r, r);
		// connect them
		SDL_RenderDrawLine(renderer, x + r, y, x + w - r, y); // top
		SDL_RenderDrawLine(renderer, x, y+r, x, y + h - r); // left
		SDL_RenderDrawLine(renderer, x + w, y + r, x + w, y + h - r); // right
		SDL_RenderDrawLine(renderer, x + r, y + h, x + w - r, y + h); // bottom
	}
}

void Window::draw_line(int x1, int y1, int x2, int y2, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Window::draw_circle(int x, int y, int radius, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}
}



void Window::draw_circle_fill(int x, int y, int radius, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {

		status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
		                             x + offsety, y + offsetx);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
		                             x + offsetx, y + offsety);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
		                             x + offsetx, y - offsety);
		status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
		                             x + offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}
}

// functions for rounded rect

void
Window::DrawFillCircleTopLeft(int x, int y, int radius)
{
	int offsetx, offsety, d;

	offsetx = 0;
	offsety = radius;
	d = radius -1;

	while (offsety >= offsetx) {

		SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
		                   x, y - offsety);
		SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
		                   x, y - offsetx);
		if (d >= 2*offsetx) {
			d -= 2*offsetx + 1;
			offsetx +=1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}
}
void
Window::DrawFillCircleBottomLeft(int x, int y, int radius)
{
	int offsetx, offsety, d;

	offsetx = 0;
	offsety = radius;
	d = radius -1;

	while (offsety >= offsetx) {

		SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
		                   x, y + offsetx);
		SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
		                   x, y + offsety);

		if (d >= 2*offsetx) {
			d -= 2*offsetx + 1;
			offsetx +=1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}
}
void
Window::DrawFillCircleTopRight(int x, int y, int radius)
{
	int offsetx, offsety, d;


	offsetx = 0;
	offsety = radius;
	d = radius -1;

	while (offsety >= offsetx) {

		SDL_RenderDrawLine(renderer, x, y - offsety,
		                   x + offsetx, y - offsety);
		SDL_RenderDrawLine(renderer, x, y - offsetx,
		                   x + offsety, y - offsetx);
		if (d >= 2*offsetx) {
			d -= 2*offsetx + 1;
			offsetx +=1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}
}
void
Window::DrawFillCircleBottomRight(int x, int y, int radius)
{
	int offsetx, offsety, d;

	offsetx = 0;
	offsety = radius;
	d = radius -1;

	while (offsety >= offsetx) {

		SDL_RenderDrawLine(renderer, x, y + offsetx,
		                   x + offsety, y + offsetx);
		SDL_RenderDrawLine(renderer, x, y + offsety,
		                   x + offsetx, y + offsety);

		if (d >= 2*offsetx) {
			d -= 2*offsetx + 1;
			offsetx +=1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}
}

void Window::DrawTopLeftCircle(int32_t centreX, int32_t centreY, int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y) {
		SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}
void Window::DrawTopRightCircle(int32_t centreX, int32_t centreY, int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y) {
		SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}
void Window::DrawBottomLeftCircle(int32_t centreX, int32_t centreY, int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y) {
		SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}
void Window::DrawBottomRightCircle(int32_t centreX, int32_t centreY, int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y) {
		SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}
