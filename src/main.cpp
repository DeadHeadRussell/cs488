#include <gtkglmm.h>
#include <gtkmm.h>
#include <iostream>

#include "AppWindow.h"
#include "Terrain.h"

int main(int argc, char** argv) {
  Terrain::CreateMountain("test.hm");

  Gtk::Main kit(argc, argv);
  Gtk::GL::init(argc, argv);

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << "l | t | w | f" << std::endl;
  }

  AppWindow window(argv[1][0]);

  Gtk::Main::run(window);

  return 0;
}

