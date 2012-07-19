#include <gtkglmm.h>
#include <gtkmm.h>

#include "AppWindow.h"
#include "Terrain.h"

int main(int argc, char** argv) {
  CreateMountain("test.hm");

  Gtk::Main kit(argc, argv);
  Gtk::GL::init(argc, argv);

  AppWindow window;

  Gtk::Main::run(window);

  return 0;
}

