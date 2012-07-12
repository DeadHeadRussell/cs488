#include "Appwindow.h"

using Gtk::Menu_Helpers::MenuElem;

AppWindow::AppWindow() {
  set_title("Project");

  menubar_.items().push_back(MenuElem("_Dummy", dummy_));
  add(vbox_);
  vbox_.pack_start(menubar_, Gtk::PACK_SHRINK);

  viewer_.set_size_request(300, 300);
  vbox_.pack_start(viewer_);

  show_all();
}

