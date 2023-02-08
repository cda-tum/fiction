//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_GATE_LOGIC_MAP_HPP
#define FICTION_GATE_LOGIC_MAP_HPP

#include <map>
#include <string>

namespace fiction::gate_logic {

    const std::map<std::string, std::map<std::string, std::string>> GATE_TRUTH_TABLE = {
            {"and",       {{"00", "0"},  {"01", "0"},  {"10", "0"},  {"11", "1"}}},
            {"or",        {{"00", "0"},  {"01", "1"},  {"10", "1"},  {"11", "1"}}},
            {"nor",       {{"00", "1"},  {"01", "0"},  {"10", "0"},  {"11", "0"}}},
            {"xnor",      {{"00", "1"},  {"01", "0"},  {"10", "0"},  {"11", "1"}}},
            {"nand",      {{"00", "1"},  {"01", "1"},  {"10", "1"},  {"11", "0"}}},
            {"xor",       {{"00", "0"},  {"01", "1"},  {"10", "1"},  {"11", "0"}}},
            {"ha",        {{"00", "00"}, {"01", "10"}, {"10", "10"}, {"11", "01"}}},
            {"hourglass", {{"00", "00"}, {"01", "01"}, {"10", "10"}, {"11", "11"}}},
            {"inv",       {{"0",  "1"},  {"1",  "0"}}},
            {"cx",        {{"00", "00"}, {"01", "10"}, {"10", "01"}, {"11", "11"}}},
            {"inv_diag",  {{"0",  "1"},  {"1",  "0"}}},
            {"fo2",       {{"0",  "00"}, {"1",  "11"}}},
            {"wire",      {{"0",  "0"},  {"1",  "1"}}},
            {"wire_diag", {{"0",  "0"},  {"1",  "1"}}}
    };


    const std::map<std::string, std::map<std::string, std::string>> GATES_IN_OUT_NUMBER = {
            {"and",       {{"in", "2"}, {"out", "1"}}},
            {"or",        {{"in", "2"}, {"out", "1"}}},
            {"nor",       {{"in", "2"}, {"out", "1"}}},
            {"xnor",      {{"in", "2"}, {"out", "1"}}},
            {"nand",      {{"in", "2"}, {"out", "1"}}},
            {"xor",       {{"in", "2"}, {"out", "1"}}},
            {"ha",        {{"in", "2"}, {"out", "2"}}},
            {"hourglass", {{"in", "2"}, {"out", "2"}}},
            {"inv",       {{"in", "1"}, {"out", "1"}}},
            {"cx",        {{"in", "2"}, {"out", "2"}}},
            {"inv_diag",  {{"in", "1"}, {"out", "1"}}},
            {"fo2",       {{"in", "1"}, {"out", "2"}}},
            {"wire",      {{"in", "1"}, {"out", "1"}}},
            {"wire_diag", {{"in", "1"}, {"out", "1"}}}
    };

}  // namespace fiction::gate_logic

#endif //FICTION_GATE_LOGIC_MAP_HPP
