#include "Material.h"

#include <GL/gl.h>
#include <GL/glu.h>

Material::~Material() {}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks,
                             double shininess)
    : kd_(kd)
    , ks_(ks)
    , shininess_(shininess) {}

PhongMaterial::~PhongMaterial() {}

void PhongMaterial::Render() const {
  glColorMaterial(GL_FRONT, GL_SPECULAR);
  glColor3d(ks_.R(), ks_.G(), ks_.B());

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glColor3d(kd_.R(), kd_.G(), kd_.B());

  glMaterialf(GL_FRONT, GL_SHININESS, shininess_);
}

