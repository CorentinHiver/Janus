#ifndef HITSIPM_HPP
#define HITSIPM_HPP

#include "libCo.hpp"

class HitSiPM
{
public:
  HitSiPM() 
    : HGs(new std::vector<double>())
    , LGs(new std::vector<double>())
    , ToTs(new std::vector<double>())
    , ToAs(new std::vector<double>())
    , IDs(new std::vector<int>())
  {}

  ~HitSiPM() noexcept
  {
    delete HGs;
    delete LGs;
    delete ToTs;
    delete ToAs;
    delete IDs;
  }

  HitSiPM(HitSiPM const &) = delete;
  HitSiPM& operator=(HitSiPM const &) = delete;

  HitSiPM(HitSiPM && other) noexcept
    : number_hits(other.number_hits)
    , counter(other.counter)
    , timestamp(other.timestamp)
    , relative_ts(other.relative_ts)
    , trig_id(other.trig_id)
    , HGs(other.HGs)
    , LGs(other.LGs)
    , ToTs(other.ToTs)
    , ToAs(other.ToAs)
    , IDs(other.IDs)
    , m_size(other.m_size)
  {
    other.HGs  = nullptr;
    other.LGs  = nullptr;
    other.ToTs = nullptr;
    other.ToAs = nullptr;
    other.IDs  = nullptr;
    other.m_size = 0;
  }

  HitSiPM& operator=(HitSiPM && other) noexcept
  {
    if (this != &other)
    {
      delete HGs;
      delete LGs;
      delete ToTs;
      delete ToAs;
      delete IDs;

      number_hits = other.number_hits;
      counter     = other.counter;
      timestamp   = other.timestamp;
      relative_ts = other.relative_ts;
      trig_id     = other.trig_id;
      
      HGs  = other.HGs;
      LGs  = other.LGs;
      ToTs = other.ToTs;
      ToAs = other.ToAs;
      IDs  = other.IDs;
      m_size = other.m_size;

      other.HGs  = nullptr;
      other.LGs  = nullptr;
      other.ToTs = nullptr;
      other.ToAs = nullptr;
      other.IDs  = nullptr;
      other.m_size = 0;
    }
    return *this;
  }

  void reset() noexcept
  {
    timestamp   = 0.0;
    relative_ts = 0.0;
    number_hits = 0;
    counter     = 0;
    trig_id     = 0;
    m_size      = 0;
    
    if (HGs)  HGs->clear();
    if (LGs)  LGs->clear();
    if (ToTs) ToTs->clear();
    if (ToAs) ToAs->clear();
    if (IDs)  IDs->clear();
  }

  std::vector<double>& getHG () noexcept { return *HGs; }
  std::vector<double>& getLG () noexcept { return *LGs; }
  std::vector<double>& getToT() noexcept { return *ToTs; }
  std::vector<double>& getToA() noexcept { return *ToAs; }
  std::vector<int>   & getID () noexcept { return *IDs; }
  
  std::vector<double> const & getHG () const noexcept { return *HGs; }
  std::vector<double> const & getLG () const noexcept { return *LGs; }
  std::vector<double> const & getToT() const noexcept { return *ToTs; }
  std::vector<double> const & getToA() const noexcept { return *ToAs; }
  std::vector<int>    const & getID () const noexcept { return *IDs; }

  double HG (size_t hit_i) const { return (*HGs)[hit_i]; }
  double LG (size_t hit_i) const { return (*LGs)[hit_i]; }
  double ToT(size_t hit_i) const { return (*ToTs)[hit_i]; }
  double ToA(size_t hit_i) const { return (*ToAs)[hit_i]; }
  int    ID (size_t hit_i) const { return (*IDs)[hit_i]; }

  void fillHG (double val) { HGs->push_back(val); }
  void fillLG (double val) { LGs->push_back(val); }
  void fillToT(double val) { ToTs->push_back(val); }
  void fillToA(double val) { ToAs->push_back(val); }
  void fillID (int    val) { IDs->push_back(val); }

  size_t size() const noexcept { return m_size; }
  void countHit() noexcept { ++m_size; }

  // ID handling (bits 0-5: channel, bits 6+: board)
  void setID(uint8_t channel_id, int board_id) 
  {
    IDs->push_back(static_cast<int>(channel_id & 0x3F) | (board_id << 6));
  }
  
  uint8_t channel_id(size_t hit_i) const 
  {
    return static_cast<uint8_t>((*IDs)[hit_i] & 0x3F);
  }
  
  int board_id(size_t hit_i) const 
  {
    return (*IDs)[hit_i] >> 6;
  }

  friend std::ostream& operator<<(std::ostream& out, HitSiPM const & hit)
  {
    print("Size :", hit.size(), "hits. Timestamp :", hit.timestamp, "us. Relative timestamp :", hit.relative_ts, "Trigger ID :", hit.trig_id);
    print("hit_i\tHG\tLG\tToT\tToA");
    for (size_t i = 0; i < hit.size(); ++i)
    {
      if (0 < hit.HG(i) || 0 < hit.LG(i) || 0 < hit.ToT(i) || 0 < hit.ToA(i))
        print(i, hit.HG(i), hit.LG(i), hit.ToT(i), hit.ToA(i));
    }
    return out;
  }

  // Champs publics
  uint16_t number_hits{}, counter{};
  double timestamp{}, relative_ts{};
  uint64_t trig_id{};

protected:
  std::vector<double>* HGs{nullptr};
  std::vector<double>* LGs{nullptr};
  std::vector<double>* ToTs{nullptr};
  std::vector<double>* ToAs{nullptr};
  std::vector<int>*    IDs{nullptr};

  size_t m_size{};
};

#endif // HITSIPM_HPP