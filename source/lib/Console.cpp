#include "Console.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>

Console::Console(void)
{
  verbose = false;
  ignore_unknown = false;
}


// Print an error message and return false;
bool Console::message_error(std::string message)
{
  std::cerr<<"[Error]"<<message<<std::endl;
  return false;
  
}
// Print a status message and return true;
bool Console::message_status(std::string message)
{
  if (verbose == true)
    std::clog<<"[Status]"<<message<<std::endl;
  return true;
  
}

// Evaluate one line of string (which should contain only one directive).
bool Console::eval(std::string line)
{
  // Trim the string.
  string_trim_blank(  line);
  string_trim_comment(line);
  if (line.empty())
    return true;
  
  //std::cout<<line<<std::endl;
  
  // Find "=" in the string.
  // This determines variables/commands.
  size_t eq = line.find_first_of("=");
  if (eq != std::string::npos)
  {
    // Variable
    std::string name  = line.substr(0,eq); string_trim_blank(name);
    std::string value = line.substr(eq+1); string_trim_blank(value); 
    dv_type::iterator iter;
    if(find_var(name,iter)==false)
      return ignore_unknown;
    iter->second = value;
    return true;
  }
  else
  {
    // Command
    std::string name;
    std::vector<std::string> args;
    string_split(line,name,args);
    // Search the dictionary.
    dc_type::iterator iter;
    if(find_cmd(name,iter)==false)
      return ignore_unknown;
    //
    return iter->second(*this,args);
  }
}

// Evaluate lines in a file.
void Console::runfile(const char *filename)
{
  std::ifstream ss(filename);
  while(1)
  {
      std::string line;
      std::getline(ss,line);
      if(ss.good()==false)
          break;
      if(eval(line)==false){
        std::cout<<"Error, execution halts."<<std::endl;
        break;
      }
  }
}

void Console::show(void)
{
  for(auto it : dict_v)
    message_variable(it.first,it.second);
  for(auto it : dict_c)
    message_command(it.first);
}

void Console::input(void)
{
  while(1)
  {
    std::cout<<"->"<<std::flush;
    std::string line;
    std::getline(std::cin,line);
    eval(line);
  }
}

bool Console::addVariable(std::string name,std::string value)
{
  auto iter = dict_v.find(name);
  if ( iter!= dict_v.end() ){
    message_has_existed(name);
    return false;
  }
  dict_v.insert(std::make_pair(name,value));
  return true;
}

bool Console::addCommand(std::string name,cmd_type f)
{
  auto iter  = dict_v.find(name);
  if ( iter != dict_v.end() ){
    message_has_existed(name);
    return false;
  }
  dict_c.insert(std::make_pair(name,f));
  return true;
}

void Console::string_trim_blank(std::string &line)
{
  const char *blank = " \t\n\r\f\v";
  line.erase(0, line.find_first_not_of(blank));
  line.erase(line.find_last_not_of(blank)+1);
}

void Console::string_trim_comment(std::string &line)
{
  size_t pos = line.find_first_of("#");
  if (pos != std::string::npos)
    line = line.erase(pos);
}


// Split a string delimited by spaces.
void Console::string_split(const std::string &line,std::vector<std::string> &slist)
{
  slist.clear();
  std::stringstream ss; ss.str(line);
  std::string token;
  while(std::getline(ss,token,' '))
    if(token.empty()==false)
      slist.push_back(token);
}


void Console::string_split(const std::string &line,std::string &head,std::vector<std::string> &slist)
{
  head.clear();
  slist.clear();
  std::stringstream ss; ss.str(line);
  std::string token;
  while(std::getline(ss,token,' '))
    if(token.empty()==false)
      if(head.empty())
        head = token;
      else
        slist.push_back(token);
}

bool Console::find_var(const std::string &name,dv_type::iterator &iter)
{
  iter = dict_v.find(name);
  if (iter == dict_v.end()){
    message_not_found(name);
    return false;
  }
  return true;
}

bool Console::find_cmd(const std::string &name,dc_type::iterator &iter)
{
  iter = dict_c.find(name);
  if (iter == dict_c.end()){
    message_not_found(name);
    return false;
  }
  return true;
}
