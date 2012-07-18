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
  //float specular[] = {ks_.R(), ks_.G(), ks_.B(), 1.0};
  //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

  //glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glColor3d(kd_.R(), kd_.G(), kd_.B());

  //glMaterialf(GL_FRONT, GL_SHININESS, shininess_);
}

