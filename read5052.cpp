// #include "DataReader_5202.hpp" // First version
#include "Reader5052.hpp" // Second version
// #include "JanusSession.hpp"

int main(int argc, char** argv)
{
  std::string filename;
  if (argc == 2) filename = argv[1];
  // // MyDataReader data(filename);
  // // data.toRoot();

  Reader5052<64> reader(filename);
  reader.initOutput();
  while(reader.readEvent()) reader.fillTree();
  auto rootFilename = removeExtension(removePath(filename))+".root";
  reader.write(rootFilename,"recreate");

  // JanusSession session;
  // session.connect();
  return 0;
}