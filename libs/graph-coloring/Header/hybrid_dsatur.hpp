#ifndef _HYBRID_DSATUR_HPP_
#define _HYBRID_DSATUR_HPP_

#include "coloring_algorithm.hpp"
#include "dsatur.hpp"
#include "tabucol.hpp"

using GraphColoring::GraphColor;
using GraphColoring::Dsatur;
using GraphColoring::Tabucol;

namespace GraphColoring{
	class HybridDsatur : public GraphColor {
		public: 
			/* Constructors */
			explicit HybridDsatur(map<string,vector<string>> graph) : GraphColor(graph) { }

			/* Mutators */
			map<string,int> color();

			/* Accessors */
			string get_algorithm() { return "HYBRID_DSATUR"; }
	};
}

#endif //_HYBRID_DSATUR_HPP_
