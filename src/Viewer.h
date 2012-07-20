#ifndef __VIEWER_H__
#define __VIEWER_H__

#include <gtkglmm.h>
#include <gtkmm.h>

#include "Flock.h"
#include "Node.h"

class Viewer : public Gtk::GL::DrawingArea {
 public:
  Viewer();
  virtual ~Viewer();

  void Invalidate();

 protected:
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);

 private:
  Node* root_;

  HeightMap* terrain_;
  Flock flock_;

  bool button_down_;
  int mouse_prev_[2];
};

#endif

