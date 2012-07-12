#include "Viewer.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "Logging.h"

Viewer::Viewer() {
  Glib::RefPtr<Gdk::GL::Config> gl_config;
  gl_config = Gdk::GL::Config::create(Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH |
                                      Gdk::GL::MODE_DOUBLE);

  if (!gl_config) {
    ERROR("Unable to setup OpenGL Configuration!");
    abort();
  }

  set_gl_capability(gl_config);

  add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
             Gdk::BUTTON3_MOTION_MASK | Gdk::POINTER_MOTION_MASK |
             Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
             Gdk::VISIBILITY_NOTIFY_MASK);
}

Viewer::~Viewer() {}

void Viewer::Invalidate() {
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect(allocation, false);
}

void Viewer::on_realize() {
  Gtk::GL::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Drawable> gl_drawable = get_gl_drawable();
  
  if (!gl_drawable || !gl_drawable->gl_begin(get_gl_context())) {
    return;
  }

  glClearColor(0.0, 0.0, 0.0, 0.0);

  gl_drawable->gl_end();
}

bool Viewer::on_expose_event(GdkEventExpose* event) {
  Glib::RefPtr<Gdk::GL::Drawable> gl_drawable = get_gl_drawable();
  if (!gl_drawable || !gl_drawable->gl_begin(get_gl_context())) {
    return false;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, get_width(), get_height());
  gluPerspective(40.0, (GLfloat)get_width() / get_height(), 0.1, 1000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render scene here

  gl_drawable->swap_buffers();
  gl_drawable->gl_end();

  return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event) {
  Glib::RefPtr<Gdk::GL::Drawable> gl_drawable = get_gl_drawable();
  if (!gl_drawable || !gl_drawable->gl_begin(get_gl_context())) {
    return false;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width / event->height, 0.1, 1000.0);

  glMatrixMode(GL_MODELVIEW);

  gl_drawable->gl_end();

  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event) {
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event) {
  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}

