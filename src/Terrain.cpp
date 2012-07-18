#include "Terrain.h"

#include <limits>
#include <utility>
#include <vector>

#include "Node.h"

using std::istream;
using std::numeric_limits;
using std::pair;
using std::string;
using std::vector;

Node* GenerateTerrain(istream& map_stream, string name) {
  HeightMap height_map;
  map_stream >> height_map;

  ThermalWeathering(height_map, 100);

  Node* node = new Node(name);
  return node;
}

void ThermalWeathering(HeightMap& height_map, unsigned iterations) {
  HeightMap h1 = height_map;
  HeightMap h2;

  int neighbours[][2] = {
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, -1},
    {0, 1},
    {1, -1},
    {1, 0},
    {1, 1}
  };

  for (unsigned t = 0; t < iterations; t++) {
    h2 = h1;
    for (int i = 0; (unsigned)i < h1.GetWidth(); i++) {
      for (int j = 0; (unsigned)j < h1.GetLength(); j++) {
        double min_diff = numeric_limits<double>::infinity();

        vector<pair<unsigned, double> > neighbour_list;

        for (unsigned n = 0; n < 8; n++) {
          int ni = i + neighbours[n][0];
          int nj = j + neighbours[n][1];
          if (ni < 0 || (unsigned)ni >= h1.GetWidth() || nj < 0 ||
              (unsigned)nj >= h1.GetLength()) {
            continue;
          }

          double diff = h1[i][j] - h1[ni][nj];

          if (diff > h1.GetTalus()) {
            neighbour_list.push_back(pair<unsigned, double>(n, diff));
            if (diff < min_diff) {
              min_diff = diff;
            }
          }
        }
        
        for (unsigned i = 0; i < neighbour_list.size(); i++) {
          unsigned n = neighbour_list[i].first;
          double diff = neighbour_list[i].second;

          double amount = (diff / (diff + 1)) * min_diff;

          unsigned ni = i + neighbours[n][0];
          unsigned nj = j + neighbours[n][1];
          h2[i][j] -= amount;
          h2[ni][nj] += amount;
        }
      }
    }
  }
}

HeightMap::HeightMap()
    : width_(0)
    , length_(0)
    , map_(0)
    , talus_(0) {}

HeightMap::~HeightMap() {
  delete[] map_;
}

HeightMap& HeightMap::operator=(const HeightMap& other) {
  width_ = other.width_;
  length_ = other.length_;
  map_ = new double[width_ * length_];
  talus_ = other.talus_;

  for (unsigned i = 0; i < width_ * length_; i++) {
    map_[i] = other.map_[i];
  }

  return *this;
}

void HeightMap::Render() const {
  for (unsigned x = 0; x < width_; x++) {
    for (unsigned y = 0; y < length_; y++) {
      //
    }
  }
}

istream& operator>>(istream& stream, HeightMap& height_map) {
  // File format:
  //   unsigned -> width
  //   unsigned -> height
  //   double -> talus
  //   double[width * height] -> map
  stream >> height_map.width_;
  stream >> height_map.length_;
  stream >> height_map.talus_;

  delete[] height_map.map_;
  height_map.map_ = new double[height_map.width_ * height_map.length_];
  stream.read((char*)height_map.map_,
              (height_map.width_ * height_map.length_) * sizeof(double));

  return stream;
}

