#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <string>

#include "Algebra.h"

class Material {
 public:
  virtual ~Material();
  virtual void Render() const = 0;

 protected:
  Material() {}
};

class PhongMaterial : public Material {
 public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void Render() const;

 private:
  Colour kd_;
  Colour ks_;
  double shininess_;
};

class Texture : public Material {
 public:
  Texture(std::string file_name);
  virtual ~Texture();

  virtual void Render() const;

 private:
  bool tiff_;
  int width_;
  int height_;
  int components_;
  unsigned char* buffer_;

  unsigned texture_object_;
};

#endif
