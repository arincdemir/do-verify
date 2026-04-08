#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <argp.h>

#include "do-verify/MTLEngine.hpp"
#include "do-verify/ptl.hpp"
#include "do-verify/json_feeder.hpp"
#include "do-verify/interval_set.hpp"

using namespace db_interval_set;
using namespace do_verify;

// argp option keys
enum RYBINX_OPTS : uint8_t
{
    OPT_DENSE = 'v',
    OPT_DISCRETE = 'x'
};

const char *argp_program_version = "do-verify-bin 0.2.0";
const char *argp_program_bug_address = "Arinc Demir <github.com/arincdemir>";
static const char *doc = "Do-verify (Reelay) CLI Tool";
static const char *args_doc = "TRACE_FILE PROPERTIES_FILE";

struct arguments
{
    char *trace_file = nullptr;
    char *properties_file = nullptr;
    bool dense = false;
    bool discrete = false;
};

static struct argp_option options[] = {
    {"dense", OPT_DENSE, nullptr, 0, "Use dense time model (default)", 0},
    {"discrete", OPT_DISCRETE, nullptr, 0, "Use discrete time model", 0},
    {nullptr}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    auto *args = (struct arguments *)state->input;
    switch (key)
    {
    case OPT_DENSE:
        args->dense = true;
        break;
    case OPT_DISCRETE:
        args->discrete = true;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num == 0)
        {
            args->trace_file = arg;
        }
        else if (state->arg_num == 1)
        {
            args->properties_file = arg;
        }
        else 
        {
            return ARGP_ERR_UNKNOWN;
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

static struct argp argp = {options, parse_opt, args_doc, doc};

void discrete_case(const arguments &args);
void dense_case(const arguments &args);

int main(int argc, char **argv)
{
    struct arguments args;
    argp_parse(&argp, argc, argv, 0, nullptr, &args);

    if (!args.dense && !args.discrete)
    {
        args.dense = true; // Default to dense
    }

    if (args.discrete)
    {
        discrete_case(args);
    }
    else
    {
        dense_case(args);
    }

    return 0;
}

void dense_case(const arguments &args)
{
    std::ifstream props_in(args.properties_file);
    if (!props_in.is_open())
    {
        std::cerr << "Error opening properties file: " << args.properties_file << std::endl;
        exit(1);
    }

    std::vector<std::string> formulas;
    std::string line;
    while (std::getline(props_in, line))
    {
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) continue;
        formulas.push_back(line);
    }

    if (formulas.empty())
    {
        std::cerr << "No formulas found in properties file." << std::endl;
        exit(1);
    }

    DenseMultiPropertyMonitor monitor = createDenseMultiPropertyMonitor(3000);
    ptl_parser parser;

    for (const auto &f : formulas)
    {
        try {
            parser.parse_dense(f, monitor);
        } catch (const std::exception &e) {
            std::cerr << "Error parsing formula: " << f << "\n" << e.what() << std::endl;
            exit(1);
        }
    }
    finalize_monitor(monitor);

    auto *feeder = create_dense_json_feeder(monitor, args.trace_file);
    if (!feeder)
    {
        std::cerr << "Error creating feeder for trace file: " << args.trace_file << std::endl;
        exit(1);
    }

    std::vector<IntervalSet> output;
    while (feed_next(feeder, output))
    {
    }

    destroy_feeder(feeder);
}

void discrete_case(const arguments &args)
{
    std::ifstream props_in(args.properties_file);
    if (!props_in.is_open())
    {
        std::cerr << "Error opening properties file: " << args.properties_file << std::endl;
        exit(1);
    }

    std::vector<std::string> formulas;
    std::string line;
    while (std::getline(props_in, line))
    {
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) continue;
        formulas.push_back(line);
    }

    if (formulas.empty())
    {
        std::cerr << "No formulas found in properties file." << std::endl;
        exit(1);
    }

    DiscreteMultiPropertyMonitor monitor = createDiscreteMultiPropertyMonitor(3000);
    ptl_parser parser;

    for (const auto &f : formulas)
    {
        try {
            parser.parse_discrete(f, monitor);
        } catch (const std::exception &e) {
            std::cerr << "Error parsing formula: " << f << "\n" << e.what() << std::endl;
            exit(1);
        }
    }
    finalize_monitor(monitor);

    auto *feeder = create_discrete_json_feeder(monitor, args.trace_file);
    if (!feeder)
    {
        std::cerr << "Error creating feeder for trace file: " << args.trace_file << std::endl;
        exit(1);
    }

    std::vector<bool> output;
    while (feed_next(feeder, output))
    {
    }

    destroy_feeder(feeder);
}
