#include "Primitive.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "Logging.h"

#include <iostream>
using namespace std;

Primitive::~Primitive() {}

Sphere::~Sphere() {}

void Sphere::Render() const {
  FUNC_ENTER;
  GLUquadricObj* quadric = gluNewQuadric();
  gluQuadricDrawStyle(quadric, GLU_FILL);

  gluSphere(quadric, 1, 12, 12);

  gluDeleteQuadric(quadric); 
}

Mesh::~Mesh() {}

void Mesh::Render() const {
  FUNC_ENTER;
  GLUquadricObj* quadric = gluNewQuadric();
  gluQuadricDrawStyle(quadric, GLU_FILL);

  CylinderList::const_iterator it;

  for (it = cylinders_.begin(); it != cylinders_.end(); ++it) {
    glPushMatrix();

    Point3D p = (*it).pos_;
    glTranslated(p[0], p[1], p[2]);

    // Cylinders are constructed from z = 0 .. height
    // First rotate along x-axis so cylinders are y = 0 .. height
    // then rotate using the rotation_ matrix.
    Matrix4x4 transform = 
      (*it).rotation_ * 
      Matrix4x4(
        Vector4D(1, 0, 0, 0),
        Vector4D(0, 0, 1, 0),
        Vector4D(0, -1, 0, 0),
        Vector4D(0, 0, 0, 1));
    glMultMatrixd(transform.Transpose().Begin());

    gluCylinder(quadric, (*it).radius_, (*it).end_radius_, (*it).height_, 8, 1);

    glPopMatrix();
  }

  gluDeleteQuadric(quadric); 
}

void Mesh::Extend(double length, const Matrix4x4& rotation, double radius, double end_radius) {
  Cylinder c;
  c.height_ = length;
  c.rotation_ = rotation;
  c.radius_ = radius;
  c.end_radius_ = end_radius;
  c.pos_ = end_point_;

  cylinders_.push_back(c);

  Vector3D heading = rotation * Vector3D(0.0, 1.0, 0.0);

  double heading_length = sqrt((length * length) / (heading.Length2()));
  end_point_ = end_point_ + heading_length * heading;
}

