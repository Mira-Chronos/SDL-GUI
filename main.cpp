#include <iostream>
#include <algorithm>
#include "SDL_GUI.hpp"
#include "Theme.hpp"
int main() {
  Window root = Window();
  Dropdown *d = new Dropdown(&root, "placeholder" , {"1", "2", "3"});
  d->grid(0, 0);


  Entry *e = new Entry(&root);
  e->grid(2,0);

  (new Button(&root, "set to 1", [d](){d->set(0);}))->grid(0, 1);
  
  Textbox *t = new Textbox(&root,10,20);
  (new Button(&root, "Display", [t](){std::cout << t->get()<< std::endl;}))->grid(1, 0);
  t->grid(1,1);



  (new Button(&root, "Display", [e](){std::cout << e->get()<< std::endl;}))->grid(2, 1);
  root.run();
}