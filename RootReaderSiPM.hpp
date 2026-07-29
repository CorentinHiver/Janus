#pragma once

#include "RootHitSiPM.hpp"
class RootReaderSiPM
{
public:
  RootReaderSiPM() noexcept = default;
  ~RootReaderSiPM() noexcept
  {
    if (m_file && !m_file->IsZombie() && m_file->IsOpen()) m_file->Close();
  }
  RootReaderSiPM(std::string const & filename)
  {
    open(filename);
  }
  RootReaderSiPM(TFile* file)
  {
    load(file);
  }

  void open(std::string const & filename)
  {
    m_file = TFile::Open(filename.c_str(), "read");
    if (!m_file) throw std::runtime_error(filename + " could not be open");
    load(m_file);
  }

  void load(TFile* file)
  {
    m_file = file;
    if (!m_file) throw std::runtime_error("file could not be open !");
    m_filename = m_file->GetName();
    m_tree = m_file->Get<TTree>("SiPM");
    if (!m_tree) throw std::runtime_error("tree SiPM could not be found in " + m_filename);
    m_entries = m_tree->GetEntries();
    m_hit.readFrom(m_tree);
  }

  bool readNext()
  {
    if (++m_cursor < m_entries) {m_tree->GetEntry(m_cursor); return true;}
    else return false;
  }

  auto & getHitAccess() {return m_hit;}

private:
  std::string m_filename;
  RootHitSiPM m_hit;
  TFile* m_file{};
  TTree* m_tree{};
  Long64_t m_entries{}, m_cursor{};
};