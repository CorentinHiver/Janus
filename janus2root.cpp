#include "Reader5052.hpp"

using namespace Colib;

int main(int argc, char** argv)
{
  if (argc < 2) {error("No filename !!"); return 1;}
  for (int i = 1; i<argc; ++i)
  {
    Reader5052<64> reader(argv[i]);
    auto rootFilename = removeExtension(removePath(argv[i]))+".root";
    if (fileExists(rootFilename)) {print("Skipping already existing", rootFilename); continue;}
    reader.initRootOutput(rootFilename, "recreate");
    reader.convert();
  }
  return 0;
}

// pauseDebug();
// while(reader.readEvent()) 
// {
// #ifdef SAFE
//   try{reader.fillTree();}
//   catch(...){reader.write(); return 1;}
// #else // not SAFE
//   reader.fillTree();
// #endif // SAFE
//   debug(reader.getHit());
//   debug("hit cursor", reader.getCursor());
//   pauseDebug();
// }
// reader.write();