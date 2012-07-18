#ifndef __L_SYSTEM_H__
#define __L_SYSTEM_H__

#include <map>
#include <stack>
#include <string>
#include <utility>

#include "Algebra.h"
#include "Node.h"

class LSystem {
 public:
  struct TurtleState {
    TurtleState(std::string turtle = "", unsigned iterations = 0,
                double angle = 0.0, double width = 1.0, std::string name = "");

    std::string name_;
    unsigned node_count_;

    std::string turtle_;
    unsigned iterations_;

    Matrix4x4 rotation_;

    double angle_;
    double width_;
    double width_end_;
    double length_;

    GeometryNode* current_node_;
    std::stack<Matrix4x4> rotation_stack_;
    std::stack<GeometryNode*> node_stack_;
    std::stack<double> width_stack_;

    void* data_;
  };

  typedef double (*ExpandCallback)(TurtleState&, double);
  typedef std::pair<std::string, ExpandCallback> ExpandPair;
  typedef std::map<char, ExpandPair> ExpandRules;
  
  typedef void (*EvalCallback)(TurtleState&, double);
  typedef std::map<char, EvalCallback> EvalRules;

  static void InitializeCommonRules();

  static Node* Generate(ExpandRules, EvalRules rules, TurtleState state);

  static Node* GenerateAlgae(std::string name = "");
  static Node* GenerateGrass(std::string name = "");
  static Node* GenerateFlower(std::string name = "");
  static Node* GenerateBush(std::string name = "");
  static Node* GenerateTree(std::string name = "");

 private:
  static EvalRules common_eval_;
};

#endif

