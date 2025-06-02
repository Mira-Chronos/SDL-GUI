#include "Progressbar.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "EventHandler.hpp"


Progressbar::Progressbar(std::shared_ptr<Parent> parent, int min, int max)
    : Widget(parent), min(min), max(max), value(min)
{
  	w = 100;
	h = 10;
}

void Progressbar::update_and_render(float dt)
{
    // Appel du rendu de base (optionnel)
    Widget::update_and_render(dt);

    // Calcul de la largeur remplie
    float percent = (max > min) ? float(value - min) / float(max - min) : 0.0f;
    int fill_w = int(w * percent);

    // Dessin du fond
    window->draw_rect(x, y, w, h, Theme::PROGRESSBAR_BACKGROUND);

    // Dessin de la barre de progression
    window->draw_rect(x, y, fill_w, h, Theme::PROGRESSBAR_FOREGROUND);
}

void Progressbar::on_press()
{
    // Récupère la position de la souris
    int mouse_x = EventHandler::get_mouse_x();
    // Calcule la position relative dans la barre
    int relative_x = mouse_x - x;
    // Clamp la valeur entre 0 et la largeur de la barre
    if (relative_x < 0) relative_x = 0;
    if (relative_x > w) relative_x = w;
    // Met à jour la valeur en fonction du clic
    float percent = float(relative_x) / float(w);
    value = min + int(percent * (max - min));
    this->set(value);
}