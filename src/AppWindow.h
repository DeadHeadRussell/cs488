#ifndef __APP_WINDOW_H__
#define __APP_WINDOW_H__

#include <gtkmm.h>

#include "Viewer.h"

class AppWindow : public Gtk::Window {
 public:
  AppWindow();

 private:
  Gtk::VBox vbox_;

  Gtk::MenuBar menubar_;
  Gtk::Menu dummy_;

  Viewer viewer_;
};

#endif

