#include <gtkglmm.h>
#include <gtkmm.h>

#include "appwindow.h"

int main(int argc, char** argv) {
  Gtk::Main kit(argc, argv);
  Gtk::GL::init(argc, argv);

  AppWindow window;

  Gtk::Main::run(window);

  return 0;
}

