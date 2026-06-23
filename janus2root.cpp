// #include "DataReader_5202.hpp" // First version
#include "Reader5052.hpp" // Second version
// #include "JanusSession.hpp"
#include "Analyse/RootHitSiPM.hpp"

using namespace Colib;

int main(int argc, char** argv)
{
  std::string filename;
  if (argc == 2) filename = argv[1];
  else {error("No filename !!"); return 1;}

  // // MyDataReader data(filename); // First version
  // // data.toRoot();

  Reader5052<64> reader(filename); // Second version
  // reader.setMaxHits(1e6);
  auto rootFilename = removeExtension(removePath(filename))+".root";
  reader.initRootOutput(rootFilename, "recreate");
  Colib::pauseDebug();
  while(reader.readEvent()) 
  {
    reader.fillTree();
    debug(reader.getHit());
    debug("hit cursor", reader.getCursor());
    // if (reader.getCursor() % 10000 == 0) print("\r", reader.getSizeReadRel());
    Colib::pauseDebug();
  }
  reader.write();
  // reader.write(rootFilename,"recreate");

  // JanusSession session; // Other project : 
  // session.connect();
  return 0;
}