#ifndef HITSIPM
#define HITSIPM

#include "LibCo/libCo.hpp"
// #include "MyBinaryData.hpp"

template<size_t _size>
class HitSiPM
{
public:
  HitSiPM() {}

  void reset()
  {
    timestamp = 0;
    hit_id = 0;
    number_hits = 0;
    for (int i = 0; i<_size; ++i)
    {
      HGs[i] = 0;
      LGs[i] = 0;
      ToTs[i] = 0;
      ToAs[i] = 0;
    }
  }
  
  friend std::ostream& operator<<(std::ostream& out, HitSiPM<_size> const & hit)
  {
    print("Size :", _size, "hits. Timestamp :", hit.timestamp, "ns. Trigger ID :", hit.hit_id);
    print("hit_i\tHG\tLG\tToT\tToA");
    for (int i = 0; i<_size; ++i)
    {
      printT(i, hit.HGs[i], hit.LGs[i], hit.ToTs[i], hit.ToAs[i]);
    }
    return out;
  }

  double   timestamp   = 0;
  uint64_t hit_id      = 0;
  uint16_t number_hits = 0;
  uint32_t counter     = 0;
  double HGs[_size] = {0};
  double LGs[_size] = {0};
  double ToTs[_size] = {0};
  double ToAs[_size] = {0};

  auto const & size() {return m_size;}

private:
  size_t m_size = _size;
};

#endif //HITSIPM


// /// @brief @deprecated
  // void readBin(MyBinaryData const & data)
  // {//! Deprecated
  //   float time_factor = 1; // Conversion factor from LSB to ns. It changes if force_ns is 1 (true)
  //   if (data.t_force_ns)
  //       time_factor = data.t_LSB_ns;

  //   if (data.t_ch_id.size() > _size) {print("size issue :", data.t_ch_id.size(), ";", _size); return;}

  //   timestamp = data.t_tstamp;

  //   for (uint32_t i = 0; i < data.t_ch_id.size(); ++i)
  //   {
  //     HGs[i] = data.t_PHA_HG[i];
  //     LGs[i] = data.t_PHA_LG[i];
      
  //     if (data.t_time_unit)
  //     {
  //       ToTs[i] = data.t_ToT_f[i];
  //       ToAs[i] = data.t_ToA_f[i];
  //     } 
  //     else
  //     {
  //       ToTs[i] = time_factor*data.t_ToT_i[i];
  //       ToAs[i] = time_factor*data.t_ToA_i[i];
  //     }
  //   }
  // }

  // /**
  //  * @brief Reads the binary data and clears the initial reader
  //  * @deprecated
  //  * 
  //  * @param data 
  //  */
  // void siphonData(MyBinaryData& data)
  // {//! Deprecated
  //   this->readBin(data);
  //   data.ResetValues();
  // }