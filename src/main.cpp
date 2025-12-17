#include <array>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <argp.h>
#include <sys/types.h>

#include <do-verify/binary_row_reader.hpp>
#include <do-verify/MTLEngine.hpp>

using namespace db_interval_set;
using namespace do_verify;

// argp option keys
enum RYBINX_OPTS : uint8_t
{
    OPT_DENSE = 'v',
    OPT_DISCRETE = 'x'
};

const char *argp_program_version = "do-verify-bin 0.1.0";
const char *argp_program_bug_address = "Arinc Demir <github.com/arincdemir>";
static const char *doc = "Do-verify (Reelay) on Binary Row format";
static const char *args_doc = "SPEC FILE";

struct arguments
{
    char *spec;
    char *file;
    bool dense = false;
    bool discrete = false;
};

static std::array<struct argp_option, 12> options = {
    {{"dense", OPT_DENSE, nullptr, 0, "Use dense time model (default)", 0},
     {"discrete", OPT_DISCRETE, nullptr, 0, "Use discrete time model", 0},
     {nullptr}}};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    auto *arguments = (struct arguments *)state->input;
    switch (key)
    {
    case OPT_DENSE:
        arguments->dense = true;
        break;
    case OPT_DISCRETE:
        arguments->discrete = true;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num == 0)
        {
            arguments->spec = arg;
        }
        else
        {
            arguments->file = arg;
        }
        break;
    case ARGP_KEY_END:
        if (state->arg_num < 2)
        {
            argp_usage(state);
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
static struct argp argp = {options.data(), parse_opt, args_doc, doc};

void discrete_case(arguments arguments, std::vector<binary_row_reader::TimescalesInput> allInputs);
void dense_case(arguments arguments, std::vector<binary_row_reader::TimescalesInput> allInputs);

int main(int argc, char **argv)
{
    struct arguments arguments;
    argp_parse(&argp, argc, argv, 0, nullptr, &arguments);

    // Choices
    bool use_discrete = arguments.discrete;
    bool use_dense = arguments.dense;

    // Apply defaults
    if (!use_discrete && !use_dense)
    {
        use_discrete = true;
    }

    std::ifstream input(arguments.file, std::ios::binary);
    if (!input)
    {
        std::cerr << "Error opening file: " << arguments.file << std::endl;
        return 1;
    }

    const auto &allInputs = binary_row_reader::readInputFile(arguments.file);

    if (use_discrete)
    {
        discrete_case(arguments, allInputs);
    }
    else
    {
        dense_case(arguments, allInputs);
    }
}

void discrete_case(arguments arguments, std::vector<binary_row_reader::TimescalesInput> allInputs)
{
    // AbsentAQ: historically((once[:N]{q}) -> ((not{p}) since {q}))
    if (strcmp(arguments.spec, "historically((once[:10]{q}) -> ((not{p}) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 10};
        DiscreteNode notNode{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since{empty(holder), false, NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DiscreteNode implies{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, once, notNode, since, implies, always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically((once[:100]{q}) -> ((not{p}) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 100};
        DiscreteNode notNode{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since{empty(holder), false, NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DiscreteNode implies{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, once, notNode, since, implies, always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically((once[:1000]{q}) -> ((not{p}) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 1000};
        DiscreteNode notNode{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since{empty(holder), false, NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DiscreteNode implies{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, once, notNode, since, implies, always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AbsentBQR: historically({r} && !{q} && once{q}) -> ((not{p}) since[A:B] {q})
    else if (strcmp(arguments.spec, "historically({r} && !{q} && once{q}) -> ((not{p}) since[3:10] {q})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 7, 0, 3, 10};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 8, 0, 0};
        DiscreteNode always_node{empty(holder), false, NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} && !{q} && once{q}) -> ((not{p}) since[30:100] {q})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 7, 0, 30, 100};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 8, 0, 0};
        DiscreteNode always_node{empty(holder), false, NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} && !{q} && once{q}) -> ((not{p}) since[300:1000] {q})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 7, 0, 300, 1000};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 8, 0, 0};
        DiscreteNode always_node{empty(holder), false, NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AbsentBR: historically({r} -> (historically[:N](not{p})))
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:10](not{p})))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode inner_always{empty(holder), false, NodeType::ALWAYS, 0, 3, 0, 10};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_p, inner_always, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:100](not{p})))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode inner_always{empty(holder), false, NodeType::ALWAYS, 0, 3, 0, 100};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_p, inner_always, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:1000](not{p})))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode inner_always{empty(holder), false, NodeType::ALWAYS, 0, 3, 0, 1000};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_p, inner_always, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AlwaysAQ: historically((once[:N]{q}) -> ({p} since {q}))
    else if (strcmp(arguments.spec, "historically((once[:10]{q}) -> ({p} since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 10};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 1, 0, 0, B_INFINITY};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 3, 4, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, once_q, since_node, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically((once[:100]{q}) -> ({p} since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 100};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 1, 0, 0, B_INFINITY};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 3, 4, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, once_q, since_node, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically((once[:1000]{q}) -> ({p} since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 1000};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 1, 0, 0, B_INFINITY};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 3, 4, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, once_q, since_node, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AlwaysBQR: historically(({r} && !{q} && once{q}) -> ({p} since[A:B] {q}))
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ({p} since[3:10] {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 1, 0, 3, 10};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 8, 0, 0};
        DiscreteNode always_node{empty(holder), false, NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ({p} since[30:100] {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 1, 0, 30, 100};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 8, 0, 0};
        DiscreteNode always_node{empty(holder), false, NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ({p} since[300:1000] {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 1, 0, 300, 1000};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 8, 0, 0};
        DiscreteNode always_node{empty(holder), false, NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AlwaysBR: historically({r} -> (historically[:N]{p}))
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:10]{p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode inner_always{empty(holder), false, NodeType::ALWAYS, 0, 1, 0, 10};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 2, 3, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 4, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, inner_always, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:100]{p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode inner_always{empty(holder), false, NodeType::ALWAYS, 0, 1, 0, 100};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 2, 3, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 4, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, inner_always, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:1000]{p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode inner_always{empty(holder), false, NodeType::ALWAYS, 0, 1, 0, 1000};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 2, 3, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 4, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, inner_always, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // RecurBQR: historically(({r} && !{q} && once{q}) -> ((once[:N]({p} or {q})) since {q}))
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ((once[:10]({p} or {q})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode p_or_q{empty(holder), false, NodeType::OR, 1, 0, 0, 0};
        DiscreteNode once_p_or_q{empty(holder), false, NodeType::EVENTUALLY, 0, 7, 0, 10};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 8, 0, 0, B_INFINITY};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 9, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 10, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, p_or_q, once_p_or_q, since_node, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ((once[:100]({p} or {q})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode p_or_q{empty(holder), false, NodeType::OR, 1, 0, 0, 0};
        DiscreteNode once_p_or_q{empty(holder), false, NodeType::EVENTUALLY, 0, 7, 0, 100};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 8, 0, 0, B_INFINITY};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 9, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 10, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, p_or_q, once_p_or_q, since_node, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ((once[:1000]({p} or {q})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};
        DiscreteNode p_or_q{empty(holder), false, NodeType::OR, 1, 0, 0, 0};
        DiscreteNode once_p_or_q{empty(holder), false, NodeType::EVENTUALLY, 0, 7, 0, 1000};
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 8, 0, 0, B_INFINITY};
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 9, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 10, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, p_or_q, once_p_or_q, since_node, implies_node, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // RecurGLB: historically(once[:N]{p})
    else if (strcmp(arguments.spec, "historically(once[:10]{p})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 10};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{p, once, always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(once[:100]{p})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 100};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{p, once, always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(once[:1000]{p})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 1000};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{p, once, always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // RespondBQR: historically(({r} && !{q} && once{q}) -> ( (({s} -> once[A:B]{p}) and not((not {s}) since[B:] {p})) since {q}))
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ( (({s} -> once[3:10]{p}) and not((not {s}) since[10:] {p})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode s{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and_A1{empty(holder), false, NodeType::AND, 3, 4, 0, 0};
        DiscreteNode and_A2{empty(holder), false, NodeType::AND, 6, 5, 0, 0};
        DiscreteNode once_p{empty(holder), false, NodeType::EVENTUALLY, 0, 1, 3, 10};
        DiscreteNode implies_D{empty(holder), false, NodeType::IMPLIES, 2, 8, 0, 0};
        DiscreteNode not_s{empty(holder), false, NodeType::NOT, 0, 2, 0, 0};
        DiscreteNode since_F{empty(holder), false, NodeType::SINCE, 10, 1, 10, B_INFINITY};
        DiscreteNode not_F{empty(holder), false, NodeType::NOT, 0, 11, 0, 0};
        DiscreteNode and_C{empty(holder), false, NodeType::AND, 9, 12, 0, 0};
        DiscreteNode since_B{empty(holder), false, NodeType::SINCE, 13, 0, 0, B_INFINITY};
        DiscreteNode implies_main{empty(holder), false, NodeType::IMPLIES, 7, 14, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 15, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, s, r, not_q, once_q, and_A1, and_A2, once_p, implies_D, not_s, since_F, not_F, and_C, since_B, implies_main, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].s, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ( (({s} -> once[30:100]{p}) and not((not {s}) since[100:] {p})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode s{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and_A1{empty(holder), false, NodeType::AND, 3, 4, 0, 0};
        DiscreteNode and_A2{empty(holder), false, NodeType::AND, 6, 5, 0, 0};
        DiscreteNode once_p{empty(holder), false, NodeType::EVENTUALLY, 0, 1, 30, 100};
        DiscreteNode implies_D{empty(holder), false, NodeType::IMPLIES, 2, 8, 0, 0};
        DiscreteNode not_s{empty(holder), false, NodeType::NOT, 0, 2, 0, 0};
        DiscreteNode since_F{empty(holder), false, NodeType::SINCE, 10, 1, 100, B_INFINITY};
        DiscreteNode not_F{empty(holder), false, NodeType::NOT, 0, 11, 0, 0};
        DiscreteNode and_C{empty(holder), false, NodeType::AND, 9, 12, 0, 0};
        DiscreteNode since_B{empty(holder), false, NodeType::SINCE, 13, 0, 0, B_INFINITY};
        DiscreteNode implies_main{empty(holder), false, NodeType::IMPLIES, 7, 14, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 15, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, s, r, not_q, once_q, and_A1, and_A2, once_p, implies_D, not_s, since_F, not_F, and_C, since_B, implies_main, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].s, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ( (({s} -> once[300:1000]{p}) and not((not {s}) since[1000:] {p})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode s{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DiscreteNode and_A1{empty(holder), false, NodeType::AND, 3, 4, 0, 0};
        DiscreteNode and_A2{empty(holder), false, NodeType::AND, 6, 5, 0, 0};
        DiscreteNode once_p{empty(holder), false, NodeType::EVENTUALLY, 0, 1, 300, 1000};
        DiscreteNode implies_D{empty(holder), false, NodeType::IMPLIES, 2, 8, 0, 0};
        DiscreteNode not_s{empty(holder), false, NodeType::NOT, 0, 2, 0, 0};
        DiscreteNode since_F{empty(holder), false, NodeType::SINCE, 10, 1, 1000, B_INFINITY};
        DiscreteNode not_F{empty(holder), false, NodeType::NOT, 0, 11, 0, 0};
        DiscreteNode and_C{empty(holder), false, NodeType::AND, 9, 12, 0, 0};
        DiscreteNode since_B{empty(holder), false, NodeType::SINCE, 13, 0, 0, B_INFINITY};
        DiscreteNode implies_main{empty(holder), false, NodeType::IMPLIES, 7, 14, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 15, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, s, r, not_q, once_q, and_A1, and_A2, once_p, implies_D, not_s, since_F, not_F, and_C, since_B, implies_main, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].s, allInputs[i].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // RespondGLB: historically(({s} -> once[A:B]{p}) and not((not {s}) since[B:] {p}))
    else if (strcmp(arguments.spec, "historically(({s} -> once[3:10]{p}) and not((not {s}) since[10:] {p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode s{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once_p{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 3, 10};
        DiscreteNode implies_D{empty(holder), false, NodeType::IMPLIES, 1, 2, 0, 0};
        DiscreteNode not_s{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_F{empty(holder), false, NodeType::SINCE, 4, 0, 10, B_INFINITY};
        DiscreteNode not_F{empty(holder), false, NodeType::NOT, 0, 5, 0, 0};
        DiscreteNode and_C{empty(holder), false, NodeType::AND, 3, 6, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 7, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{p, s, once_p, implies_D, not_s, since_F, not_F, and_C, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].p, allInputs[i].s});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({s} -> once[30:100]{p}) and not((not {s}) since[100:] {p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode s{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once_p{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 30, 100};
        DiscreteNode implies_D{empty(holder), false, NodeType::IMPLIES, 1, 2, 0, 0};
        DiscreteNode not_s{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_F{empty(holder), false, NodeType::SINCE, 4, 0, 100, B_INFINITY};
        DiscreteNode not_F{empty(holder), false, NodeType::NOT, 0, 5, 0, 0};
        DiscreteNode and_C{empty(holder), false, NodeType::AND, 3, 6, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 7, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{p, s, once_p, implies_D, not_s, since_F, not_F, and_C, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].p, allInputs[i].s});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({s} -> once[300:1000]{p}) and not((not {s}) since[1000:] {p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode s{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once_p{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 300, 1000};
        DiscreteNode implies_D{empty(holder), false, NodeType::IMPLIES, 1, 2, 0, 0};
        DiscreteNode not_s{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since_F{empty(holder), false, NodeType::SINCE, 4, 0, 1000, B_INFINITY};
        DiscreteNode not_F{empty(holder), false, NodeType::NOT, 0, 5, 0, 0};
        DiscreteNode and_C{empty(holder), false, NodeType::AND, 3, 6, 0, 0};
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 7, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{p, s, once_p, implies_D, not_s, since_F, not_F, and_C, root_always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].p, allInputs[i].s});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else
    {
        std::cout << "Error: Can't find code for spec: " << arguments.spec << std::endl;
    }
}


void dense_case(arguments arguments, std::vector<binary_row_reader::TimescalesInput> allInputs)
{
    // AbsentAQ: historically((once[:N]{q}) -> ((not{p}) since {q}))
    if (strcmp(arguments.spec, "historically((once[:10]{q}) -> ((not{p}) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 10};
        DenseNode notNode{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since{empty(holder), empty(holder), NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DenseNode implies{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0};
        DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, once, notNode, since, implies, always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically((once[:100]{q}) -> ((not{p}) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 100};
        DenseNode notNode{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since{empty(holder), empty(holder), NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DenseNode implies{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0};
        DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, once, notNode, since, implies, always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically((once[:1000]{q}) -> ((not{p}) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 1000};
        DenseNode notNode{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since{empty(holder), empty(holder), NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DenseNode implies{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0};
        DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, once, notNode, since, implies, always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AbsentBQR: historically({r} && !{q} && once{q}) -> ((not{p}) since[A:B] {q})
    else if (strcmp(arguments.spec, "historically({r} && !{q} && once{q}) -> ((not{p}) since[3:10] {q})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 7, 0, 3, 10};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0};
        DenseNode always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} && !{q} && once{q}) -> ((not{p}) since[30:100] {q})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 7, 0, 30, 100};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0};
        DenseNode always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} && !{q} && once{q}) -> ((not{p}) since[300:1000] {q})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 7, 0, 300, 1000};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0};
        DenseNode always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AbsentBR: historically({r} -> (historically[:N](not{p})))
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:10](not{p})))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 3, 0, 10};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_p, inner_always, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:100](not{p})))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 3, 0, 100};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_p, inner_always, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:1000](not{p})))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 3, 0, 1000};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_p, inner_always, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AlwaysAQ: historically((once[:N]{q}) -> ({p} since {q}))
    else if (strcmp(arguments.spec, "historically((once[:10]{q}) -> ({p} since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 10};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, 0, B_INFINITY};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 3, 4, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, once_q, since_node, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically((once[:100]{q}) -> ({p} since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 100};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, 0, B_INFINITY};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 3, 4, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, once_q, since_node, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically((once[:1000]{q}) -> ({p} since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 1000};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, 0, B_INFINITY};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 3, 4, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, once_q, since_node, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AlwaysBQR: historically(({r} && !{q} && once{q}) -> ({p} since[A:B] {q}))
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ({p} since[3:10] {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, 3, 10};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0};
        DenseNode always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ({p} since[30:100] {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, 30, 100};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0};
        DenseNode always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ({p} since[300:1000] {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, 300, 1000};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0};
        DenseNode always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // AlwaysBR: historically({r} -> (historically[:N]{p}))
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:10]{p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, 10};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 3, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 4, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, inner_always, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:100]{p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, 100};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 3, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 4, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, inner_always, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically({r} -> (historically[:1000]{p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, 1000};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 3, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 4, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, inner_always, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // RecurBQR: historically(({r} && !{q} && once{q}) -> ((once[:N]({p} or {q})) since {q}))
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ((once[:10]({p} or {q})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode p_or_q{empty(holder), empty(holder), NodeType::OR, 1, 0, 0, 0};
        DenseNode once_p_or_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 7, 0, 10};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 8, 0, 0, B_INFINITY};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 9, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 10, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, p_or_q, once_p_or_q, since_node, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ((once[:100]({p} or {q})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode p_or_q{empty(holder), empty(holder), NodeType::OR, 1, 0, 0, 0};
        DenseNode once_p_or_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 7, 0, 100};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 8, 0, 0, B_INFINITY};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 9, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 10, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, p_or_q, once_p_or_q, since_node, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ((once[:1000]({p} or {q})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};
        DenseNode p_or_q{empty(holder), empty(holder), NodeType::OR, 1, 0, 0, 0};
        DenseNode once_p_or_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 7, 0, 1000};
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 8, 0, 0, B_INFINITY};
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 9, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 10, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, p_or_q, once_p_or_q, since_node, implies_node, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // RecurGLB: historically(once[:N]{p})
    else if (strcmp(arguments.spec, "historically(once[:10]{p})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 10};
        DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<DenseNode> nodes{p, once, always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(once[:100]{p})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 100};
        DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<DenseNode> nodes{p, once, always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(once[:1000]{p})") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 1000};
        DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<DenseNode> nodes{p, once, always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].p});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // RespondBQR: historically(({r} && !{q} && once{q}) -> ( (({s} -> once[A:B]{p}) and not((not {s}) since[B:] {p})) since {q}))
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ( (({s} -> once[3:10]{p}) and not((not {s}) since[10:] {p})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and_A1{empty(holder), empty(holder), NodeType::AND, 3, 4, 0, 0};
        DenseNode and_A2{empty(holder), empty(holder), NodeType::AND, 6, 5, 0, 0};
        DenseNode once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 1, 3, 10};
        DenseNode implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 2, 8, 0, 0};
        DenseNode not_s{empty(holder), empty(holder), NodeType::NOT, 0, 2, 0, 0};
        DenseNode since_F{empty(holder), empty(holder), NodeType::SINCE, 10, 1, 10, B_INFINITY};
        DenseNode not_F{empty(holder), empty(holder), NodeType::NOT, 0, 11, 0, 0};
        DenseNode and_C{empty(holder), empty(holder), NodeType::AND, 9, 12, 0, 0};
        DenseNode since_B{empty(holder), empty(holder), NodeType::SINCE, 13, 0, 0, B_INFINITY};
        DenseNode implies_main{empty(holder), empty(holder), NodeType::IMPLIES, 7, 14, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 15, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, s, r, not_q, once_q, and_A1, and_A2, once_p, implies_D, not_s, since_F, not_F, and_C, since_B, implies_main, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].s, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ( (({s} -> once[30:100]{p}) and not((not {s}) since[100:] {p})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and_A1{empty(holder), empty(holder), NodeType::AND, 3, 4, 0, 0};
        DenseNode and_A2{empty(holder), empty(holder), NodeType::AND, 6, 5, 0, 0};
        DenseNode once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 1, 30, 100};
        DenseNode implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 2, 8, 0, 0};
        DenseNode not_s{empty(holder), empty(holder), NodeType::NOT, 0, 2, 0, 0};
        DenseNode since_F{empty(holder), empty(holder), NodeType::SINCE, 10, 1, 100, B_INFINITY};
        DenseNode not_F{empty(holder), empty(holder), NodeType::NOT, 0, 11, 0, 0};
        DenseNode and_C{empty(holder), empty(holder), NodeType::AND, 9, 12, 0, 0};
        DenseNode since_B{empty(holder), empty(holder), NodeType::SINCE, 13, 0, 0, B_INFINITY};
        DenseNode implies_main{empty(holder), empty(holder), NodeType::IMPLIES, 7, 14, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 15, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, s, r, not_q, once_q, and_A1, and_A2, once_p, implies_D, not_s, since_F, not_F, and_C, since_B, implies_main, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].s, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({r} && !{q} && once{q}) -> ( (({s} -> once[300:1000]{p}) and not((not {s}) since[1000:] {p})) since {q}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY};
        DenseNode and_A1{empty(holder), empty(holder), NodeType::AND, 3, 4, 0, 0};
        DenseNode and_A2{empty(holder), empty(holder), NodeType::AND, 6, 5, 0, 0};
        DenseNode once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 1, 300, 1000};
        DenseNode implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 2, 8, 0, 0};
        DenseNode not_s{empty(holder), empty(holder), NodeType::NOT, 0, 2, 0, 0};
        DenseNode since_F{empty(holder), empty(holder), NodeType::SINCE, 10, 1, 1000, B_INFINITY};
        DenseNode not_F{empty(holder), empty(holder), NodeType::NOT, 0, 11, 0, 0};
        DenseNode and_C{empty(holder), empty(holder), NodeType::AND, 9, 12, 0, 0};
        DenseNode since_B{empty(holder), empty(holder), NodeType::SINCE, 13, 0, 0, B_INFINITY};
        DenseNode implies_main{empty(holder), empty(holder), NodeType::IMPLIES, 7, 14, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 15, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, s, r, not_q, once_q, and_A1, and_A2, once_p, implies_D, not_s, since_F, not_F, and_C, since_B, implies_main, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].s, allInputs[i - 1].r});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    // RespondGLB: historically(({s} -> once[A:B]{p}) and not((not {s}) since[B:] {p}))
    else if (strcmp(arguments.spec, "historically(({s} -> once[3:10]{p}) and not((not {s}) since[10:] {p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 3, 10};
        DenseNode implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 1, 2, 0, 0};
        DenseNode not_s{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_F{empty(holder), empty(holder), NodeType::SINCE, 4, 0, 10, B_INFINITY};
        DenseNode not_F{empty(holder), empty(holder), NodeType::NOT, 0, 5, 0, 0};
        DenseNode and_C{empty(holder), empty(holder), NodeType::AND, 3, 6, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 7, 0, B_INFINITY};
        std::vector<DenseNode> nodes{p, s, once_p, implies_D, not_s, since_F, not_F, and_C, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].p, allInputs[i - 1].s});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({s} -> once[30:100]{p}) and not((not {s}) since[100:] {p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 30, 100};
        DenseNode implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 1, 2, 0, 0};
        DenseNode not_s{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_F{empty(holder), empty(holder), NodeType::SINCE, 4, 0, 100, B_INFINITY};
        DenseNode not_F{empty(holder), empty(holder), NodeType::NOT, 0, 5, 0, 0};
        DenseNode and_C{empty(holder), empty(holder), NodeType::AND, 3, 6, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 7, 0, B_INFINITY};
        std::vector<DenseNode> nodes{p, s, once_p, implies_D, not_s, since_F, not_F, and_C, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].p, allInputs[i - 1].s});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else if (strcmp(arguments.spec, "historically(({s} -> once[300:1000]{p}) and not((not {s}) since[1000:] {p}))") == 0)
    {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 300, 1000};
        DenseNode implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 1, 2, 0, 0};
        DenseNode not_s{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since_F{empty(holder), empty(holder), NodeType::SINCE, 4, 0, 1000, B_INFINITY};
        DenseNode not_F{empty(holder), empty(holder), NodeType::NOT, 0, 5, 0, 0};
        DenseNode and_C{empty(holder), empty(holder), NodeType::AND, 3, 6, 0, 0};
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 7, 0, B_INFINITY};
        std::vector<DenseNode> nodes{p, s, once_p, implies_D, not_s, since_F, not_F, and_C, root_always};

        for (int i = 1; i < allInputs.size(); i++)
        {
            run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].p, allInputs[i - 1].s});
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
    else
    {
        std::cout << "Error: Can't find code for spec: " << arguments.spec << std::endl;
    }

}