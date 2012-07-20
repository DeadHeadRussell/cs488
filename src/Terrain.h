#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <iostream>
#include <string>

#include "Primitive.h"

class HeightMap;
class Node;

namespace Terrain {
  void CreateMountain(std::string name);
  HeightMap* GenerateTerrain(std::string name);
  void ThermalWeathering(HeightMap* height_map, unsigned iterations);
};

class HeightMap : public Primitive {
 public:
  HeightMap();
  HeightMap(const HeightMap&);
  ~HeightMap();

  void SetNode(Node* node) { node_ = node; }
  Node* GetNode() { return node_; }

  unsigned GetWidth() const { return width_; }
  unsigned GetLength() const { return length_; }
  double GetTalus() const { return talus_; }

  HeightMap& operator=(const HeightMap& other);
  double* operator[](unsigned i) { return map_ + i * width_; }
  const double* operator[](unsigned i) const { return map_ + i * width_; }

  void ComputeNormals();
  virtual void Render() const;

 private:
  Vector3D& GetNormal(unsigned i, unsigned j) const;

  Node* node_;

  unsigned width_;
  unsigned length_;
  double* map_;
  Vector3D* normals_;
  double talus_;

  friend std::istream& operator>>(std::istream&, HeightMap&);
};

std::istream& operator>>(std::istream& stream, HeightMap& height_map);

#endif

