#ifndef _TABUCOL_HPP_
#define _TABUCOL_HPP_

#include "coloring_algorithm.hpp"

using GraphColoring::GraphColor;

namespace GraphColoring{
    class Tabucol : public GraphColor {
        private:
            int condition;
            int tabu_size;
            int rep;
            int nbmax;

            int f(map<string,int> coloring);

        public: 
            /* Constructors */
            explicit Tabucol(map<string, vector<string> > input_graph, int condition = 0, int tabu_size = 25, int rep = 100, int nbmax = 1000);

            /* Mutators */
            map<string,int> color();
            void set_coloring(map<string,int> coloring) { this->graph_colors = coloring; }
            void set_condition(int con) { this->condition = con; }
            void set_tabu_size(int tabu_size) { this->tabu_size = tabu_size; }
            void set_rep(int rep) { this->rep = rep; }
            void set_nbmax(int nbmax) { this->nbmax = nbmax; }
            map<string,int> make_colors_contiguous(map<string,int>);

            /* Accessors */
            string get_algorithm() { return "TABUCOL"; }
    };
}

#endif //_TABUCOL_HPP_
