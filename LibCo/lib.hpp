#include <unordered_map>
#include <string>
#include <limits>
#include <fstream>

//todo : remove lib.hpp


namespace CoLib
{
  
  //----------------------------------------------------//
  //       General files and folders manipulations      //
  //----------------------------------------------------//

  std::string removeExtension (std::string const & string) { return (string.substr(0, string.find_last_of(".")  ));}
  std::string extension       (std::string const & string) { return (string.substr(   string.find_last_of(".")+1));}
  std::string getExtension    (std::string const & string) { return (string.substr(   string.find_last_of(".")+1));}
  std::string getPath         (std::string const & string) { return (string.substr(0, string.find_last_of("/")+1));}
  std::string removePath      (std::string const & string) { return (string.substr(   string.find_last_of("/")+1));}
  std::string rmPathAndExt    (std::string const & string) { return            removePath(removeExtension(string));}
  std::string get_shortname   (std::string const & string) { return            removePath(removeExtension(string));}

  //-----------------------//
  //  Handle lookup tables //
  //-----------------------//

  using stringToIntMap = std::unordered_map<std::string, size_t>;
  constexpr size_t mapMax = std::numeric_limits<size_t>::max();
}

