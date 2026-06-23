#include "RootHitSiPM.hpp"

using namespace Colib;

void Analysis(std::string const & filename)
{
  auto file = TFile::Open(filename.c_str(), "read");
  if (!file) throw_error(filename+" can't be open !!");
  auto tree = file->Get<TTree>("SiPM");
  if (!tree) throw_error("in " + filename + " : tree SiPM can't be open !!");
  RootHitSiPM hit(tree);
  auto c = new TCanvas;
  for (int i = 0; i<100; ++i)
  {
    tree->GetEntry(i);
    hit.drawHitmap();
    c->SaveAs(("images/HGmaps/"+std::to_string(i)+".png").c_str());


  }
  file->Close();
}

int main(int argc, char** argv)
{
  Analysis(argv[1]);
  return 0;
}