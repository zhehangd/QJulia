#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <vector>
#include <map>

class Parser
{
public:
    
    // Get the instance of the singleton parser.
    static Parser& getInstance(void);
    
    bool add(char opt,bool isArg);
    
    void parse(int argc,const char **argv);

    
    // This is an ordered list of options and arguments.
    // Options: character, argument (if has)
    // Input:  c=0 
    std::vector<std::pair<char,std::string> > input;
    // This store registered options.
    std::map<char,bool> optMap;
};


#endif
