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
#include <map>
#include <any>

#define PEGLIB_USE_STD_ANY 1
#include <peglib.h>
#include "ptl_grammar.hpp"

#include "MTLEngine.hpp"
#include "interval_set.hpp"

namespace do_verify {



struct ptl_parser : ptl_grammar{


  peg::parser parser;
  std::vector<ParsedNode> result_nodes;
  std::map<std::string, unsigned int> proposition_map;

  explicit ptl_parser() {

    parser = peg::parser(grammar);
    parser.set_logger([](size_t line, size_t col, const std::string &msg) {
      std::cerr << line << ":" << col << ": " << msg << std::endl;
    });

    parser["NotExpr"] = [&](const peg::SemanticValues &sv) {
      int childIndex = std::any_cast<int>(sv[0]);
      ParsedNode node;
      node.type = NodeType::NOT;
      node.leftOperandIndex = 0;
      node.rightOperandIndex = childIndex;
      node.a = 0;
      node.b = 0;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["Implicative"] = [&](const peg::SemanticValues &sv) {
      if (sv.size() > 1) {
        ParsedNode node;
        node.type = NodeType::IMPLIES;
        node.leftOperandIndex = std::any_cast<int>(sv[0]);
        node.rightOperandIndex = std::any_cast<int>(sv[1]);
        node.a = 0;
        node.b = 0;
        result_nodes.push_back(node);
        return static_cast<int>(result_nodes.size() - 1);
      } else {
        return std::any_cast<int>(sv[0]);
      }
    };

    parser["Disjunctive"] = [&](const peg::SemanticValues &sv) {
      if (sv.size() > 1) {
        int left = std::any_cast<int>(sv[0]);
        for (size_t i = 1; i < sv.size(); i++) {
          int right = std::any_cast<int>(sv[i]);
          ParsedNode node;
          node.type = NodeType::OR;
          node.leftOperandIndex = left;
          node.rightOperandIndex = right;
          node.a = 0;
          node.b = 0;
          result_nodes.push_back(node);
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
          ParsedNode node;
          node.type = NodeType::AND;
          node.leftOperandIndex = left;
          node.rightOperandIndex = right;
          node.a = 0;
          node.b = 0;
          result_nodes.push_back(node);
          left = static_cast<int>(result_nodes.size() - 1);
        }
        return left;
      } else {
        return std::any_cast<int>(sv[0]);
      }
    };

    parser["OnceExpr"] = [&](const peg::SemanticValues &sv) {
      int childIndex = std::any_cast<int>(sv[0]);
      ParsedNode node;
      node.type = NodeType::EVENTUALLY;
      node.leftOperandIndex = 0;
      node.rightOperandIndex = childIndex;
      node.a = 0;
      node.b = B_INFINITY;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["TimedOnceExpr"] = [&](const peg::SemanticValues &sv) {
      std::pair<int, int> bound = std::any_cast<std::pair<int, int>>(sv[0]);
      int childIndex = std::any_cast<int>(sv[1]);
      ParsedNode node;
      node.type = NodeType::EVENTUALLY;
      node.leftOperandIndex = 0;
      node.rightOperandIndex = childIndex;
      node.a = bound.first;
      node.b = bound.second;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["HistExpr"] = [&](const peg::SemanticValues &sv) {
      int childIndex = std::any_cast<int>(sv[0]);
      ParsedNode node;
      node.type = NodeType::ALWAYS;
      node.leftOperandIndex = 0;
      node.rightOperandIndex = childIndex;
      node.a = 0;
      node.b = B_INFINITY;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["TimedHistExpr"] = [&](const peg::SemanticValues &sv) {
      std::pair<int, int> bound = std::any_cast<std::pair<int, int>>(sv[0]);
      int childIndex = std::any_cast<int>(sv[1]);
      ParsedNode node;
      node.type = NodeType::ALWAYS;
      node.leftOperandIndex = 0;
      node.rightOperandIndex = childIndex;
      node.a = bound.first;
      node.b = bound.second;
      result_nodes.push_back(node);
      return static_cast<int>(result_nodes.size() - 1);
    };

    parser["SinceExpr"] = [&](const peg::SemanticValues &sv) {
      if (sv.size() == 3) {
        int leftIndex = std::any_cast<int>(sv[0]);
        std::pair<int, int> bound = std::any_cast<std::pair<int, int>>(sv[1]);
        int rightIndex = std::any_cast<int>(sv[2]);
        ParsedNode node;
        node.type = NodeType::SINCE;
        node.leftOperandIndex = leftIndex;
        node.rightOperandIndex = rightIndex;
        node.a = bound.first;
        node.b = bound.second;
        result_nodes.push_back(node);
        return static_cast<int>(result_nodes.size() - 1);
      } else if (sv.size() == 2) {
        int leftIndex = std::any_cast<int>(sv[0]);
        int rightIndex = std::any_cast<int>(sv[1]);
        ParsedNode node;
        node.type = NodeType::SINCE;
        node.leftOperandIndex = leftIndex;
        node.rightOperandIndex = rightIndex;
        node.a = 0;
        node.b = B_INFINITY;
        result_nodes.push_back(node);
        return static_cast<int>(result_nodes.size() - 1);
      } else {
        return std::any_cast<int>(sv[0]);
      }
    };

    parser["Atom"] = [&](const peg::SemanticValues &sv) {
      std::string name = std::any_cast<std::string>(sv[0]);
      if (proposition_map.find(name) == proposition_map.end()) {
        ParsedNode node;
        node.type = NodeType::PROPOSITION;
        node.leftOperandIndex = static_cast<int>(proposition_map.size());  // Use this as the input index for the proposition
        node.rightOperandIndex = 0;
        node.a = 0;
        node.b = 0;
        result_nodes.push_back(node);
        proposition_map[name] = static_cast<unsigned int>(result_nodes.size() - 1);
        return static_cast<int>(result_nodes.size() - 1);
      }
      else {
        return static_cast<int>(proposition_map[name]);
      }
      
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

    parser.enable_packrat_parsing();
  }

  std::vector<ParsedNode> parse(const std::string &pattern) {
    result_nodes.clear();
    proposition_map.clear();
    bool ok = parser.parse(pattern.c_str());
    if (!ok) {
      throw std::runtime_error("Failed to parse pattern: " + pattern);
    }
    return result_nodes;
  }

  std::vector<DiscreteNode> parse_discrete(const std::string &pattern, db_interval_set::IntervalSetHolder &holder) {
    parse(pattern);
    std::vector<DiscreteNode> nodes;
    nodes.reserve(result_nodes.size());
    for (auto &pn : result_nodes) {
      DiscreteNode dn;
      dn.type = pn.type;
      dn.leftOperandIndex = pn.leftOperandIndex;
      dn.rightOperandIndex = pn.rightOperandIndex;
      dn.a = pn.a;
      dn.b = pn.b;
      dn.state = db_interval_set::empty(holder);
      dn.output = false;
      nodes.push_back(dn);
    }
    return nodes;
  }

  std::vector<DenseNode> parse_dense(const std::string &pattern, db_interval_set::IntervalSetHolder &holder) {
    parse(pattern);
    std::vector<DenseNode> nodes;
    nodes.reserve(result_nodes.size());
    for (auto &pn : result_nodes) {
      DenseNode dn;
      dn.type = pn.type;
      dn.leftOperandIndex = pn.leftOperandIndex;
      dn.rightOperandIndex = pn.rightOperandIndex;
      dn.a = pn.a;
      dn.b = pn.b;
      dn.state = db_interval_set::empty(holder);
      dn.output = db_interval_set::empty(holder);
      nodes.push_back(dn);
    }
    return nodes;
  }

  const std::map<std::string, unsigned int>& get_proposition_map() const {
    return proposition_map;
  }

};

} // namespace do_verify
