#ifndef TREESIPM_HPP
#define TREESIPM_HPP

#include "LibCo/libRoot.hpp"
#include "HitSiPM.hpp"

template<size_t _size>
class TreeSiPM
{
public:
  TreeSiPM(){}
  ~TreeSiPM(){}

  void init()
  {
    m_tree = new TTree("SiPM","SiPM");
    m_tree->SetDirectory(nullptr);
    createBranch(m_tree, &m_size, "size");
    createBranch(m_tree, &m_hit.timestamp, "timestamp");
    createBranchArray(m_tree, &m_hit.HGs , "HG" , "size");
    createBranchArray(m_tree, &m_hit.LGs , "LG" , "size");
    createBranchArray(m_tree, &m_hit.ToTs, "ToT", "size");
    createBranchArray(m_tree, &m_hit.ToAs, "ToA", "size");
  }

  auto siphonData(MyBinaryData &data) {return m_hit.siphonData(data);}

  auto operator->() {return m_tree;} 
  auto get() {return m_tree;} 
  
  void fill(MyBinaryData &data)
  {
    m_hit.siphonData(data);
    m_tree->Fill();
  }

  void write(std::string const & filename, std::string const & mode)
  {
    auto file = TFile::Open(filename.c_str(),mode.c_str());
    file->cd();
    m_tree->Write();
    file->Close();
  }

  auto & hit() {return m_hit;}

  friend std::ostream& operator<<(std::ostream& out, TreeSiPM<_size> & tree)
  {
    out << tree.get();
    return out;
  }

private:
  TTree *m_tree = nullptr;
  HitSiPM<_size> m_hit;
  size_t m_size = _size;
};


#endif //TREESIPM_HPP
