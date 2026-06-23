#ifndef HITSIPM_HPP
#define HITSIPM_HPP

#include "libCo.hpp"

template<size_t _size = 64>
class HitSiPM
{
public:
  HitSiPM() {}

  void reset()
  {
    timestamp   = {};
    relative_ts = {};
    id          = {};
    number_hits = {};

    for (size_t i = 0; i<_size; ++i)
    {
      HGs [i] = {};
      LGs [i] = {};
      ToTs[i] = {};
      ToAs[i] = {};
    }
  }
  
  friend std::ostream& operator<<(std::ostream& out, HitSiPM<_size> const & hit)
  {
    print("Size :", _size, "hits. Timestamp :", hit.timestamp, "us. Relative timestamp :", hit.relative_ts, "Trigger ID :", hit.trig_id);
    print("hit_i\tHG\tLG\tToT\tToA");
    for (int i = 0; i<_size; ++i)
    {
      if (0 < hit.HGs[i] || 0 < hit.LGs[i] || 0 < hit.ToTs[i] || 0 < hit.ToAs[i])
        printT(i, hit.HGs[i], hit.LGs[i], hit.ToTs[i], hit.ToAs[i]);
    }
    return out;
  }

  uint16_t number_hits{};
  uint16_t counter{};
  double timestamp{}, relative_ts{};
  uint64_t id{}, trig_id{};
  
  double HGs [_size] {{}}, LGs [_size] {{}}, ToTs[_size] {{}}, ToAs[_size] {{}};

  auto const & size() {return m_size;}

protected:
  size_t m_size {_size};
};

#endif //HITSIPM_HPP