#ifndef _LMXRLF_HPP_
#define _LMXRLF_HPP_

#include "coloring_algorithm.hpp"

using GraphColoring::GraphColor;

namespace GraphColoring
{
class Lmxrlf : public GraphColor
{
  private:
    /* Internal Members */
    int condition;
    int global;
    int f;
    int local;

    /* Helper Functions */
    vector<string>   get_independent(const vector<string>& set);
    vector<string>   make_independent_set();
    int              objf(const vector<string>& set);
    int              min_objf(const vector<vector<string>>& list_of_best);
    int              max_objf(const vector<vector<string>>& list_of_best);
    int              min_pos_objf(const vector<vector<string>>& list_of_best);
    int              max_pos_objf(const vector<vector<string>>& list_of_best);
    vector<string>   uncolored_neighbor(const vector<string>& new_set);
    map<string, int> lmxrlf_alg(const int endcond);

  public:
    /* Constructors */
    explicit Lmxrlf(const map<string, vector<string>>& input_graph, int condition = 0, int global = 10, int f = 1,
                    int local = 10);

    /* Mutators */
    map<string, int> color() override;

    /* Accessors */
    void set_condition(const int cond)
    {
        this->condition = cond;
    }
    void set_global(const int glob)
    {
        this->global = glob;
    }
    void set_f(const int f_val)
    {
        this->f = f_val;
    }
    void set_local(const int loc)
    {
        this->local = loc;
    }
    string get_algorithm() override
    {
        return "LMXRLF";
    }
};
}  // namespace GraphColoring

#endif  //_LMXRLF_HPP_
