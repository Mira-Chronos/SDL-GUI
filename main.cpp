#include <iostream>
#include <algorithm>
#include "SDL_GUI.hpp"
#include "Theme.hpp"
int main() {
  Window root = Window();
  Dropdown *d = new Dropdown(&root, "placeholder" , {"1", "2", "3"});
  d->grid(0, 0);
  (new Button(&root, "set to 1", [d](){d->set(1);}))->grid(0, 1);
  root.run();
}