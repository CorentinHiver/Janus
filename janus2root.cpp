// #include "DataReader_5202.hpp" // First version
#include "Reader5052.hpp" // Second version
// #include "JanusSession.hpp"
#include "Analyse/RootHitSiPM.hpp"

using namespace Colib;

int main(int argc, char** argv)
{
  if (argc < 2) {error("No filename !!"); return 1;}
  for (int i = 1; i<argc; ++i)
  {
    Reader5052<64> reader(argv[i]);
    auto rootFilename = removeExtension(removePath(argv[i]))+".root";
    reader.initRootOutput(rootFilename, "recreate");
    Colib::pauseDebug();
    while(reader.readEvent()) 
    {
      reader.fillTree();
      debug(reader.getHit());
      debug("hit cursor", reader.getCursor());
      Colib::pauseDebug();
    }
    reader.write();
  }
  return 0;
}