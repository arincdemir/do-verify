#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

namespace json_reader {

struct TimescalesInput {
    int time;
    std::vector<bool> propositions;
};

TimescalesInput read_line(std::string &line) {
    std::vector<bool> propositions;
    int time = 0;
    std::string segment;
    int index = 0;
    while (!isdigit(line.at(index))) {
        index++;
    }
    int numStart = index - 1;
    while (isdigit(line.at(index))) {
        index++;
    }
    time = stoi(line.substr(numStart, index - numStart));
    
    while (line.at(index) != '}') {
        while (line.at(index) != ':') {
            index++;
        }
        int truthStart = index + 2;
        while (line.at(index) != ',' && line.at(index) != '}') {
            index++;
        }
        if ("true" == line.substr(truthStart, index - truthStart)) {
            propositions.push_back(true);
        }
        else {
            propositions.push_back(false);
        }
    }
    return TimescalesInput{time, propositions};

};


}