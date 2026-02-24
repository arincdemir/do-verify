/*
 * Copyright (c) 2019-2023 Dogan Ulus
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
 
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <any>

#define PEGLIB_USE_STD_ANY 1
#include <peglib.h>
#include "ptl_grammar.hpp"

#include "MTLEngine.hpp"
#include "interval_set.hpp"

namespace do_verify {



struct ptl_parser : ptl_grammar{


  peg::parser parser;
  std::vector<DiscreteNode> result_nodes;
  bool dense = false;
  db_interval_set::IntervalSetHolder holder;

  explicit ptl_parser() {

    parser = peg::parser(grammar);
    parser.set_logger([](size_t line, size_t col, const std::string &msg) {
      std::cerr << line << ":" << col << ": " << msg << std::endl;
    });

    parser["NotExpr"] = [&](const peg::SemanticValues &sv) {
      int childIndex = std::any_cast<int>(sv[0]);
      DiscreteNode notNode;
      notNode.type = NodeType::NOT;
      notNode.leftOperandIndex = childIndex;
      notNode.rightOperandIndex = 0;
      notNode.a = 0;
      notNode.b = 0;
      notNode.state = db_interval_set::empty(holder);
      notNode.output = false;
      result_nodes.push_back(notNode);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["Implicative"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      if (sv.size() > 1) {
        DiscreteNode implicationNode;
        implicationNode.type = NodeType::IMPLIES;
        implicationNode.leftOperandIndex = std::any_cast<int>(sv[0]);
        implicationNode.rightOperandIndex = std::any_cast<int>(sv[1]);
        implicationNode.a = 0;
        implicationNode.b = 0;
        implicationNode.state = db_interval_set::empty(holder);
        implicationNode.output = false;
        result_nodes.push_back(implicationNode);
        return static_cast<int>(result_nodes.size() - 1);
      } else {
        return std::any_cast<int>(sv[0]);
      }
    };

    parser["Disjunctive"] = [&](const peg::SemanticValues &sv) {
      if (sv.size() > 1) {
        // Chain binary OR nodes: (a | b | c) -> OR(OR(a, b), c)
        int left = std::any_cast<int>(sv[0]);
        for (size_t i = 1; i < sv.size(); i++) {
          int right = std::any_cast<int>(sv[i]);
          DiscreteNode orNode;
          orNode.type = NodeType::OR;
          orNode.leftOperandIndex = left;
          orNode.rightOperandIndex = right;
          orNode.a = 0;
          orNode.b = 0;
          orNode.state = db_interval_set::empty(holder);
          orNode.output = false;
          result_nodes.push_back(orNode);
          left = static_cast<int>(result_nodes.size() - 1);
        }
        return left;
      } else {
        return std::any_cast<int>(sv[0]);
      }
    };

    parser["Conjunctive"] = [&](const peg::SemanticValues &sv) {
      if (sv.size() > 1) {
        int left = std::any_cast<int>(sv[0]);
        for (size_t i = 1; i < sv.size(); i++) {
          int right = std::any_cast<int>(sv[i]);
          DiscreteNode andNode;
          andNode.type = NodeType::AND;
          andNode.leftOperandIndex = left;
          andNode.rightOperandIndex = right;
          andNode.a = 0;
          andNode.b = 0;
          andNode.state = db_interval_set::empty(holder);
          andNode.output = false;
          result_nodes.push_back(andNode);
          left = static_cast<int>(result_nodes.size() - 1);
        }
        return left;
      } else {
        return std::any_cast<int>(sv[0]);
      }
    };

    parser["OnceExpr"] = [&](const peg::SemanticValues &sv) {
      int childIndex = std::any_cast<int>(sv[0]);

      DiscreteNode node;
      node.type = NodeType::EVENTUALLY;
      node.leftOperandIndex = childIndex;
      node.rightOperandIndex = 0;
      node.a = 0;
      node.b = B_INFINITY;
      node.state = db_interval_set::empty(holder);
      node.output = false;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["TimedOnceExpr"] = [&](const peg::SemanticValues &sv) {
      std::pair<int, int> bound = std::any_cast<std::pair<int, int>>(sv[0]);
      int childIndex = std::any_cast<int>(sv[1]);

      DiscreteNode node;
      node.type = NodeType::EVENTUALLY;
      node.leftOperandIndex = childIndex;
      node.rightOperandIndex = 0;
      node.a = bound.first;
      node.b = bound.second;
      node.state = db_interval_set::empty(holder);
      node.output = false;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["HistExpr"] = [&](const peg::SemanticValues &sv) {
      int childIndex = std::any_cast<int>(sv[0]);

      DiscreteNode node;
      node.type = NodeType::ALWAYS;
      node.leftOperandIndex = childIndex;
      node.rightOperandIndex = 0;
      node.a = 0;
      node.b = B_INFINITY;
      node.state = db_interval_set::empty(holder);
      node.output = false;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["TimedHistExpr"] = [&](const peg::SemanticValues &sv) {
      std::pair<int, int> bound = std::any_cast<std::pair<int, int>>(sv[0]);
      int childIndex = std::any_cast<int>(sv[1]);

      DiscreteNode node;
      node.type = NodeType::ALWAYS;
      node.leftOperandIndex = childIndex;
      node.rightOperandIndex = 0;
      node.a = bound.first;
      node.b = bound.second;
      node.state = db_interval_set::empty(holder);
      node.output = false;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["SinceExpr"] = [&](const peg::SemanticValues &sv) {
      if (sv.size() == 3) {
        int leftIndex = std::any_cast<int>(sv[0]);
        std::pair<int, int> bound = std::any_cast<std::pair<int, int>>(sv[1]);
        int rightIndex = std::any_cast<int>(sv[2]);

        DiscreteNode node;
        node.type = NodeType::SINCE;
        node.leftOperandIndex = leftIndex;
        node.rightOperandIndex = rightIndex;
        node.a = bound.first;
        node.b = bound.second;
        node.state = db_interval_set::empty(holder);
        node.output = false;
        result_nodes.push_back(node);
        return static_cast<int>(result_nodes.size() - 1);
      } else if (sv.size() == 2) {
        int leftIndex = std::any_cast<int>(sv[0]);
        int rightIndex = std::any_cast<int>(sv[1]);

        DiscreteNode node;
        node.type = NodeType::SINCE;
        node.leftOperandIndex = leftIndex;
        node.rightOperandIndex = rightIndex;
        node.a = 0;
        node.b = B_INFINITY;
        node.state = db_interval_set::empty(holder);
        node.output = false;
        result_nodes.push_back(node);
        return static_cast<int>(result_nodes.size() - 1);
      } else {
        return std::any_cast<int>(sv[0]);
      }
    };

    parser["Atom"] = [&](const peg::SemanticValues &sv) {
      std::string name = std::any_cast<std::string>(sv[0]);
      // TODO: Find a way to map the proposition names to indices of nodes
      DiscreteNode node;
      node.type = NodeType::PROPOSITION;
      node.leftOperandIndex = 0;
      node.rightOperandIndex = 0;
      node.a = 0;
      node.b = 0;
      node.state = db_interval_set::empty(holder);
      node.output = false;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["FullBound"] = [](const peg::SemanticValues &sv) {
      int l = std::stoi(std::any_cast<std::string>(sv[0]));
      int u = std::stoi(std::any_cast<std::string>(sv[1]));
      return std::make_pair(l, u);
    };

    parser["LowerBound"] = [](const peg::SemanticValues &sv) {
      int l = std::stoi(std::any_cast<std::string>(sv[0]));
      return std::make_pair(l, B_INFINITY);
    };

    parser["UpperBound"] = [](const peg::SemanticValues &sv) {
      int u = std::stoi(std::any_cast<std::string>(sv[0]));
      return std::make_pair(0, u);
    };

    parser["Name"] = [](const peg::SemanticValues &sv) { return std::string(sv.token()); };
    parser["Number"] = [](const peg::SemanticValues &sv) { return std::string(sv.token()); };

    parser.enable_packrat_parsing(); // Enable packrat parsing.
  }

  std::vector<DiscreteNode> parse(const std::string &pattern, const bool dense, db_interval_set::IntervalSetHolder &holder) {
    result_nodes.clear();
    this->dense = dense;
    this->holder = holder;
    bool ok = parser.parse(pattern.c_str());
    if (!ok) {
      throw std::runtime_error("Failed to parse pattern: " + pattern);
    }
    return result_nodes;
  }
  
  std::shared_ptr<std::vector<DiscreteNode>> make_shared(const std::string &pattern, bool dense, db_interval_set::IntervalSetHolder &holder) {
    return std::make_shared<std::vector<DiscreteNode>>(parse(pattern, dense, holder));
  }

};

} // namespace do_verify
