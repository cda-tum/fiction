#ifndef _DSATUR_HPP_
#define _DSATUR_HPP_

#include "coloring_algorithm.hpp"

using GraphColoring::GraphColor;

namespace GraphColoring{
	class Dsatur : public GraphColor {
		public: 
			/* Constructors */
			explicit Dsatur(map<string,vector<string> > graph) : GraphColor(graph) {};

			/* Mutators */
			map<string,int> color();

			/* Accessors */
			string get_algorithm() { return "DSATUR"; }
	};
}

#endif //_DSATUR_HPP_
