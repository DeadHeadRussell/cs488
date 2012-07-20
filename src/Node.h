#ifndef __NODE_H__
#define __NODE_H__

#include <list>

#include "Material.h"
#include "Primitive.h"

class GeometryNode;
class HeightMap;

class Node {
 public:
  Node(const std::string& name);
  virtual ~Node();

  virtual void Render() const;

  void AddChild(Node* child) { children_.push_back(child); child->parent_ = this; }
  void RemoveChild(Node* child) { children_.remove(child); child->parent_ = NULL; }
  void Detach() { if(parent_) { parent_->RemoveChild(this); } }
  bool IsAttached() { return parent_ != NULL; }

  virtual void Rotate(char axis, double angle);
  virtual void Scale(const Vector3D& amount);
  virtual void Translate(const Vector3D& amount);
  virtual void Transform(const Matrix4x4& transform);
 
  void SetTransformation(const Matrix4x4&);

  virtual bool IsJoint() const { return false; }

  static GeometryNode* CreateSphereNode(const std::string& name);
  static GeometryNode* CreateMeshNode(const std::string& name);
  static GeometryNode* CreateHeightMapNode(const std::string& name,
                                           HeightMap* primitive);

 protected:
  unsigned id_;
  std::string name_;

  Matrix4x4 transformation_;
  Matrix4x4 transformation_inverse_;

  typedef std::list<Node*> ChildList;
  ChildList children_;
  Node* parent_;
};

class JointNode : public Node {
 public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  struct JointRange {
    double min, init, max;
  };

  virtual void Render() const;
  virtual void Rotate(char axis, double angle);
  virtual bool IsJoint() const { return true; }

  void SetJointRange(JointRange joint_range_x, JointRange joint_range_y);
  
protected:
  JointRange joint_x_;
  JointRange joint_y_;
  double angle_x_;
  double angle_y_;
};

class GeometryNode : public Node {
 public:
  GeometryNode(const std::string& name, Primitive* primitive,
               Material* material);
  virtual ~GeometryNode();

  virtual void Render() const;
  virtual void Scale(const Vector3D& amount);

  Material* GetMaterial() { return material_; }
  Primitive* GetPrimitive() { return primitive_; }

 protected:
  Matrix4x4 scale_transformation_;
  Material* material_;
  Primitive* primitive_;
};

#endif

