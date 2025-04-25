#ifndef JANUSSESSIONS_HPP
#define JANUSSESSIONS_HPP

#include "LibCo/JanusLib.hpp"

class JanusSession
{
public:
  JanusSession(){}
  int parseConfig();
  int connect()
  {
    return 0;
  }
  private:
  Janus_Config_t m_config;
  std::string m_config_name;
};

int JanusSession::parseConfig()
{
  std::ifstream file(m_config_name);
  memset(&m_config, 0, sizeof(m_config)); // Initialize
}

#endif //JANUSSESSIONS_HPP