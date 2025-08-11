#include "JanusSession.hpp"

int main()
{
  JanusSession session("eth:10.10.205.51");
  print(session.getRead(), FERS_LibMsg[session.getRead()]);
}