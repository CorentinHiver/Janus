#ifndef READER5052_HPP
#define READER5052_HPP

#include "LibCo/libRoot.hpp"
#include "HitSiPM.hpp"

template<size_t _size>
class Reader5052
{
  // Acquisition Mode 5202
  int ACQMODE_SPECT	 = 0b0001 ; // 1  Spectroscopy Mode (Energy)
  int ACQMODE_TIMING = 0b0010 ; // 2  Timing Mode 
  int ACQMODE_TSPECT = 0b0011 ; // 3  Spectroscopy + Timing Mode (Energy + Tstamp)
  int ACQMODE_COUNT	 = 0b0100 ; // 4  Counting Mode (MCS)
  int ACQMODE_WAVE	 = 0b1000 ; // 8  Waveform Mode

  // Data Qualifier 5202
  int DTQ_SPECT   = 0b00000001 ; // 1    Spectroscopy Mode (Energy)
  int DTQ_TIMING  = 0b00000010 ; // 2    Timing Mode 
  int DTQ_TSPECT  = 0b00000011 ; // 3    Spectroscopy + Timing Mode (Energy + Tstamp)
  int DTQ_COUNT   = 0b00000100 ; // 4    Counting Mode (MCS)
  int DTQ_WAVE    = 0b00001000 ; // 8    Waveform Mode
  int DTQ_RTSTAMP = 0b10000000 ; // 128  Relative Timestamp
  
  // Data Type 5202
  int LG   = 0b000001 ; // 1   Low Gain
  int HG   = 0b000010 ; // 2   High Gain
  int TOA  = 0b010000 ; // 16  Time Of Arrival
  int TOT  = 0b100000 ; // 32  Time Over Threshold
  
  std::unordered_map<int, std::string> status_dictonnary = 
  {
    {ACQMODE_SPECT  , "ACQMODE_SPECT" },
    {ACQMODE_TIMING , "ACQMODE_TIMING"},
    {ACQMODE_TSPECT , "ACQMODE_TSPECT"},
    {ACQMODE_COUNT  , "ACQMODE_COUNT" },
    {ACQMODE_WAVE   , "ACQMODE_WAVE"  },
    {DTQ_SPECT  , "DTQ_SPECT"  },
    {DTQ_TIMING , "DTQ_TIMING" },
    {DTQ_TSPECT , "DTQ_TSPECT" },
    {DTQ_COUNT  , "DTQ_COUNT"  },
    {DTQ_WAVE   , "DTQ_WAVE"   },
    {DTQ_RTSTAMP, "DTQ_RTSTAMP"},
    {LG , "LG"  },
    {HG , "HG"  },
    {TOA, "TOA" },
    {TOT, "TOT" }
  };

  constexpr bool relTimestamp(uint8_t const & acq_mode){return (acq_mode & 0xF0 == DTQ_RTSTAMP);}
  
public:

  Reader5052(){}
  Reader5052(std::string const & filename): m_filename(filename) {open();}
  ~Reader5052() {if (m_datafile.is_open()) m_datafile.close();}

  void initOutput()
  {
    if (!m_datafile.is_open()) throw_error("Open the datafile before initializing the output");
    if (m_output_init) return;
    m_tree = new TTree("SiPM","SiPM");
    m_tree->SetDirectory(nullptr);
    createBranch(m_tree, &m_size, "size");
    createBranch(m_tree, &m_hit.timestamp, "timestamp");
    createBranchArray(m_tree, &m_hit.HGs , "HG" , "size");
    createBranchArray(m_tree, &m_hit.LGs , "LG" , "size");
    createBranchArray(m_tree, &m_hit.ToTs, "ToT", "size");
    createBranchArray(m_tree, &m_hit.ToAs, "ToA", "size");
    m_output_init = true;
  }

  void open(std::string filename = "")
  {
    if (filename != "") m_filename = filename;
    m_datafile.open(m_filename,std::ios::binary | std::ios::in);
    if (!m_datafile.is_open())
    {
      throw_error("Could not open file '"+ m_filename+"'");
      return;
    }
    readHeader();   
  }

  void readHeader()
  {
    if (!m_datafile.is_open()) throw_error("Open the datafile before reading the header");
    if (m_header_read) return;

    // Read Data Format Version
    read_buff(&tmp_8);
    m_data_version = std::to_string(tmp_8) + ".";
    m_data_format = 10 * tmp_8;
    read_buff(&tmp_8);
    m_data_format += tmp_8;
    m_data_version += std::to_string(tmp_8);

    for (int i = 0; i < 3; ++i) 
    {
      read_buff(&tmp_8);
      if (i != 0) m_software_version += ".";
      m_software_version += std::to_string(tmp_8);
    }

    if (m_data_format >= 32) 
    {
      read_buff(&m_board_version);
      read_buff(&m_run_number);
    }

    read_buff(&m_acquisition_mode); // Read m_acquisition_mode

    if (m_data_format >= 31) 
    { // From V3.1 the header include OutFileUnit(8bits), EN_BIN (16bits), LSB_ns (32bits, float) 
      read_buff(&m_en_bin); // EnBin
      read_buff(&m_time_unit); // Is time in LSB (0) or ns (1)
      read_buff(&m_LSB_ns); // Value of LSB in ns
    }

    read_buff(&m_start_run);

    // Calculate the file size :
    m_data_begin_pos = m_datafile.tellg(); // Data starts after the header, hence current position
    m_datafile.seekg(0, std::ios::end); // Go to the end of the file
    m_data_end_pos = m_datafile.tellg(); // Register the position of the end of the file
    m_data_size = m_data_end_pos - m_data_begin_pos; // Calculate size
    m_datafile.seekg(m_data_begin_pos, std::ios::beg); // Returning to the beginning of the data

    m_header_read = true;
    printHeader();
  }

  void printHeader()
  {
    print("Software version:", m_data_version);
    print("data format" , int(m_data_format));
    print("board version", int(m_board_version));
    print("run number", int(m_run_number));
    print("acquisition mode", status_dictonnary.at(m_acquisition_mode));
    print("en bin", int(m_en_bin));
    print("time unit", int(m_time_unit));
    print("LSB_ns", m_LSB_ns);
    print("data size", m_data_size, "o");
  }

  bool readEvent()
  {
    m_hit.reset();
    
    std::size_t read_size = 0; // Increment the read size for every read_buff call until its size exceeds the size of the event in bytes (m_event_bin_size)
    read_buff(&m_event_bin_size , read_size);
    if (m_datafile.eof()) return false; // eof is activated only after a failed reading

    read_buff(&m_board          , read_size);
    read_buff(&m_hit.timestamp  , read_size);
    
    if (m_acquisition_mode & DTQ_RTSTAMP) read_buff(&m_rel_timestamp, read_size);

    auto timingMode = [this](){return ((m_acquisition_mode & 0X0F) == DTQ_TIMING);};
    auto relativeTimestampMode = [this](){return (m_acquisition_mode & DTQ_RTSTAMP);};

    if (timingMode()) read_buff(&m_hit.number_hits , read_size);
    else 
    {
      read_buff(&m_hit.hit_id  , read_size);
      read_buff(&m_channel_mask, read_size);
    }

    if (m_hit.hit_id % int(1e4) == 0) print(nicer_double(m_hit.hit_id, 0));
    
    while (read_size < m_event_bin_size)
    { // Looping through all the written channels :
           if (m_acquisition_mode & DTQ_TSPECT) readTimeOrSpectroChannel(read_size); // Spect Or Time
      else if (m_acquisition_mode & DTQ_COUNT ) readCountChannel              (read_size);  // Count mode
      else throw_error("Don't know acquisition mode "+std::to_string(int(m_acquisition_mode)));
      
      if (!timingMode()) ++m_hit.number_hits;
    }
    return true;
 }

  void readTimeOrSpectroChannel(size_t & read_size)
  {
    uint8_t  channel_id;
    uint8_t  data_type;

    read_buff(&channel_id, read_size);
    read_buff(&data_type , read_size);

    if (channel_id>=m_hit.size()) error(channel_id, ">", m_hit.size());

    if (data_type & LG ) {
      read_buff(&tmp_u16, read_size);
      m_hit.LGs[channel_id] = tmp_u16;
    }
    if (data_type & HG ) {
      read_buff(&tmp_u16, read_size);
      m_hit.HGs[channel_id] = tmp_u16;
    }
    if (data_type & TOA) {
      if (m_time_unit) { // Default is 0. If ver > 3.1 it can be 1, that means time is given as float
        read_buff(&tmp_f  , read_size);
        m_hit.ToAs[channel_id] = static_cast<double>(tmp_f  );
      } else {
        read_buff(&tmp_u32, read_size);
        m_hit.ToAs[channel_id] = static_cast<double>(tmp_u32)/m_LSB_ns;
      }
    }
    if (data_type & TOT) {
      if (m_time_unit) { // Default is 0. If ver > 3.1 it can be 1, that means time is given as float
        read_buff(&tmp_f  , read_size);
        m_hit.ToTs[channel_id] = static_cast<double>(tmp_f  );
      } else {
        read_buff(&tmp_u16, read_size);
        m_hit.ToTs[channel_id] = static_cast<double>(tmp_u16)/m_LSB_ns;
      }
    }
    if (data_type == 0) 
    {
      error("Can't read data type 0");
      print(
        "channel_id ", int(channel_id),
        "data_type 0x", std::hex, int(data_type), std::dec, 
        "m_event_bin_size ", m_event_bin_size,
        "m_board ", int(m_board),         
        "m_timestamp ", m_timestamp
      );
    }
  }

  void fillTree() {m_tree -> Fill();}

  void write(std::string const & filename, std::string const & mode)
  {
    auto file = TFile::Open(filename.c_str(), mode.c_str());
    file   -> cd   ();
    m_tree -> Write();
    file   -> Close();
    print(filename, "written");
  }

  void readCountChannel(size_t & read_size) 
  {
    uint8_t channel_id; 
    read_buff(&channel_id, read_size);
    read_buff(&m_hit.counter, read_size);
  }

  bool const end() {return m_datafile.eof();}
  
private:
  std::ifstream m_datafile;
  std::string m_filename;
  HitSiPM<_size> m_hit;
  TTree *m_tree = nullptr;
  size_t m_size = _size;

  std::string event_str;

  bool m_header_read = false;
  bool m_output_init = false;

  std::streampos m_data_begin_pos, m_data_end_pos;
  std::streamoff m_data_size;

  std::string m_data_version;
  std::string m_software_version;

  // File informations :
  uint8_t  m_data_format      = 0 ;
  uint8_t  m_acquisition_mode = 0 ;
  uint8_t  m_time_unit        = 0 ; // ToA or ToT written as int (LSB) or float (ns)
  uint16_t m_board_version    = 0 ;
  uint16_t m_run_number       = 0 ;
  uint16_t m_en_bin           = 0 ;
  uint64_t m_start_run        = 0 ;
  float    m_LSB_ns           = 0.;

  void resetFileInformations()
  {
    m_data_format      = 0 ;
    m_acquisition_mode = 0 ;
    m_time_unit        = 0 ;
    m_board_version    = 0 ;
    m_run_number       = 0 ;
    m_en_bin           = 0 ;
    m_start_run        = 0 ;
    m_LSB_ns           = 0.;
  }
  
  // Event informations :
  uint8_t  m_board          = 0 ;
  uint16_t m_event_bin_size = 0 ;
  uint64_t m_channel_mask   = 0 ;
  double   m_timestamp      = 0.;
  double   m_rel_timestamp  = 0.;

  void resetEventInformations()
  {
    m_board          = 0 ;
    m_event_bin_size = 0 ;
    m_channel_mask   = 0 ;
    m_timestamp      = 0.;
    m_rel_timestamp  = 0.;
  }

  // Helper members :
  uint8_t  tmp_8   = 0 ;
  uint16_t tmp_u16 = 0 ;
  uint32_t tmp_u32 = 0 ;
  float    tmp_f   = 0.;

  template<class Type>
  auto & read_buff(Type * buff) {return m_datafile.read(reinterpret_cast<char*>(buff), sizeof(Type));}

  template<class Type>
  auto & read_buff(Type * buff, size_t & size_read) 
  {
    auto & ret = read_buff(buff);
    if (ret) size_read += sizeof(Type);
    return ret;
  }
};

#endif //READER5052_HPP

/*
// Acquisition Mode 5202
#define ACQMODE_SPECT		0b0001  // Spectroscopy Mode (Energy)
#define ACQMODE_TIMING	0b0010  // Timing Mode 
#define ACQMODE_TSPECT	0b0011  // Spectroscopy + Timing Mode (Energy + Tstamp)
#define ACQMODE_COUNT		0b0100  // Counting Mode (MCS)
#define ACQMODE_WAVE		0b1000  // Waveform Mode

std::unordered_map<int, std::string> acquisitionModes = 
{
  {ACQMODE_SPECT, "ACQMODE_SPECT"},
  {ACQMODE_TIMING, "ACQMODE_TIMING"},
  {ACQMODE_TSPECT, "ACQMODE_TSPECT"},
  {ACQMODE_COUNT, "ACQMODE_COUNT"},
  {ACQMODE_WAVE, "ACQMODE_WAVE"}
};

// Data Qualifier 5202
#define DTQ_SPECT			  0b00000001  // Spectroscopy Mode (Energy)
#define DTQ_TIMING			0b00000010  // Timing Mode 
#define DTQ_TSPECT			0b00000011  // Spectroscopy + Timing Mode (Energy + Tstamp)
#define DTQ_COUNT 			0b00000100  // Counting Mode (MCS)
#define DTQ_WAVE			  0b00001000  // Waveform Mode
#define DTQ_RTSTAMP     0b10000000  // Relative Timestamp

// Data Type 5202
#define LG              0b000001
#define HG              0b000010
#define TOA             0b010000
#define TOT             0b100000

static constexpr bool relTimestamp(uint8_t const & acq_mode){return (acq_mode & 0xF0 == DTQ_RTSTAMP);}

*/