#ifndef __FLOCK_H__
#define __FLOCK_H__

#include <list>

#include "Algebra.h"

class HeightMap;
class Node;

class Flock {
 public:
  enum Type {
    FISH
  };

  Flock(Type type = FISH, unsigned number = 20);
  ~Flock();

  void SetBounds(HeightMap& map, unsigned height);
  Node* GetNode() { return node_; }
  void SetDestination(const Point3D& destination) {
    destination_ = destination; at_destination_ = false; }
  bool AtDestination() { return at_destination_; }
  void Move();
 
  class Animal;
  typedef std::list<Animal*> FlockList;

  class Animal {
   public:
    Animal() : node_(NULL) {}
    virtual ~Animal() {}

    virtual Node* GetNode() { return node_; }

    virtual const Vector3D& GetVelocity() const { return velocity_; }
    virtual const Point3D& GetPosition() const { return position_; }

    virtual bool InAlignment(const Animal* other) const {
      return position_.Distance(other->position_) < alignment_; }
    virtual bool InAttraction(const Animal* other) const {
      return position_.Distance(other->position_) < attraction_; }
    virtual void SetVelocity(const Vector3D& velocity) { velocity_ = velocity; }
    virtual bool Move(const FlockList& flock) = 0;
    virtual bool MoveToCenter(const FlockList& flock) = 0;
    virtual bool MoveRandomly(const FlockList& flock) = 0;

   protected:
    Node* node_;

    Point3D position_;
    Vector3D velocity_;

    double alignment_;
    double attraction_;
  };

 private:
  FlockList flock_;
  Node* node_;

  bool at_destination_;
  Point3D destination_;
};

#endif

