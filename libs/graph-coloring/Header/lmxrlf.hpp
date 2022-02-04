#ifndef _LMXRLF_HPP_
#define _LMXRLF_HPP_

#include "coloring_algorithm.hpp"

using GraphColoring::GraphColor;

namespace GraphColoring{
    class Lmxrlf : public GraphColor {
        private:
            /* Internal Members */
            int condition;
            int global;
            int f;
            int local;

            /* Helper Functions */
            vector<string> get_independent(vector<string> set);
            vector<string> make_independent_set();
            int objf(vector<string> set);
            int min_objf(vector< vector<string> > list_of_best);
            int max_objf(vector< vector<string> > list_of_best);
            int min_pos_objf(vector< vector<string> > list_of_best);
            int max_pos_objf(vector< vector<string> > list_of_best);
            vector<string> uncolored_neighbor(vector<string> new_set);
            map<string,int> lmxrlf_alg(int endcond);

        public: 
            /* Constructors */
            explicit Lmxrlf(map<string, vector<string> > input_graph, int condition = 0, int global = 10, int f = 1, int local = 10);

            /* Mutators */
            map<string,int> color();

            /* Accessors */
            void set_condition(int condition) { this->condition = condition; }
            void set_global(int global) { this->global = global; }
            void set_f(int f) { this->f = f; }
            void set_local(int local) { this->local = local; }
            string get_algorithm() { return "LMXRLF"; }
    };
}

#endif //_LMXRLF_HPP_
