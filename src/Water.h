#ifndef __WATER_H__
#define __WATER_H__

#define GL_GLEXT_PROTOTYPES 

#include <GL/gl.h>
#include <GL/glu.h>
#include <list>
#include <string>

#include "Primitive.h"

class HeightMap;
class Node;

class Water {
 public:
  Water(const HeightMap& height_map, double height);
  ~Water();

  Node* GetNode() { return node_; }

  void Animate(bool update_cube);
  void Render();

 private:
  static std::string vertex_shader_;
  static GLenum water_program_;
  static GLenum water_shader_;

  struct Ripple {
    Point3D origin_;
    double amplitude_;
    double gaussian_one_;
    double gaussian_two_;
    unsigned phase_;
    unsigned length_;
  };

  Node* node_;
  HeightMap* height_map_;
  std::list<Ripple> ripples_;
  double height_;
};

#endif

