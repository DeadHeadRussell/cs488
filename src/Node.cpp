#include "Node.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "Terrain.h"

using std::list;
using std::string;

static int ids = 0;

GeometryNode* Node::CreateMeshNode(const string& name) {
  Primitive* primitive = new Mesh();
  Material* material = new PhongMaterial(Colour(0.2, 0.6, 0.2), Colour(0.1, 0.1, 0.1), 1.0);
  GeometryNode* node = new GeometryNode(name, primitive, material);
  return node;
}

GeometryNode* Node::CreateSphereNode(const string& name) {
  Primitive* primitive = new Sphere();
  Material* material = new PhongMaterial(Colour(1.0, 1.0, 1.0), Colour(1.0, 1.0, 1.0), 1.0);
  GeometryNode* node = new GeometryNode(name, primitive, material);
  return node;
}

GeometryNode* Node::CreateHeightMapNode(const string& name, HeightMap* primitive) {
  Material* material = new PhongMaterial(Colour(1.0, 1.0, 1.0), Colour(1.0, 1.0, 1.0), 1.0);
  GeometryNode* node = new GeometryNode(name, primitive, material);
  return node;
}

Node::Node(const string& name)
    : name_(name) {
  id_ = ids++;
}

Node::~Node() {
  ChildList::const_iterator it = children_.begin();
  for (; it != children_.end(); it++) {
    delete *it;
  }
}

void Node::Render() const {
  glPushMatrix();
  glMultMatrixd(transformation_.Transpose().Begin());

  ChildList::const_iterator it = children_.begin();
  for (; it != children_.end(); it++) {
    (*it)->Render();
  }

  glPopMatrix();
}

void Node::Rotate(char axis, double angle) {
  Matrix4x4 r;
  double s = sin(angle * M_PI / 180);
  double c = cos(angle * M_PI / 180);

  switch (axis) {
    case 'x':
    case 'X':
      r = Matrix4x4(Vector4D(1, 0, 0, 0), Vector4D(0, c, -s, 0),
                    Vector4D(0, s, c, 0), Vector4D(0, 0, 0, 1));
      break;

    case 'y':
    case 'Y':
      r = Matrix4x4(Vector4D(c, 0, s, 0), Vector4D(0, 1, 0, 0),
                    Vector4D(-s, 0, c, 0), Vector4D(0, 0, 0, 1));
      break;

    case 'z':
    case 'Z':
      r = Matrix4x4(Vector4D(c, -s, 0, 0), Vector4D(s, c, 0, 0),
                    Vector4D(0, 0, 1, 0), Vector4D(0, 0, 0, 1));
      break;

    default:
      break;
  }

  SetTransformation(transformation_ * r);
}

void Node::Scale(const Vector3D& amount) {
  Matrix4x4 s(Vector4D(amount[0], 0, 0, 0), Vector4D(0, amount[1], 0, 0),
              Vector4D(0, 0, amount[2], 0), Vector4D(0, 0, 0, 1));
  SetTransformation(transformation_ * s);
}

void Node::Translate(const Vector3D& amount) {
  Matrix4x4 t(Vector4D(1, 0, 0, amount[0]), Vector4D(0, 1, 0, amount[1]),
              Vector4D(0, 0, 1, amount[2]), Vector4D(0, 0, 0, 1));
  SetTransformation(transformation_ * t);
}

void Node::Transform(const Matrix4x4& transformation) {
  SetTransformation(transformation_ * transformation);
}

void Node::SetTransformation(const Matrix4x4& transformation) {
  transformation_ = transformation;
  transformation_inverse_ = transformation.Invert();
}

JointNode::JointNode(const string& name)
  : Node(name) {}

JointNode::~JointNode() {}

void JointNode::Rotate(char axis, double angle) {
  switch (axis) {
    case 'x':
      angle_x_ += angle;

      if (angle_x_ > joint_x_.max) {
        angle -= angle_x_ - joint_x_.max;
        angle_x_ = joint_x_.max;
      }

      if (angle_x_ < joint_x_.min) {
        angle -= angle_x_ - joint_x_.min;
        angle_x_ = joint_x_.min;
      }
      break;

    case 'y':
      angle_y_ += angle;

      if (angle_y_ > joint_y_.max) {
        angle -= angle_y_ - joint_y_.max;
        angle_y_ = joint_y_.max;
      }

      if (angle_y_ < joint_y_.min) {
        angle -= angle_y_ - joint_y_.min;
        angle_y_ = joint_y_.min;
      }
      break;

    default:
      angle = 0;
      break;

  }

  if (angle != 0) {
    Node::Rotate(axis, angle);
  }
}

void JointNode::Render() const {
  glPushMatrix();
  glMultMatrixd(transformation_.Transpose().Begin());

  ChildList::const_iterator it = children_.begin();
  for (; it != children_.end(); it++) {
    (*it)->Render();
  }

  glPopMatrix();
}

void JointNode::SetJointRange(JointRange joint_x, JointRange joint_y) {
  joint_x_ = joint_x;
  angle_x_ = joint_x.init;

  joint_y_ = joint_y;
  angle_y_ = joint_y.init;
}

GeometryNode::GeometryNode(const string& name, Primitive* primitive, Material* material)
    : Node(name)
    , material_(material)
    , primitive_(primitive) {}

GeometryNode::~GeometryNode() {
  delete primitive_;
  delete material_;
}

void GeometryNode::Scale(const Vector3D& amount) {
  Matrix4x4 s(Vector4D(amount[0], 0, 0, 0), Vector4D(0, amount[1], 0, 0),
              Vector4D(0, 0, amount[2], 0), Vector4D(0, 0, 0, 1));
  scale_transformation_ = scale_transformation_ * s;
}

void GeometryNode::Render() const {
  glPushMatrix();
  glPushName(id_);
  glMultMatrixd(transformation_.Transpose().Begin());

  glPushMatrix();
  glMultMatrixd(scale_transformation_.Transpose().Begin());

  if (material_) {
    material_->Render();
  }

  if (primitive_) {
    primitive_->Render();
  }

  // Pop the scale transformation
  glPopMatrix();

  ChildList::const_iterator it = children_.begin();
  for (; it != children_.end(); it++) {
    (*it)->Render();
  }

  glPopMatrix();
}
 
