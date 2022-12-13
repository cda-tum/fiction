#ifndef _HYBRID_LMXRLF_HPP_
#define _HYBRID_LMXRLF_HPP_

#include "coloring_algorithm.hpp"
#include "lmxrlf.hpp"
#include "tabucol.hpp"

using GraphColoring::GraphColor;
using GraphColoring::Lmxrlf;
using GraphColoring::Tabucol;

namespace GraphColoring{
	class HybridLmxrlf : public GraphColor {
		private: 
			int condition;
			map<string,vector<string>> get_subgraph(map<string,int> coloring);

		public: 
            /* Constructors */
			explicit HybridLmxrlf(map<string,vector<string> > graph, int condition = 0) : GraphColor(graph) { this->condition = condition; }

            /* Mutators */
			void set_condition(int condition) { this->condition = condition; }
			map<string,int> color();

            /* Accessors */
			string get_algorithm() { return "Hybrid LMXRLF"; }
	};
}

#endif //_HYBRID_LMXRLF_HPP_
