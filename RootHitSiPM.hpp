#pragma once

#include "HitSiPM.hpp"
#include "lib.hh"

class RootHitSiPM : public HitSiPM
{
  std::vector<int> my_ROOT_nice_colors = { 1, 2, 4, 6, 8, 9, 11, 30};
  
  auto myGetROOTniceColors(int const & i)
  { // Get the nice colors abovedefined. The "i & 111" is equivalent to "i modulo 8"
    return my_ROOT_nice_colors[(i & 111)];
  }

public:
  RootHitSiPM() noexcept = default;
  RootHitSiPM(TTree* tree) : m_tree(tree)
  {
    readFrom(tree);
  }

  void readFrom(TTree * tree)
  {
    if (!tree) Colib::throw_error("RootHitSiPM::readFrom(TTree): Tree not found !!!");
    m_tree = tree;
    m_tree -> SetBranchAddress("size"     , &(m_size   ));
    m_tree -> SetBranchAddress("timestamp", &(timestamp));
    m_tree -> SetBranchAddress("ID"       , &(IDs      ));
    m_tree -> SetBranchAddress("HG"       , &(HGs      ));
    m_tree -> SetBranchAddress("LG"       , &(LGs      ));
    m_tree -> SetBranchAddress("ToT"      , &(ToTs     ));
    m_tree -> SetBranchAddress("ToA"      , &(ToAs     ));
  }

  void writeTo(TTree * tree)
  {
    m_tree = tree;
    if (!tree) Colib::throw_error("RootHitSiPM::readFrom(TTree): Tree not found !!!");
    m_tree->Branch("size"     , &m_size   , LIB::defaultBufferSize);
    m_tree->Branch("timestamp", &timestamp, LIB::defaultBufferSize);
    m_tree->Branch("ID" , &IDs , LIB::defaultBufferSize);
    m_tree->Branch("HG" , &HGs , LIB::defaultBufferSize);
    m_tree->Branch("LG" , &LGs , LIB::defaultBufferSize);
    m_tree->Branch("ToT", &ToTs, LIB::defaultBufferSize);
    m_tree->Branch("ToA", &ToAs, LIB::defaultBufferSize);
  }

  auto fast_id(int hit_i) const {auto id = channel_id(hit_i); return id/2 + id%2;}
  auto slow_id(int hit_i) const {auto id = channel_id(hit_i); return id/2 + id%2 - 1;}
  std::pair<int, int> slow_xy(int hit_i) const
  {
    auto id_in_ASIC = slow_id(hit_i) % 16;
    return std::make_pair(id_in_ASIC % 4, id_in_ASIC / 4);
  }

  TH2F* drawChannelmap(std::string value = "HG")
  {
    auto name = "hitmap_"+std::to_string(++map_id);
    auto map = new TH2F(name.c_str(), ("HitMap"+value).c_str(), 4,0,4, 4,0,4);
    const auto* vec_ptr = [&]() -> const std::vector<double>* {
        if (value == "HG")  return HGs ;
        if (value == "LG")  return LGs ;
        if (value == "ToA") return ToAs;
        if (value == "ToT") return ToTs;
        return nullptr;
    }();
    if (!vec_ptr) return nullptr;
    auto array = map->GetArray();
    for (int i = 0; i < m_size; ++i) { array[slow_id(i)] = (*vec_ptr)[i]; }
    map->SetEntries(map->GetEntries() + m_size);
    map->Draw();
    return map;
  }

private:
  TTree* m_tree{};
  size_t map_id{};
};