// #include "DataReader_5202.hpp" // First version
#include "Reader5052.hpp" // Second version
// #include "JanusSession.hpp"

int main(int argc, char** argv)
{
  std::string filename;
  if (argc == 2) filename = argv[1];

  // // MyDataReader data(filename); // First version
  // // data.toRoot();

  Reader5052<64> reader(filename); // Second version
  reader.initOutput();
  auto rootFilename = removeExtension(removePath(filename))+".root";
  reader.writeTo(rootFilename,"recreate");
  while(reader.readEvent()) reader.fillTree();
  print("writting");
  reader.write();
  // reader.write(rootFilename,"recreate");

  // JanusSession session; // Other project : 
  // session.connect();
  return 0;
}