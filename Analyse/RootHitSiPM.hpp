#pragma once

#include "../HitSiPM.hpp"
#include "libRoot.hpp"

template<size_t _size = 64>
class RootHitSiPM : public HitSiPM<_size>
{
public:
  // using Hit = HitSiPM<_size>;
  RootHitSiPM() noexcept = default;
  RootHitSiPM(TTree* tree) : m_tree(tree)
  {
    m_tree -> SetBranchAddress("size"     , &(this->m_size)   );
    m_tree -> SetBranchAddress("timestamp", &(this->timestamp));
    m_tree -> SetBranchAddress("HG"       , &(this->HGs)      );
    m_tree -> SetBranchAddress("LG"       , &(this->LGs)      );
    m_tree -> SetBranchAddress("ToT"      , &(this->ToTs)     );
    m_tree -> SetBranchAddress("ToA"      , &(this->ToAs)     );
  }

  void drawSpectra(std::string value = "HG", size_t xbins = 8, size_t ybins = 8, bool separate = false)
  {
    if (this->m_size != xbins*ybins) warning(xbins, "x", ybins, "bins !=", this->m_size, "bins !!");
    auto name = "spectraMap_"+std::to_string(map_id++);
    std::vector<TH1F*> histos; histos.reserve(this->m_size);
    for (int id{}; id<this->m_size; ++id) 
    {
      auto nameSpectrum = name+"_"+std::to_string(id);
      auto titleSpectrum = "channel " + std::to_string(id);
      histos.emplace_back(nameSpectrum.c_str(), titleSpectrum.c_str(), 4096,0,4096);
    }

        //  if (value ==  "HG") for (int hit = 0; hit < m_tree->GetEntries(); ++hit) histos.at(id) -> Fill(HG );
    // else if (value ==  "LG") for (int hit = 0; hit < m_tree->GetEntries(); ++hit) histos.at(id) -> Fill(LG );
    // else if (value == "ToA") for (int hit = 0; hit < m_tree->GetEntries(); ++hit) histos.at(id) -> Fill(ToA);
    // else if (value == "ToT") for (int hit = 0; hit < m_tree->GetEntries(); ++hit) histos.at(id) -> Fill(ToT);
    
         if (value ==  "HG") {for (int hit = 0; hit < m_tree->GetEntries(); ++hit) {for (int id{}; id<this->m_size; ++id) {histos[id] -> Fill(this->HGs [id]);}}}
    else if (value ==  "LG") {for (int hit = 0; hit < m_tree->GetEntries(); ++hit) {for (int id{}; id<this->m_size; ++id) {histos[id] -> Fill(this->LGs [id]);}}}
    else if (value == "ToA") {for (int hit = 0; hit < m_tree->GetEntries(); ++hit) {for (int id{}; id<this->m_size; ++id) {histos[id] -> Fill(this->ToAs[id]);}}}
    else if (value == "ToT") {for (int hit = 0; hit < m_tree->GetEntries(); ++hit) {for (int id{}; id<this->m_size; ++id) {histos[id] -> Fill(this->ToTs[id]);}}}
    

    auto c = new TCanvas(name.c_str(), "spectra map");
    for (int id{}; id<this->m_size; ++id) 
    {
      if (separate) 
      {
        c->cd(id+1);
        histos[id]-> SetLineColor(Colib::getROOTniceColors(id));
        histos[id] -> Draw();
      }
      else
      {
        histos[id] -> Draw((id == 0) ? "" : "same");
      }
    }
    c->SaveAs("spectra.png");
    c->SaveAs("spectra.root");
    c->SaveAs("spectra.C");
  }

  TH2F* drawHitmap(std::string value = "HG", size_t xbins = 8, size_t ybins = 8)
  {
    if (this->m_size != xbins*ybins) warning(xbins, "x", ybins, "bins !=", this->m_size, "bins !!");
    auto name = "hitmap_"+std::to_string(map_id++);
    auto map = new TH2F(name.c_str(), ("HitMap"+value).c_str(), xbins,0,xbins, ybins,0,ybins);
         if (value == "HG" ) for (int i = 0; i<this->m_size; ++i) map->SetBinContent(i, (this-> HGs)[i]);
    else if (value == "LG" ) for (int i = 0; i<this->m_size; ++i) map->SetBinContent(i, (this-> LGs)[i]);
    else if (value == "ToA") for (int i = 0; i<this->m_size; ++i) map->SetBinContent(i, (this->ToAs)[i]);
    else if (value == "ToT") for (int i = 0; i<this->m_size; ++i) map->SetBinContent(i, (this->ToTs)[i]);
    map->Draw();
    return map;
  }

private:
  TTree* m_tree{};
  size_t map_id{};
};