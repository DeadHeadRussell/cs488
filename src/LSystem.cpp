#include "LSystem.h"

#include <cstdlib>
#include <sstream>

#include "Logging.h"

using std::string;
using std::stringstream;

void yawLeft(LSystem::TurtleState&, double);
void yawRight(LSystem::TurtleState&, double);
void pitchDown(LSystem::TurtleState&, double);
void pitchUp(LSystem::TurtleState&, double);
void rollLeft(LSystem::TurtleState&, double);
void rollRight(LSystem::TurtleState&, double);
void pushNode(LSystem::TurtleState&, double);
void popNode(LSystem::TurtleState&, double);
void decreaseWidth(LSystem::TurtleState&, double);
void drawForward(LSystem::TurtleState&, double);

LSystem::EvalRules LSystem::common_eval_;

void LSystem::InitializeCommonRules() {
  common_eval_['+'] = yawLeft;
  common_eval_['-'] = yawRight;
  common_eval_['&'] = pitchDown;
  common_eval_['^'] = pitchUp;
  common_eval_['<'] = rollLeft;
  common_eval_['>'] = rollRight;
  common_eval_['['] = pushNode;
  common_eval_[']'] = popNode;
  common_eval_['!'] = decreaseWidth;
}

Node* LSystem::Generate(ExpandRules expand, EvalRules eval, TurtleState state) {
  if (!common_eval_.size()) {
    InitializeCommonRules();
  }

  // Insert rules into a copy of common_eval so the caller can overwrite any of
  // the common_eval rules.
  EvalRules temp = common_eval_;
  temp.insert(eval.begin(), eval.end());
  eval = temp;

  if (!eval['F']) {
    eval['F'] = drawForward;
  }

  for (unsigned i = 0; i < state.iterations_; i++) {
    for (unsigned j = 0; j < state.turtle_.size(); j++) {
      if (expand.find(state.turtle_[j]) == expand.end()) {
        continue;
      }

      ExpandPair rule = expand[state.turtle_[j]];
      string replacement = rule.first;
      state.turtle_.replace(j, 1, replacement);
      j += replacement.size() - 1;

      if (state.turtle_[j + 1] == '(') {
        size_t pos = state.turtle_.find(')', j + 1);
        if (pos == string::npos) {
          stringstream ss;
          ss << "Parse error (pos: " << (j + 1) << "): Unmatched '(' in turtle!";
          ERROR(ss.str());
          return NULL;
        }

        stringstream ss(state.turtle_.substr(j + 2, pos - j - 2));
        double parameter;
        ss >> parameter;

        if (rule.second) {
          parameter = rule.second(state, parameter);
          state.turtle_.erase(j + 2, pos - j - 2);
          stringstream ss;
          ss << parameter;
          state.turtle_.insert(j + 2, ss.str());
        }
        
        j = pos - 1;
      }
    }
  }

  Node* wrapper = new Node(state.name_ + "-wrapper");
  GeometryNode* root = Node::CreateMeshNode(state.name_ + "-root", "data/img/tree_bark.jpg");
  state.current_node_ = root;
  wrapper->AddChild(root);

  for (unsigned i = 0; i < state.turtle_.size(); i++) {
    if (eval.find(state.turtle_[i]) == eval.end()) {
      continue;
    }

    double parameter = 0.0;
    size_t pos = string::npos;
    if (state.turtle_[i + 1] == '(') {
      pos = state.turtle_.find(')', i + 1);
      if (pos == string::npos) {
        stringstream ss;
        ss << "Parse error (pos: " << (i + 1) << "): Unmatched '(' in turtle!";
        ERROR(ss.str());
        delete root;
        return NULL;
      }

      stringstream ss(state.turtle_.substr(i + 2, pos - i - 2));
      ss >> parameter;
    }

    EvalCallback callback = eval[state.turtle_[i]];
    if (callback) {
      callback(state, parameter);
    }

    if (pos != string::npos) {
      i = pos;
    }
  }

  return wrapper;
}

Node* LSystem::GenerateAlgae(string name) {
  return NULL;
}

Node* LSystem::GenerateGrass(string name) {
  ExpandRules expand;
  expand['F'] = ExpandPair("FF", NULL);
  expand['X'] = ExpandPair("F-[[X]+X]+F[+FX]-X", NULL);

  EvalRules eval;

  unsigned iterations = 6;

  TurtleState state("X", iterations, 25.0, 5, name);
  return Generate(expand, eval, state);
}

Node* LSystem::GenerateFlower(string name) {
  return NULL;
}

void drawLeaf(LSystem::TurtleState&, double) {
  //
}

void changeColour(LSystem::TurtleState&, double) {
  //
}

Node* LSystem::GenerateBush(string name) {
  ExpandRules expand;
  expand['A'] = ExpandPair("[&FL!A]<<<<<'[&FL!A]<<<<<<<'[&FL!A]", NULL);
  expand['F'] = ExpandPair("S<<<<<F", NULL);
  expand['S'] = ExpandPair("FL", NULL);
  expand['L'] = ExpandPair("['''^^-f+f+f++++++f+f+f]", NULL);

  EvalRules eval;
  eval['f'] = drawLeaf;
  eval['\''] = changeColour;

  unsigned iterations = 6;
  TurtleState state("A", iterations, 22.5, 5, name);
  return Generate(expand, eval, state);
}

/**
  * Tree generation
  **/

struct TreeData {
  double diverge_angle1;
  double diverge_angle2;
  double branching_angle;
  double elongation_rate;
  double width_rate;
};

double treeLengthCallback(LSystem::TurtleState& state, double value) {
  TreeData* data = (TreeData*)state.data_;
  return data->elongation_rate * value;
}

double treeWidthCallback(LSystem::TurtleState& state, double value) {
  TreeData* data = (TreeData*)state.data_;
  return data->width_rate * value;
}

Node* LSystem::GenerateTree(string name) {
  ExpandRules expand;
  
  TreeData data = {94.74, 132.63, 18.95, 1.109, 1.732};
  //TreeData data = {112.50, 157.50, 22.50, 1.790, 1.732};

  stringstream a_ss;
  a_ss << "!(" << data.width_rate << ")F(50)[&(" << data.branching_angle
       << ")F(50)A]<(" << data.diverge_angle1 << ")[&(" << data.branching_angle
       << ")F(50)A]<(" << data.diverge_angle2 << ")[&(" << data.branching_angle
       << ")F(50)A]";
  expand['A'] = ExpandPair(a_ss.str(), NULL);
  expand['F'] = ExpandPair("F", treeLengthCallback);
  expand['!'] = ExpandPair("!", treeWidthCallback);

  EvalRules eval;

  unsigned iterations = 6;

  TurtleState state("!(1)F(200)<(45)A", iterations, 30.0,
                    pow(data.width_rate, iterations), name);
  state.data_ = &data;
  return Generate(expand, eval, state);
}

LSystem::TurtleState::TurtleState(string turtle, unsigned iterations,
                                  double angle, double width, string name)
    : name_(name)
    , node_count_(0)
    , turtle_(turtle)
    , iterations_(iterations)
    , angle_(angle)
    , width_(width)
    , width_end_(width)
    , length_(30)
    , current_node_(NULL)
    , data_(NULL) {
}

void yawLeft(LSystem::TurtleState& state, double angle) {
  FUNC_ENTER;
  
  if (angle == 0) {
    angle = state.angle_;
  }
  angle = angle * M_PI / 180;

  double c = cos(angle);
  double s = sin(angle);
  Matrix4x4 rotation(Vector4D(c, s, 0, 0), Vector4D(-s, c, 0, 0), Vector4D(0, 0, 1, 0), Vector4D(0, 0, 0, 1));

  state.rotation_ = state.rotation_ * rotation;
  FUNC_LEAVE;
}

void yawRight(LSystem::TurtleState& state, double angle) {
  FUNC_ENTER;

  if (angle == 0) {
    angle = state.angle_;
  }
  angle = angle * M_PI / 180;

  double c = cos(-angle);
  double s = sin(-angle);
  Matrix4x4 rotation(Vector4D(c, s, 0, 0), Vector4D(-s, c, 0, 0), Vector4D(0, 0, 1, 0), Vector4D(0, 0, 0, 1));

  state.rotation_ = state.rotation_ * rotation;
  FUNC_LEAVE;
}

void pitchDown(LSystem::TurtleState& state, double angle) {
  FUNC_ENTER;

  if (angle == 0) {
    angle = state.angle_;
  }
  angle = angle * M_PI / 180;

  double c = cos(angle);
  double s = sin(angle);
  Matrix4x4 rotation(Vector4D(1, 0, 0, 0), Vector4D(0, c, -s, 0), Vector4D(0, s, c, 0), Vector4D(0, 0, 0, 1));

  state.rotation_ = state.rotation_ * rotation;
  FUNC_LEAVE;
}

void pitchUp(LSystem::TurtleState& state, double angle) {
  FUNC_ENTER;

  if (angle == 0) {
    angle = state.angle_;
  }
  angle = angle * M_PI / 180;

  double c = cos(-angle);
  double s = sin(-angle);
  Matrix4x4 rotation(Vector4D(1, 0, 0, 0), Vector4D(0, c, -s, 0), Vector4D(0, s, c, 0), Vector4D(0, 0, 0, 1));

  state.rotation_ = state.rotation_ * rotation;
  FUNC_LEAVE;
}

void rollLeft(LSystem::TurtleState& state, double angle) {
  FUNC_ENTER;

  if (angle == 0) {
    angle = state.angle_;
  }
  angle = angle * M_PI / 180;

  double c = cos(angle);
  double s = sin(angle);
  Matrix4x4 rotation(Vector4D(c, 0, -s, 0), Vector4D(0, 1, 0, 0), Vector4D(s, 0, c, 0), Vector4D(0, 0, 0, 1));

  state.rotation_ = state.rotation_ * rotation;
  FUNC_LEAVE;
}

void rollRight(LSystem::TurtleState& state, double angle) {
  FUNC_ENTER;

  if (angle == 0) {
    angle = state.angle_;
  }
  angle = angle * M_PI / 180;

  double c = cos(-angle);
  double s = sin(-angle);
  Matrix4x4 rotation(Vector4D(c, 0, -s, 0), Vector4D(0, 1, 0, 0), Vector4D(s, 0, c, 0), Vector4D(0, 0, 0, 1));

  state.rotation_ = state.rotation_ * rotation;
  FUNC_LEAVE;
}

void pushNode(LSystem::TurtleState& state, double) {
  FUNC_ENTER;
  stringstream ss;
  ss << state.name_ << '-' << state.node_count_;
  GeometryNode* new_node = Node::CreateMeshNode(ss.str());
  state.node_count_++;

  Mesh* mesh = reinterpret_cast<Mesh*>(state.current_node_->GetPrimitive());
  new_node->Translate(mesh->GetEndPoint() - Point3D(0, 0, 0));

  state.node_stack_.push(state.current_node_);
  state.current_node_->AddChild(new_node);
  state.current_node_ = new_node;

  state.rotation_stack_.push(state.rotation_);
  state.width_stack_.push(state.width_);
  state.width_stack_.push(state.width_end_);
  FUNC_LEAVE;
}

void popNode(LSystem::TurtleState& state, double) {
  FUNC_ENTER;
  state.current_node_ = state.node_stack_.top();
  state.node_stack_.pop();

  state.rotation_ = state.rotation_stack_.top();
  state.rotation_stack_.pop();

  state.width_end_ = state.width_stack_.top();
  state.width_stack_.pop();
  state.width_ = state.width_stack_.top();
  state.width_stack_.pop();
  FUNC_LEAVE;
}

void decreaseWidth(LSystem::TurtleState& state, double width) {
  if (width != 0) {
    state.width_end_ = width;
  } else {
    state.width_end_ = state.width_ * 0.99;
  }
}

void drawForward(LSystem::TurtleState& state, double length) {
  FUNC_ENTER;
  Mesh* mesh = reinterpret_cast<Mesh*>(state.current_node_->GetPrimitive());

  if (length == 0) {
    length = state.length_;
  }
  mesh->Extend(length, state.rotation_, state.width_, state.width_end_);
  state.width_ = state.width_end_;

  FUNC_LEAVE;
}

