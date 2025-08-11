#ifndef JANUSSESSIONS_HPP
#define JANUSSESSIONS_HPP

#include "../LibCo/libCo.hpp"
#include "FERSlib.h"
// #include "../ferslib/include/FERSlib.h"

class JanusSession
{
public:
  JanusSession(std::string path){m_read = this->connect(path);}
  int parseConfig();
  int connect(std::string path)
  {
    char* writable_path = new char[path.size() + 1]; // Allocate memory
    std::strcpy(writable_path, path.c_str());
    return FERS_OpenDevice(writable_path, 0);
  }
  auto const & getRead() const {return m_read;}
  private:
  // Janus_Config_t m_config;
  std::string m_config_name;
  int m_read = 0;
};

int JanusSession::parseConfig()
{
  // std::ifstream file(m_config_name);
  // memset(&m_config, 0, sizeof(m_config)); // Initialize
  return 1;
}

#endif //JANUSSESSIONS_HPP