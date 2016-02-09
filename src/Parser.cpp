
// isalnum()
#include <cctype>

#include <map>
#include <vector>
#include <string>

#include <iostream>

#include "Parser.h"


using namespace std;



// Get the instance of the singleton parser.
Parser& Parser::getInstance(void)
{
    static Parser parser;
    return parser;
}

bool Parser::add(char opt,bool isArg)
{
    // Option must be an alphanumeric character.
    if(isalnum(opt)==false)
        return false;
    // An option can be registered once.
    if(optMap.find(opt)!=optMap.end())
        return false;
    // Register the option.
    optMap.insert(make_pair(opt,isArg));
    return true;
}

void Parser::parse(int argc,const char **argv)
{
    input.clear(); input.reserve(100);
    // Validate the arguments.
    //  1. First character must pass isgraph()(no space' ').
    //  2. All the rest must be printable.
    for(int i=0;i<argc;i++)
    {
        // Current argment.
        const char* p = argv[i];
        // Validate the arguments.
        bool val = false;
        if(isgraph(p[0])!=0)
        {
            val = true;
            for(int j=1;p[j]!='\0';j++)
                if(isprint(p[j])==0){
                    val = false; break;}
        }
        if(val==false){
            cout<<"Abnormal argument ["<<p[0]<<"]."<<endl; continue;}
        // If this is an option.
        if(p[0]=='-')
        {
            // Each following alphanumeric characters are treated as an option.
            // Until a non-alphanumeric one is met.(space,'\0',etc.)
            for(int j=1;isalnum(p[j]);j++)
            {
                // [opt] the current option character.
                char opt = p[j];
                // Find it in the map.
                auto it = optMap.find(opt);
                // Check the existence.
                if(it==optMap.end())
                    cout<<"option -"<<opt<<" does not exist."<<endl;
                else
                    // Check whether an argument is needed.
                    if(it->second==false) // Takes no argument.
                        input.push_back(pair<char,string>(opt,""));
                    else // Takes an argument.
                        if(j!=1) // Only the first option after the '-' can.
                            cout<<"option -"<<opt<<" can't be here."<<endl;
                        else // Here is a potential problem, hopefully ok.
                            if((i+1)<argc)
                                input.push_back(pair<char,string>(opt,argv[(i++)+1]));
                            else
                                cout<<"option -"<<opt<<" takes an argument."<<endl;
            }
        }
        else // This is not an option, add it to the input vector.
            input.push_back(pair<char,string>(0,p));
    }
}





