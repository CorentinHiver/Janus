#pragma once

#include "libCo.hpp"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

namespace LIB
{
  //---------------------//
  // TREE BRANCH FILLING //
  //---------------------//
  template<class T> std::string typeRoot(){return "Unknown";}

  template<> std::string typeRoot<bool          >(){return "O";}
  template<> std::string typeRoot<char          >(){return "B";}
  template<> std::string typeRoot<unsigned char >(){return "b";}
  template<> std::string typeRoot<short         >(){return "S";}
  template<> std::string typeRoot<unsigned short>(){return "s";}
  template<> std::string typeRoot<int           >(){return "I";}
  template<> std::string typeRoot<unsigned int  >(){return "i";}
  template<> std::string typeRoot<long          >(){return "G";}
  template<> std::string typeRoot<unsigned long >(){return "g";}
  template<> std::string typeRoot<double        >(){return "D";}
  template<> std::string typeRoot<float         >(){return "F";}
  template<> std::string typeRoot<Long64_t      >(){return "L";}
  template<> std::string typeRoot<ULong64_t     >(){return "l";}

  constexpr int defaultBufferSize = 256000;
  
  /// @brief Create a branch for a given value and name
  template<class T>
  auto createBranch(TTree* tree, std::string const & name, T * value, int buffsize = defaultBufferSize)
  {
    std::string type_root_format = name+"/"+typeRoot<T>();
    return (tree -> Branch(name.c_str(), value, type_root_format.c_str(), buffsize));
  }

  template<class T>
  TBranch* createBranch(TTree* tree, std::string const & name, std::vector<T> * vec, int buffsize = defaultBufferSize)
  {
    // Uses ROOT Object I/O. Passing a leaflist string here is invalid.
    return tree->Branch(name.c_str(), vec, buffsize);
  }

  /// @brief Create a branch for a given array and name
  /// @param name_size: The name of the leaf that holds the size of the array (like the event multiplicity)
  template<class T>
  TBranch* createBranchArray(TTree* tree, std::string const & name, T * array, std::string const & name_size, int buffsize = defaultBufferSize)
  {
    std::string type_root_format = name + "[" + name_size + "]/" + typeRoot<std::remove_extent_t<T>>();
    return tree->Branch(name.c_str(), array, type_root_format.c_str(), buffsize);
  }
}