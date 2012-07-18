#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include <list>

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

#endif

