#include <iostream>
#include <do-verify/ptl.hpp>
#include <do-verify/MTLEngine.hpp>
#include <do-verify/interval_set.hpp>

using namespace do_verify;
using namespace db_interval_set;

int main() {
    ptl_parser p;

    std::string formula = "historically(({r} && !{q} && once {q}) -> ({p} since[3:10] {q}))";

    std::cout << "Parsing: " << formula << std::endl;

    try {
        db_interval_set::IntervalSetHolder holder;
        auto nodes = p.parse_dense(formula, holder);
        std::cout << "Parsed successfully! " << nodes.size() << " nodes created." << std::endl;

        // Show proposition map
        auto &propMap = p.get_proposition_map();
        std::cout << "\nProposition map:" << std::endl;
        for (auto &[name, idx] : propMap) {
            std::cout << "  " << name << " -> input index " << idx << std::endl;
        }
        std::cout << std::endl;

        for (size_t i = 0; i < nodes.size(); i++) {
            std::cout << "Node " << i << ": type=";
            switch (nodes[i].type) {
                case NodeType::PROPOSITION: std::cout << "PROPOSITION"; break;
                case NodeType::AND:         std::cout << "AND"; break;
                case NodeType::OR:          std::cout << "OR"; break;
                case NodeType::NOT:         std::cout << "NOT"; break;
                case NodeType::IMPLIES:     std::cout << "IMPLIES"; break;
                case NodeType::EVENTUALLY:  std::cout << "EVENTUALLY"; break;
                case NodeType::ALWAYS:      std::cout << "ALWAYS"; break;
                case NodeType::SINCE:       std::cout << "SINCE"; break;
                case NodeType::TEST:        std::cout << "TEST"; break;
            }
            std::cout << " left=" << nodes[i].leftOperandIndex
                      << " right=" << nodes[i].rightOperandIndex
                      << " a=" << nodes[i].a
                      << " b=" << nodes[i].b
                      << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
