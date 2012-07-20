#include "Terrain.h"

#include <fstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <limits>
#include <utility>
#include <vector>

#include "Node.h"

using std::ifstream;
using std::ios_base;
using std::istream;
using std::numeric_limits;
using std::ofstream;
using std::pair;
using std::string;
using std::vector;

namespace Terrain {

void CreateMountain(string file_name) {
  ofstream file(file_name.c_str(), ios_base::out | ios_base::binary);

  unsigned width = 100;
  double talus = 0;
  file.write((char*)&width, sizeof(width));
  file.write((char*)&width, sizeof(width));
  file.write((char*)&talus, sizeof(talus));

  for (unsigned i = 0; i < 100 * 10; i++) {
    double a = 50.0;
    file.write((char*)&a, sizeof(a));
  }

  for (unsigned i = 0; i < 80; i++) {
    for (unsigned j = 0; j < 10; j++) {
      double a = 50.0;
      file.write((char*)&a, sizeof(a));
    }

    for (unsigned j = 0; j < 80; j++) {
      double a = 0.0;
      file.write ((char*)&a, sizeof(a));
    }
    
    for (unsigned j = 0; j < 10; j++) {
      double a = 50.0;
      file.write((char*)&a, sizeof(a));
    }
  }

  for (unsigned i = 0; i < 100 * 10; i++) {
    double a = 50.0;
    file.write((char*)&a, sizeof(a));
  }

  file.close();
}

HeightMap* GenerateTerrain(string name) {
  ifstream map_stream(name.c_str());

  HeightMap* height_map = new HeightMap();
  map_stream >> *height_map;

  //ThermalWeathering(height_map, 500);

  Node* node = new Node(name);
  node->AddChild(Node::CreateHeightMapNode(name, height_map));
  height_map->SetNode(node);
  return height_map;
}

void ThermalWeathering(HeightMap* height_map, unsigned iterations) {
  HeightMap h1;
  HeightMap h2;

  static int neighbours[][2] = {
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, -1},
    {0, 1},
    {1, -1},
    {1, 0},
    {1, 1}
  };

  h2 = *height_map;

  for (unsigned t = 0; t < iterations; t++) {
    h1 = h2;
    double total = 0.0;
    for (int i = 0; (unsigned)i < h1.GetWidth(); i++) {
      for (int j = 0; (unsigned)j < h1.GetLength(); j++) {
        double total_diff = 0.0;
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
            total_diff += diff;
            if (diff < min_diff) {
              min_diff = diff;
            }
          }
        }
        
        unsigned count = neighbour_list.size();
        double redistribute;
        if (count > 0) {
          redistribute = ((double)count / (count + 1)) * min_diff;
          total += redistribute;
          h2[i][j] -= redistribute;
        }

        for (unsigned k = 0; k < count; k++) {
          unsigned n = neighbour_list[k].first;
          double diff = neighbour_list[k].second;

          int ni = i + neighbours[n][0];
          int nj = j + neighbours[n][1];
          h2[ni][nj] += (diff / total_diff) * redistribute;
        }
      }
    }
  }

  *height_map = h2;
  height_map->ComputeNormals();
}

}

HeightMap::HeightMap()
    : node_(0)
    , width_(0)
    , length_(0)
    , map_(0)
    , normals_(0)
    , talus_(0) {}

HeightMap::~HeightMap() {
  delete[] map_;
  delete[] normals_;
}

HeightMap::HeightMap(const HeightMap& other) {
  if (this == &other) {
    return;
  }

  node_ = other.node_;

  width_ = other.width_;
  length_ = other.length_;
  talus_ = other.talus_;

  delete[] map_;
  delete[] normals_;
  map_ = new double[width_ * length_];
  normals_ = new Vector3D[width_ * length_];

  for (unsigned i = 0; i < width_ * length_; i++) {
    map_[i] = other.map_[i];
    normals_[i] = other.normals_[i];
  }

  return;
}

HeightMap& HeightMap::operator=(const HeightMap& other) {
  if (this == &other) {
    return *this;
  }

  node_ = other.node_;

  width_ = other.width_;
  length_ = other.length_;
  talus_ = other.talus_;

  delete[] map_;
  delete[] normals_;
  map_ = new double[width_ * length_];
  normals_ = new Vector3D[width_ * length_];

  for (unsigned i = 0; i < width_ * length_; i++) {
    map_[i] = other.map_[i];
    normals_[i] = other.normals_[i];
  }

  return *this;
}

Colour getColour(double height) {
  // 0..100
  static Colour red(1.0, 0.0, 0.0);
  static Colour green(0.0, 1.0, 0.0);
  static Colour blue(0.0, 0.0, 1.0);

  if (height < 35) {
    return blue;
  } else if (height < 70) {
    return green;
  } else {
    return red;
  }
}

void HeightMap::Render() const {
  glFrontFace(GL_CW);
  glBegin(GL_QUADS);
  Colour c(0.0);
  double value;
  for (unsigned i = 0; i < width_ - 1; i++) {
    for (unsigned j = 0; j < length_ - 1; j++) {
      value = (*this)[i][j];
      c = getColour(value);
      glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
      glColor3d(c.R(), c.G(), c.B());
      Vector3D normal = GetNormal(i, j);
      glNormal3d(normal[0], normal[1], normal[2]);
      glVertex3d(i, value, j);

      value = (*this)[i + 1][j];
      c = getColour(value);
      glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
      glColor3d(c.R(), c.G(), c.B());
      normal = GetNormal(i, j);
      glNormal3d(normal[0], normal[1], normal[2]);
      glVertex3d(i + 1, value, j);

      value = (*this)[i + 1][j + 1];
      c = getColour(value);
      glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
      glColor3d(c.R(), c.G(), c.B());
      normal = GetNormal(i, j);
      glNormal3d(normal[0], normal[1], normal[2]);
      glVertex3d(i + 1, value, j + 1);

      value = (*this)[i][j + 1];
      c = getColour(value);
      glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
      glColor3d(c.R(), c.G(), c.B());
      normal = GetNormal(i, j);
      glNormal3d(normal[0], normal[1], normal[2]);
      glVertex3d(i, value, j + 1);
    }
  }
  glEnd();
}

Vector3D& HeightMap::GetNormal(unsigned i, unsigned j) const {
  return (normals_ + i * width_)[j];
}

void HeightMap::ComputeNormals() {
  for (unsigned i = 0; i < width_; i++) {
    for (unsigned j = 0; j < length_; j++) {
      double a = 0;
      double b = 0;
      double c = 0;
      double d = 0;

      if (i > 0) {
        a = (*this)[i - 1][j];
      }

      if (i < width_ - 1) {
        c = (*this)[i + 1][j];
      }

      if (j > 0) {
        d = (*this)[i][j - 1];
      }

      if (j < length_ - 1) {
        b = (*this)[i][j + 1];
      }
      
      Vector3D& N = GetNormal(i, j);
      N[0] = c - a;
      N[2] = b - d;
      N[1] = -200.0 * (1.0 / width_ + 1.0 / length_);

      N.Normalize();
    }
  }
};

istream& operator>>(istream& stream, HeightMap& height_map) {
  // File format:
  //   unsigned -> width
  //   unsigned -> height
  //   double -> talus
  //   double[width * height] -> map
  stream.read((char*)&(height_map.width_), sizeof(unsigned));
  stream.read((char*)&(height_map.length_), sizeof(unsigned));
  stream.read((char*)&(height_map.talus_), sizeof(double));

  delete[] height_map.map_;
  delete[] height_map.normals_;
  height_map.map_ = new double[height_map.width_ * height_map.length_];
  height_map.normals_ = new Vector3D[height_map.width_ * height_map.length_];

  stream.read((char*)height_map.map_,
              (height_map.width_ * height_map.length_) * sizeof(double));

  height_map.ComputeNormals();

  return stream;
}

