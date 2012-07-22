#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include <list>
#include <utility>
#include <vector>

#include "Algebra.h"

class Primitive {
 public:
  virtual ~Primitive();
  virtual void Render() const = 0;
};

class Sphere : public Primitive {
 public:
  virtual ~Sphere();
  virtual void Render() const;
};

class Mesh : public Primitive {
 public:
  virtual ~Mesh();
  virtual void Render() const;
  void Extend(double length, const Matrix4x4& rotation, double radius, double end_radius);
  const Point3D& GetEndPoint() const { return end_point_; }

 private:
  struct Cylinder {
    Matrix4x4 rotation_;
    Point3D pos_;
    double height_;
    double radius_;
    double end_radius_;
  };

  Point3D end_point_;
  typedef std::list<Cylinder> CylinderList;
  CylinderList cylinders_;
};

class Object : public Primitive {
 public:
  Object(const std::string& file_name);
  virtual ~Object();

  virtual void Render() const;

 private:
  std::vector<Point3D> vertices_;
  std::vector<Point2D> texture_vertices_;
  std::list<std::pair<unsigned, int> > face_;
};

#endif

