// TODO:  remplace the static array logic into a vector logic for flexibility

#ifndef READER5052_HPP
#define READER5052_HPP

#include "LibCo/libRoot.hpp"
#include "HitSiPM.hpp"
void ms_to_date(long long ms, int timeZone = 0)
{
  time_t sec = (time_t)(ms / 1000);
  struct tm *tm_info = localtime(&sec);   // use localtime(&sec) for local time

  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
  printf("%s\n", buf);
}
template<typename T>
void print_binary(T n)
{
  int bits = sizeof(n) * 8;
  for (int i = bits - 1; i >= 0; --i) putchar((n & (1u << i)) ? '1' : '0');
  putchar('\n');
}

template<size_t _size>
class Reader5052
{
  // Acquisition Mode 5202
  constexpr static int ACQMODE_SPECT	= 0b0001 ; // 1  Spectroscopy Mode (Energy)
  constexpr static int ACQMODE_TIMING = 0b0010 ; // 2  Timing Mode 
  constexpr static int ACQMODE_TSPECT = 0b0011 ; // 3  Spectroscopy + Timing Mode (Energy + Tstamp)
  constexpr static int ACQMODE_COUNT	= 0b0100 ; // 4  Counting Mode (MCS)
  constexpr static int ACQMODE_WAVE	  = 0b1000 ; // 8  Waveform Mode

  // Data Qualifier 5202
  constexpr static int DTQ_SPECT   = 0b00000001 ; // 1    Spectroscopy Mode (Energy)
  constexpr static int DTQ_TIMING  = 0b00000010 ; // 2    Timing Mode 
  constexpr static int DTQ_TSPECT  = 0b00000011 ; // 3    Spectroscopy + Timing Mode (Energy + Tstamp)
  constexpr static int DTQ_COUNT   = 0b00000100 ; // 4    Counting Mode (MCS)
  constexpr static int DTQ_WAVE    = 0b00001000 ; // 8    Waveform Mode
  constexpr static int DTQ_RTSTAMP = 0b10000000 ; // 128  Relative Timestamp

  constexpr static int DTQ_MODE_MASK = 0b00000111;
  constexpr static int DTQ_RTSTAMP_MASK = 0b10000000;
  
  // Data Type 5202
  constexpr static int LG   = 0b000001 ; // 1   Low Gain
  constexpr static int HG   = 0b000010 ; // 2   High Gain
  constexpr static int TOA  = 0b010000 ; // 16  Time Of Arrival
  constexpr static int TOT  = 0b100000 ; // 32  Time Over Threshold

  constexpr static std::string dataTypeString(int type)
  {
    std::string ret;
    if (type & LG)  ret += "LG " ;
    if (type & HG)  ret += "HG " ;
    if (type & TOA) ret += "TOA ";
    if (type & TOT) ret += "TOT ";
    return ret;
  }
  
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
  };

  constexpr bool relTimestamp(uint8_t const & acq_mode) {return (acq_mode & DTQ_RTSTAMP_MASK == DTQ_RTSTAMP);}
  
public:

  Reader5052(){}
  Reader5052(std::string const & filename): m_filename(filename) {open();}
  ~Reader5052() 
  {
    // Force closing of input and output files
    if (m_datafile.is_open()) m_datafile.close();
    if (m_file && m_file->IsOpen() && !m_tree->IsZombie()) 
    {
      if (m_tree) m_tree->Write();
      m_file->Close();
    }
  }

  void setMaxHits(int nb)
  {
    m_maxHits = nb;
    m_maxHitsSet = true;
  }

  void initOutput()
  {
    if (!m_datafile.is_open()) Colib::throw_error("Open the datafile before initializing the output");
    if (m_output_init) return;
    m_tree = new TTree("SiPM","SiPM");
    // m_tree->SetDirectory(nullptr);
    Colib::createBranch(m_tree, &m_size, "size");
    Colib::createBranch(m_tree, &m_hit.timestamp, "timestamp");
    Colib::createBranchArray(m_tree, &m_hit.HGs , "HG" , "size");
    Colib::createBranchArray(m_tree, &m_hit.LGs , "LG" , "size");
    Colib::createBranchArray(m_tree, &m_hit.ToTs, "ToT", "size");
    Colib::createBranchArray(m_tree, &m_hit.ToAs, "ToA", "size");

    if (m_autoSave) m_tree -> SetAutoSave();
    
    m_output_init = true;
  }

  void setRootOutput(std::string const & filename, std::string const & mode)
  {
    m_rootFilename = filename;
    m_file = TFile::Open(filename.c_str(), mode.c_str());
    m_file   -> cd   ();
  }

  /// @brief Recommended initialisation method (you won't have to worry about ROOT memory management)
  void initRootOutput(std::string const & filename, std::string const & mode)
  {
    initOutput();
    setRootOutput(filename, mode);
  }

  void open(std::string filename = "")
  {
    if (filename != "") m_filename = filename;
    if (Colib::extension(m_filename) != "dat")
    {
      error(m_filename, "is not a .dat file !!");
      return;
    }
    m_datafile.open(m_filename,std::ios::binary | std::ios::in);
    if (Colib::sizeFileConversion(Colib::sizeFile(m_datafile), "o", "Mo") > 100)
    {
      print("Autosaving on");
      m_autoSave = true;
      if (m_output_init) m_tree -> SetAutoSave(); // If m_tree already created, apply it directly
    }
    if (!m_datafile.is_open())
    {
      Colib::throw_error("Could not open file '"+ m_filename+"'");
      return;
    }
    readHeader();   
  }

  void readHeader()
  {
    if (!m_datafile.is_open()) Colib::throw_error("Open the datafile before reading the header");
    if (m_header_read) return;

    // Read Data Format Version
    read_buff(&tmp_8);
    m_data_version = std::to_string(tmp_8) + ".";
    m_data_format = 10 * tmp_8;
    read_buff(&tmp_8);
    m_data_version += std::to_string(tmp_8);
    m_data_format += tmp_8;

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

    read_buff(&m_acquisition_mode);

    if (m_data_format >= 31) 
    { // From V3.1 the header include OutFileUnit(8bits), EN_BIN (16bits), LSB_ns (32bits, float) 
      read_buff(&n_spectro_bin);    // Number of spectroscopy channels (typically 4096)
      read_buff(&m_time_unit); // Time stored in LSB (0) or ns (1)
      read_buff(&m_LSB_ns);    // Value of LSB in ns (useless if m_time_unit = 1)
    }

    read_buff(&m_start_run);

    // Calculate the file size :
    m_data_begin_pos = m_datafile.tellg();             // Data starts after the header, hence current position
    m_datafile.seekg(0, std::ios::end);                // Go to the end of the file
    m_data_end_pos = m_datafile.tellg();               // Register the position of the end of the file
    m_data_size = m_data_end_pos - m_data_begin_pos;   // Calculate size
    m_datafile.seekg(m_data_begin_pos, std::ios::beg); // Returning to the beginning of the data

    m_header_read = true;
    printHeader();
  }

  bool readEvent()
  {
    if (m_maxHitsSet && m_maxHits < m_hit_i) return false;
    m_hit_i++;
    m_hit.reset();                            // Reset hit from last event (event means a hit in caen documentation)
    std::size_t read_size = 0;                // Increment the read size for every read_buff call until its size exceeds the size of the event in bytes (m_event_bin_size)
    read_buff(&m_event_bin_size , read_size); // Tries to read the data stream
    if (m_datafile.eof()) return false;       // eof is activated only after a failed reading
    
    debug();
    debug("hit", m_hit_i, m_event_bin_size, "bits to read");

    read_buff(&m_board          , read_size); // Read the board type
    read_buff(&m_hit.timestamp  , read_size); // Read the timestamp

    debug("m_acquisition_mode");
    if ((m_acquisition_mode & DTQ_RTSTAMP_MASK) || (m_acquisition_mode & DTQ_TSPECT)) 
    {
      read_buff(&m_hit.relative_ts  , read_size); // Read the relative timestamp if activated
    }
    read_buff(&m_hit.number_hits  , read_size); // hit nb

    debug("board", int(m_board), m_hit.number_hits, "hits : timestamp", m_hit.timestamp,"us rel timestamp",  m_hit.relative_ts, "us");

    auto timingMode = (m_acquisition_mode & DTQ_TIMING);

    // if (timingMode) read_buff(&m_hit.number_hits , read_size);
    // if (!timingMode)
    // {
      read_buff(&m_hit.trig_id , read_size);
      read_buff(&m_channel_mask, read_size);
      // print_binary(m_hit.trig_id);
      // print_binary(m_channel_mask);
    // }

    if (m_hit.id % int(1e4) == 0) std::cout << "\rProgress: " << std::fixed << std::setprecision(1) << getSizeReadRel() << "%   " << std::flush;

    int i = 0;
    while (read_size < m_event_bin_size)
    { // Looping through all the written channels :
      debug("channel", i++, read_size, "bits read");
           if (m_acquisition_mode & DTQ_TSPECT) readTimeOrSpectroChannel(read_size); // Spect Or Time or Both
      else if (m_acquisition_mode & DTQ_COUNT ) readCountChannel        (read_size); // Count mode
      else Colib::throw_error("Don't know acquisition mode "+std::to_string(int(m_acquisition_mode)));
      
      // if (!timingMode) ++m_hit.number_hits; // TODO CHECK if this should be commented or not
    }
    debug(m_event_bin_size, "bits to read", read_size, "bits read");
    ++m_cursor;
    return true;
 }
 
  void readTimeOrSpectroChannel(size_t & read_size)
  {
    uint8_t channel_id;
    uint8_t data_type ;

    read_buff(&channel_id, read_size);
    read_buff(&data_type , read_size);

    debug("channel id", int(channel_id), "channel data type", std::bitset<8>(data_type), "=", dataTypeString(data_type));

    if (m_hit.size() < size_cast(channel_id)) {error(int(channel_id), ">", m_hit.size()); Colib::pause();}

    if (data_type & LG ) {             // Read Low Gain
      read_buff(&tmp_u16, read_size);
      m_hit.LGs[channel_id] = tmp_u16;
    }
    if (data_type & HG ) {             // Read High Gain
      read_buff(&tmp_u16, read_size);
      m_hit.HGs[channel_id] = tmp_u16;
    }
    if (data_type & TOA) {             // Read Time Of Arrival
      if (m_time_unit) { // Default is 0. If ver > 3.1 it can be 1, that means time is given as float
        read_buff(&tmp_f  , read_size);
        m_hit.ToAs[channel_id] = double_cast(tmp_f  );
      } else {
        read_buff(&tmp_u32, read_size);
        m_hit.ToAs[channel_id] = double_cast(tmp_u32)/m_LSB_ns;
      }
    }
    if (data_type & TOT) {             // Read Time Over Threshold
      if (m_time_unit) { // Default is 0. If ver > 3.1 it can be 1, that means time is given as float in ns directly
        read_buff(&tmp_f  , read_size);
        m_hit.ToTs[channel_id] = double_cast(tmp_f  );
      } else {
        read_buff(&tmp_u16, read_size);
        m_hit.ToTs[channel_id] = double_cast(tmp_u16)/m_LSB_ns;
      }
    }
    if (data_type == 0) 
    {
      error("Can't read data type 0");
      error(
        "channel_id "      , int(channel_id) ,
        "data_type 0x"     , std::hex        , int(data_type), std::dec, 
        "m_event_bin_size ", m_event_bin_size,
        "m_board "         , int(m_board)    ,         
        "timestamp "       , m_hit.timestamp
      );
    }
    else debug(      
        "channel_id "      , int(channel_id) ,
        "data_type 0x"     , std::hex        , int(data_type), std::dec, 
        "m_event_bin_size ", m_event_bin_size,
        "m_board "         , int(m_board)    ,         
        "timestamp "       , m_hit.timestamp
      );
  }

  /// @brief Get the amount of data already read
  auto getSizeRead(std::string unit = "o")
  {
    return Colib::sizeFileConversion(m_datafile.tellg(), "o", unit);
  }

  auto const & getCursor() const {return m_cursor;}

  // /// @brief Get the amount of data already read
  // auto getSizeRead()
  // {
  //   return Colib::sizeFileBestUnit(m_datafile.tellg());
  // }
  
  /// @brief Get the total size of the file
  auto getSizeFile()
  {
    return Colib::sizeFileBestUnitString(m_datafile);
  }

  /// @brief Get the amount of data already read divided by the size of the file
  auto getSizeReadRel()
  {
    return 100.*m_datafile.tellg() / m_data_end_pos;
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

  /// @brief Recommended writting method (you won't have to worry about ROOT memory management)
  void write()
  {
    print("writting", m_rootFilename);
    if (!m_file) Colib::throw_error("In Reader5052::write() : no file !! Either use Reader5052::setRootOutput or Reader5052::write(string filename, string mode)");
    m_file -> cd();
    if (!m_tree) Colib::throw_error("In Reader5052::write() : no tree !!");
    m_tree -> Write();
    print(m_tree->GetName(), "written");
    m_file -> Close();
  }
  
  bool const end() {return m_datafile.eof();}

  auto & getTree() {return m_tree;}

  auto const & getHit() const{return m_hit;}

  void printHeader()
  {
    print("Software version:", m_data_version       );
    print("data format"      , int(m_data_format  ) );
    print("board version"    , int(m_board_version) );
    print("run number"       , int(m_run_number   ) );
    print("acquisition mode" , status_dictonnary.at(m_acquisition_mode));
    print("n spectro bin"    , int(n_spectro_bin  ) );
    print("time unit"        , int(m_time_unit    ) );
    print("LSB_ns"           , m_LSB_ns             );
    print("data size"        , getSizeFile()        );
    print("start timestamp"  , m_start_run          );
    ms_to_date(m_start_run);
  }

  
private:

  // Internal methods :

  void readCountChannel(size_t & read_size) 
  {
    error("Count channel !!!!!!! Not developped .... Sorry");
    uint8_t channel_id; 
    read_buff(&channel_id, read_size);
    read_buff(&m_hit.counter, read_size);
  }

  // Attributes :

  uint8_t  m_data_format      = 0 ;
  uint8_t  m_acquisition_mode = 0 ;
  uint8_t  m_time_unit        = 0 ; // ToA or ToT written as int (LSB) or float (ns)
  uint16_t m_board_version    = 0 ;
  uint16_t m_run_number       = 0 ;
  uint16_t n_spectro_bin      = 0 ; // Number of spectroscopy channels (typically 4096)
  uint64_t m_start_run        = 0 ; // Timestamp of the start of the run (relative to 01/01/1970)
  float    m_LSB_ns           = 0.; // In the case of LSB, conversion factor

  // Event informations :

  uint8_t  m_board          = 0 ;
  uint16_t m_event_bin_size = 0 ;
  uint64_t m_channel_mask   = 0 ;

  std::string m_rootFilename;
  TFile * m_file = nullptr;
  TTree * m_tree = nullptr;

  bool m_autoSave{}, m_maxHitsSet{};
  int m_maxHits = 0;

  std::ifstream m_datafile;
  std::string m_filename;

  HitSiPM<_size> m_hit;
  size_t m_size = _size;
  size_t m_cursor{};
  
  // std::string event_str; UNUSED

  bool m_header_read = false;
  bool m_output_init = false;

  // File informations :

  std::streampos m_data_begin_pos, m_data_end_pos;
  std::streamoff m_data_size; // Size of the .dat file in o

  std::string m_data_version;
  std::string m_software_version;

  void resetFileInformations()
  {
    m_data_format      = 0 ;
    m_acquisition_mode = 0 ;
    m_time_unit        = 0 ;
    m_board_version    = 0 ;
    m_run_number       = 0 ;
    n_spectro_bin      = 0 ;
    m_start_run        = 0 ;
    m_LSB_ns           = 0.;
  }
  
  void resetEventInformations()
  {
    m_board          = 0 ;
    m_event_bin_size = 0 ;
    m_channel_mask   = 0 ;
  }

  // Helper members :

  uint8_t  tmp_8   = 0 ;
  uint16_t tmp_u16 = 0 ;
  uint32_t tmp_u32 = 0 ;
  uint64_t tmp_u64 = 0 ;
  float    tmp_f   = 0.;
  size_t   m_hit_i = 0 ;

  template<class T>
  auto & read_buff(T * buff) {return m_datafile.read(reinterpret_cast<char*>(buff), sizeof(T));}

  template<class T>
  auto & read_buff(T * buff, size_t & size_read) 
  {
    auto & ret = read_buff(buff);
    if (ret) size_read += sizeof(T);
    return ret;
  }
};

#endif //READER5052_HPP