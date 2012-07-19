#include "Viewer.h"

#include <algorithm>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Logging.h"
#include "LSystem.h"
#include "Terrain.h"
#include "Trackball.h"

using std::min;

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

  Node* bush = LSystem::GenerateBush("Bush");
  bush->Translate(Vector3D(-100, -200, -200));
  bush->Scale(Vector3D(0.1, 0.1, 0.1));

  Node* tree = LSystem::GenerateTree("Tree");
  tree->Translate(Vector3D(100, -200, -500));
  tree->Scale(Vector3D(0.1, 0.1, 0.1));

  Node* terrain = GenerateTerrain("test.hm");
  terrain->Translate(Vector3D(0, -200, -900));
  terrain->Scale(Vector3D(0.1, 0.1, 0.1));

  root_ = new Node("root");
  root_->AddChild(terrain);
  //root_->AddChild(bush);
  //root_->AddChild(tree);

  mouse_prev_[0] = mouse_prev_[1] = 0;
  button_down_ = false;
}

Viewer::~Viewer() {
  delete root_;
}

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

  glShadeModel(GL_SMOOTH);
  glClearColor(0, 0, 0, 0);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);

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

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_LIGHTING);

  float pos[] = {50.0, 0.0, 10.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, pos);

  float ambient[] = {0.3, 0.3, 0.3, 1.0};
  float diffuse[] = {0.8, 0.8, 0.8, 1.0};
  float specular[] = {0.5, 0.5, 0.5, 1.0};

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

  // Render scene here
  root_->Render();

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
  if (!button_down_) {
    mouse_prev_[0] = event->x;
    mouse_prev_[1] = event->y;
  }

  button_down_ = true;
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event) {
  unsigned button_count = 0;

  if (event->state & Gdk::BUTTON1_MASK) {
    button_count++;
  }

  if (event->state & Gdk::BUTTON2_MASK) {
    button_count++;
  }

  if (event->state  &Gdk::BUTTON3_MASK) {
    button_count++;
  }

  if (button_count <= 1) {
    button_down_ = false;
  }

  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event) {
  bool button[3];
  button[0] = event->state & Gdk::BUTTON1_MASK;
  button[1] = event->state & Gdk::BUTTON2_MASK;
  button[2] = event->state & Gdk::BUTTON3_MASK;

  if (!(button[0] || button[1] || button[2])) {
    return true;
  }

  double mouse_diff[2];
  mouse_diff[0] = (double)(event->x - mouse_prev_[0]);
  mouse_diff[1] = (double)(event->y - mouse_prev_[1]);

  if (button[0]) {
    root_->Translate(Vector3D(mouse_diff[0], -mouse_diff[1], 0.0));
  } else if (button[1]) {
    root_->Translate(Vector3D(0.0, 0.0, -mouse_diff[1]));
  } else if (button[2]) {
    root_->Rotate('y', mouse_diff[0] * M_PI / 180);
  }

  Invalidate();

  mouse_prev_[0] = event->x;
  mouse_prev_[1] = event->y;

  return true;
}

