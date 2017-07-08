#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>


// This class parses input from the user, or from a text file.
// Following the directives, it assigns variables and executes commands.

class   Console;
typedef bool(*cmd_type)(Console&,std::vector<std::string>&);
typedef std::map<std::string,std::string> dv_type;
typedef std::map<std::string,cmd_type> dc_type;

//typedef bool(*cmd_type)(Console&,std::vector<std::string>&);

class Console
{
public:
  Console(void);
  // Register a variable to the dictionary.
  bool addVariable(std::string name,std::string value);
  // Register a command to the dictionary.
  bool addCommand(std::string name,cmd_type f);
  // Clear all registered variables and commands.
  void clear(void){dict_v.clear();dict_c.clear();}
  
  //
  template<class T>
  bool getVariable(std::string name,T *dst,int len,char del=',');
  template<class T>
  bool getVariable(std::string name,T &dst);
  
  
  

  
  // Interactive mode.
  void input(void);
  // Evaluate one line of string (which should contain only one directive).
  bool eval(std::string line);
  // Evaluate lines in a file.
  void runfile(const char *filename);
  // Print out all content.
  void show(void);
  
  bool ignore_unknown;
  //
  bool verbose;
  //bool silent;
  
  // Convert a string to a sequence of values of the given type.
  template<class T> 
  static bool string_cast(std::string input,T &dst){std::istringstream(input)>>dst;return true;}
  // Convert a string to a value of the given type.
  template<class T> 
  static bool string_cast(std::string input,T *dst,int len,char del=',');
  
  // Print an error message and return false;
  bool message_error(std::string message);
  // Print a status message and return true;
  bool message_status(std::string message);
  
//private:
  // Remove the blank characters in the beginning and the end of the string.
  static void string_trim_blank(std::string &line);
  // Remove the characters starting from the first "#" character found.
  static void string_trim_comment(std::string &line);
  // Split a string delimited by spaces.
  static void string_split(const std::string &line,std::vector<std::string> &slist);
  static void string_split(const std::string &line,std::string &head,std::vector<std::string> &slist);
  
  // Print out messages.
  static void message_not_found(std::string key){std::cerr<<"\""<<key<<"\" not found."<<std::endl;}
  static void message_has_existed(std::string key){std::cerr<<"\""<<key<<"\" has existed."<<std::endl;}
  static void message_variable(std::string name,std::string value){std::cout<<name<<" = \""<<value<<"\""<<std::endl;}
  static void message_command(std::string name){std::cout<<name<<std::endl;}

  bool find_var(const std::string &name,dv_type::iterator &iter);
  bool find_cmd(const std::string &name,dc_type::iterator &iter);

  dv_type dict_v;
  dc_type dict_c;
  
  
};

template<class T> 
bool Console::string_cast(std::string input,T *dst,int len,char del)
{
  std::istringstream ss( input );
  std::string element;
  for(int i=0;i<len;i++)
  {
    if(!std::getline(ss,element,del)){
      for(;i<len;i++)
        dst[i] = 0;
      return false;
    }
    std::istringstream( element ) >> dst[i];
  }
  return true;
}

template<class T>
bool Console::getVariable(std::string name,T *dst,int len,char del)
{
  dv_type::iterator iter;
  if(find_var(name,iter)==false)
    return false;
  return string_cast(iter->second,dst,len,del);
}

template<class T>
bool Console::getVariable(std::string name,T &dst)
{
  dv_type::iterator iter;
  if(find_var(name,iter)==false)
    return false;
  return string_cast(iter->second,dst);
}

#endif