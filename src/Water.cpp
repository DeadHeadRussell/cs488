#include "Water.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Node.h"
#include "Terrain.h"

using std::ifstream;
using std::list;
using std::max;
using std::min;
using std::string;
using std::stringstream;

string Water::vertex_shader_;
GLenum Water::water_program_;
GLenum Water::water_shader_;

Water::Water(const HeightMap& height_map, double height) {
  // Construct water map "filling in" height_map beneath height.
  height_ = height;
  int min_width = height_map.GetWidth();
  int max_width = 0;
  int min_length = height_map.GetLength();
  int max_length = 0;

  for (int i = 0; i < height_map.GetWidth(); i++) {
    for (int j = 0; j < height_map.GetLength(); j++) {
      if (height_map[i][j] < height) {
        min_width = min(min_width, i);
        max_width = max(max_width, i);
        min_length = min(min_length, j);
        max_length = max(max_length, j);
      }
    }
  }

  min_width = max(0, min_width - 1);
  max_width = min(height_map.GetWidth() - 1, (unsigned)max_width + 1);
  min_length = max(0, min_length - 1);
  max_length = min(height_map.GetLength() - 1, (unsigned)max_length + 1);

  std::cout << min_width << "  "<<max_width<<" "<<min_length<<" "<<max_length<<std::endl;

  height_map_ = new HeightMap(max_width - min_width, max_length - min_length,
                              height, false);
  node_ = Node::CreateHeightMapNode("Water-wrapper", height_map_, "data/img/water.jpg");
  node_->Translate(Vector3D(min_width, 0, min_length));

  if (vertex_shader_.size() == 0) {
    ifstream file("data/water.vert");
    stringstream ss;
    ss << file.rdbuf();
    vertex_shader_ = ss.str();

    water_program_ = glCreateProgramObjectARB();
    water_shader_ = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    const char* str = vertex_shader_.c_str();
    glShaderSourceARB(water_shader_, 1, (const GLcharARB**)&str, NULL);
    glCompileShaderARB(water_shader_);
    glAttachObjectARB(water_program_, water_shader_);
    glLinkProgramARB(water_program_);
  }
}

Water::~Water() {
  if (!node_->IsAttached()) {
    delete node_;
  }
}

double gaussian(double mean, double std_dev) {
  return (1 / (std_dev * sqrt(2 * M_PI))) *
      exp(-(1 / 2) * (mean / std_dev) * (mean / std_dev));
}

void Water::Animate(bool changed) {
  // if changed
  //   move to center of water
  //   render scene in six cardinal directions
  //   update cube

  if ((rand() % 5 == 0)) {
    Ripple r;
    r.origin_ = Point3D(rand() % height_map_->GetWidth(), 0.0,
                       rand() % height_map_->GetLength());

    r.amplitude_ = rand() % 10;
    r.gaussian_one_ = rand() % 9 + 1;
    r.gaussian_two_ = rand() % 9 + 1;
    r.phase_ = 0;
    r.length_ = 50;
    ripples_.push_back(r);
  }

  for (unsigned i = 0; i < height_map_->GetWidth(); i++) {
    for (unsigned j = 0; j < height_map_->GetLength(); j++) {
      (*height_map_)[i][j] = height_;
    }
  }

  list<Ripple>::iterator it;
  for (it = ripples_.begin(); it != ripples_.end(); ) {
    Ripple& r = *it;
    for (unsigned i = 0; i < height_map_->GetWidth(); i++) {
      for (unsigned j = 0; j < height_map_->GetLength(); j++) {
        double distance = Point3D(i, 0, j).Distance(r.origin_);
        (*height_map_)[i][j] = (*height_map_)[i][j] +
            gaussian(distance - r.phase_, r.gaussian_one_) *
            gaussian(distance, r.gaussian_two_) * cos(distance - r.phase_);
      }
    }
    r.phase_++;

    if (r.phase_ > r.length_) {
      it = ripples_.erase(it);
    } else {
      ++it;
    }
  }

  height_map_->ComputeNormals();
}

