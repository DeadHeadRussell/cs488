#include "Flock.h"

#include <cstdlib>
#include <sstream>

#include "Node.h"

using std::stringstream;

class Fish : public Flock::Animal {
 public:
  Fish(unsigned i);
  ~Fish();

  bool WillCollide(const Flock::FlockList& flock);
  virtual void SetVelocity(const Vector3D& velocity);
  virtual bool Move(const Flock::FlockList& flock);
  virtual bool MoveToCenter(const Flock::FlockList& flock);
  virtual bool MoveRandomly(const Flock::FlockList& flock);

 private:
  double size_;
  double max_speed_;
  double max_random_;

  Vector3D next_velocity_;
};

Fish::Fish(unsigned i) {
  stringstream ss;
  ss << "Fish" << i;
  node_ = Node::CreateObjectNode(ss.str(), "data/mesh/goldfish.obj", "data/img/goldfish.tif");
  node_->Scale(Vector3D(120, 120, 120));

  max_speed_ = 0.3;
  max_random_ = 20;

  size_ = 2;
  alignment_ = 8;
  attraction_ = 20;

  double x = (i % 5) * 10 + ((rand() % 40) / 10.0 - 2.0);
  double y = (i / 5) * 10 + ((rand() % 40) / 10.0 - 2.0);
  position_ = Point3D(x, y, 0);
  node_->Translate(Vector3D(x, y, 0));
  node_->Rotate('x', 90);
}

bool Fish::WillCollide(const Flock::FlockList& flock) {
  Flock::FlockList::const_iterator it;
  for (it = flock.begin(); it != flock.end(); ++it) {
    if ((position_ + velocity_).Distance((*it)->GetPosition()) <= size_ * 2) {
      return true;
    }
  }
  return false;
}

void Fish::SetVelocity(const Vector3D& velocity) {
  velocity_ = next_velocity_;
  next_velocity_ = velocity;
  next_velocity_[0] += (rand() % 8 / 10.0 - 1.0);
  next_velocity_[1] += (rand() % 8 / 10.0 - 1.0);
  next_velocity_[2] += (rand() % 8 / 10.0 - 1.0);
  double speed = next_velocity_.Length();
  if (speed > max_speed_) {
    next_velocity_ = (max_speed_ / speed) * next_velocity_;
  }
}

bool Fish::Move(const Flock::FlockList& flock) {
  if (!WillCollide(flock) && velocity_.Length() > 0.01) {
    position_ = position_ + velocity_;
    node_->Translate(velocity_);
    return true;
  }
  return false;
}

bool Fish::MoveToCenter(const Flock::FlockList& flock) {
  Flock::FlockList::const_iterator it;
  Point3D center;
  for (it = flock.begin(); it != flock.end(); ++it) {
    center = center + (*it)->GetPosition();
  }

  center = (1.0 / flock.size()) * center;
  center = center + Point3D((rand() / (max_random_ * RAND_MAX)) - (0.5 / max_random_),
                            (rand() / (max_random_ * RAND_MAX)) - (0.5 / max_random_),
                            (rand() / (max_random_ * RAND_MAX)) - (0.5 / max_random_));
  SetVelocity(center - position_);
  return Move(flock);
}

bool Fish::MoveRandomly(const Flock::FlockList& flock) {
  do {
    SetVelocity(Vector3D((rand() / (max_random_ * RAND_MAX)) - (0.5 / max_random_),
                         (rand() / (max_random_ * RAND_MAX)) - (0.5 / max_random_),
                         (rand() / (max_random_ * RAND_MAX)) - (0.5 / max_random_)));
  } while (!Move(flock));
  return true;
}

Fish::~Fish() {
  if (!node_->IsAttached()) {
    delete node_;
  }
}

Flock::Flock(Type type, unsigned number)
    : at_destination_(true) {
  node_ = new Node("Flock-wrapper");

  if (type == FISH) {
    for (unsigned i = 0; i < number; i++) {
      Animal* a = new Fish(i);
      flock_.push_back(a);

      node_->AddChild(a->GetNode());
    }
  }
}

Flock::~Flock() {
  FlockList::iterator it;
  for (it = flock_.begin(); it != flock_.end(); ++it) {
    delete (*it);
  }

  if (!node_->IsAttached()) {
    delete node_;
  }
}

void Flock::SetBounds(HeightMap& map, unsigned height) {

}

void Flock::Move() {
  FlockList::iterator it;
  Point3D center;
  for (it = flock_.begin(); it != flock_.end(); ++it) {
    Animal* a = *it;

    Vector3D velocity;
    unsigned count = 0;

    FlockList alignment;
    FlockList attraction;
    center = center + (*it)->GetPosition();

    FlockList::iterator other_it;
    for (other_it = flock_.begin(); other_it != flock_.end(); ++other_it) {
      Animal* other = *other_it;
      if (a == other) {
        continue;
      }

      if (a->InAlignment(other)) {
        alignment.push_back(other);
        count++;
        velocity = velocity + other->GetVelocity();
      }

      if (a->InAttraction(other)) {
        attraction.push_back(other);
      }
    }

    if (count > 0) {
      velocity = (1.0 / count) * velocity;
    }
    a->SetVelocity(velocity);

    if (!at_destination_) {
      bool move_to_destination = ((rand() % 5) == 0);
      if (move_to_destination) {
        a->SetVelocity(destination_ - a->GetPosition());
      }
    }

    if (!a->Move(alignment)) {
      if (!a->MoveToCenter(attraction)) {
        a->MoveRandomly(attraction);
      }
    }
  }

  center = (1.0 / flock_.size()) * center;
  if ((center - destination_).Length() < flock_.size()) {
    at_destination_ = true;
  }
}

