#include <vector>
#include <map>
#include <iostream>

using std::cerr;
using std::endl;
using std::vector;
using std::map;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostringstream;

ifstream& Getline(ifstream& ifs, string& line) {
    getline(ifs,line);
    if (!line.empty()) {
        while (isspace(line.at(line.size()-1))) {
            line = line.substr(0,line.size()-1);
        }
    }
    return ifs;
}

vector<string> split(string to_split) {
    vector<string> split_string;
    unsigned index_start;
    for (unsigned i = 0;i < to_split.length();i++) {
        index_start = i;
        while(i < to_split.length() && !isspace(to_split.at(i))) { 
            i++; 
        }
        split_string.push_back(to_split.substr(index_start,i - index_start));
    }
    return split_string;
}

vector<vector<string>> get_input(string input_file) {
    vector<vector<string>> input;

    ifstream file(input_file);
    if(file.is_open()) {
        string line;
        while(Getline(file,line)) {
            vector<string> words = split(line);
            if(words.size() > 1) {
                for(unsigned i=1; i<words.size(); i++) {
                    if(words[i] != "0" && words[i] != "1" && words[i] != "x" && words[i] != "X") {
                        cerr << "Problem with this input line: " << line << endl;
                        cerr << "Problem is with word: \"" << words[i] << "\" at position " << i << endl;
                        return vector<vector<string>>();
                    }
                }
            } else {
                cerr << "No Actuation Data Found, Please Check input file" << endl;
                return vector<vector<string>>();
            }
            input.push_back(words);
        }
        file.close();
    } else {
        cerr << "Input File Not Found" << endl;
        return vector<vector<string>>();
    }
    return input;
}

map<string,vector<string>> parse_edge_list(string input_file) {
    map<string,vector<string>> input_graph;
    ifstream file(input_file);
    if(file.is_open()) {
        string line;
        int vertices = -1;
        int flag = 0;
        while(!flag && Getline(file,line)) {
            while(line.size() == 0) {
                Getline(file,line);
            }
            vector<string> words = split(line);
            if(words.size() != 0) {
                if(words[0] == "p") {
                    vertices = stoi(words[2]);
                    flag = 1;
                }
            }
        }
        if(!flag || vertices == -3) {
            cerr << "File is missing parameter line before edge list" << endl;
            cerr << "Should be: \"p edge <number of vertices> <number of edges>\"" << endl;
            return map<string,vector<string>>();
        }
        for(int i=0; i<vertices; i++) {
            string pre = "v";
            string temp;
            std::ostringstream convert;
            convert << (i+1);
            temp = convert.str();
            pre.append(temp);
            vector<string> base;
            input_graph[pre] = base;
        }
        while(Getline(file,line))
        {
            vector<string> words = split(line);
            if(words[0] == "e") {
                string arg1 = "v";
                arg1.append(words[1]);
                string arg2 = "v";
                arg2.append(words[2]);
                vector<string> base;
                vector<string> base2;
                input_graph[arg1].push_back(arg2);
                input_graph[arg2].push_back(arg1);
            }
        }
    } else {
        cerr << "Input File Not Found" << endl;
        return map<string,vector<string>>();
    }
    return input_graph;
}

//Used to parse test inputs where the first line is the number of
//vertices, and the next lines are the edge matrix
map<string,vector<string>> parse_edge_matrix(string input_file) {
    map<string,vector<string>> input_graph;
    string pre = "v";

    ifstream file(input_file);
    if(file.is_open()) {
        string line;
        Getline(file,line);
        int n = stoi(line);
        int i = 0;
        while(Getline(file,line)) {
            i += 1;
            vector<string> words = split(line);
            if((int)words.size() != n) {
                cerr << "Invalid Input, line " << i << " is not the correct length (" << words.size() << "," << n << "): " << line << endl;
                for (unsigned i = 0;i < words.size();i++) { 
                    cerr << "\t" << words.at(i) << endl; 
                }
                return map<string,vector<string>>();
            }
            vector<string> edges;
            for(int j = 0; j < n; j++) {
                if(words[j] == "1") {
                    pre = "v";
                    string temp;
                    ostringstream convert;
                    convert << (j+1);
                    temp = convert.str();
                    edges.push_back(pre.append(temp));
                }
            }
            pre = "v";
            string temp;
            ostringstream convert;
            convert << i;
            temp = convert.str();
            input_graph[pre.append(temp)] = edges;
        }
        if(i != n) {
            cerr << "Input is not the right length" << endl;
            return map<string,vector<string>>();
        }
        file.close();
    } else {
        cerr << "Input File Not Found" << endl;
        return map<string,vector<string>>();
    }
    return input_graph;
}
