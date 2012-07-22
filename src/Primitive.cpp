#include "Primitive.h"

#include <fstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <tiffio.h>

#include "Logging.h"

using std::ifstream;
using std::list;
using std::pair;
using std::vector;

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
  gluQuadricTexture(quadric, GL_TRUE);

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

Object::Object(const std::string& file_name) {
  // Read in .obj file
  // if you didn't pass in a .obj file, then you're using my program wrong.
  // aka, I don't have time to write a proper parser.
  ifstream file(file_name.c_str());

  // only support files with max 256 chars per line, since I'm only using files
  // that do that.
  char line[256];
  while(true) {
    file.getline(line, 256);
    if (file.eof()) {
      break;
    }

    if (line[0] == 'v' && line[1] == ' ') {
      char temp[4];
      float x, y, z;
      sscanf(line, "%s %f %f %f", temp, &x, &y, &z);
      vertices_.push_back(Point3D(x, y, z));
    } else if (line[0] == 'v' && line[1] == 't') {
      char temp[4];
      float u, v;
      sscanf(line, "%s %f %f", temp, &u, &v);
      texture_vertices_.push_back(Point2D(u, v));
    } else if (line[0] == 'f') {
      char temp[4];
      unsigned v1, v2, v3;
      int vt1, vt2, vt3;
      int matches = sscanf(line, "%s %u/%d %u/%d %u/%d", temp, &v1, &vt1, &v2,
                           &vt2, &v3, &vt3);
      if (matches != 7) {
        sscanf(line, "%s %u %u %u", temp, &v1, &v2, &v3);
        vt1 = vt2 = vt3 = 0;
      }

      // OBJ file indexing starts at 1.
      v1--; v2--; v3--; vt1--; vt2--; vt3--;

      face_.push_back(pair<unsigned, int>(v1, vt1));
      face_.push_back(pair<unsigned, int>(v2, vt2));
      face_.push_back(pair<unsigned, int>(v3, vt3));
    } else {
      // ignore everything else
    }
  }
}

Object::~Object() {}

void Object::Render() const {
  glBegin(GL_TRIANGLES);

  list<pair<unsigned, int> >::const_iterator it;
  for (it = face_.begin(); it != face_.end(); ++it) {
    pair<unsigned, int> p = (*it);
    if (p.second >= 0) {
      const Point2D& vt = texture_vertices_[p.second];
      glTexCoord2d(vt[0], vt[1]);
    }

    const Point3D& v = vertices_[p.first];
    glVertex3d(v[0], v[1], v[2]);
  }

  glEnd();
}

