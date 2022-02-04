#ifndef _MCS_HPP_
#define _MCS_HPP_

#include "coloring_algorithm.hpp"

using GraphColoring::GraphColor;

namespace GraphColoring{
	class Mcs : public GraphColor {
		public: 
			/* Constructors */
			explicit Mcs(map<string,vector<string>> graph) : GraphColor(graph) {}

			/* Mutators */
			map<string,int> color();

			/* Accessors */
			string get_algorithm() { return "MCS"; }
	};
}

#endif //_MCS_HPP_
