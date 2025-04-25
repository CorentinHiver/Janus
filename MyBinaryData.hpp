//! From BinaryData_5052
//! These are my comments
//! Deprecated

// #include <cstdint>
// #include <cstdio>
// #include <cinttypes>
// #include <iostream>
// #include <fstream>
// #include <string>
// #include <chrono>
// #include <algorithm>
// #include <vector>
// #include <array>

#ifndef MYBINARYDATA
#define MYBINARYDATA

#include "LibCo/libCo.hpp"

#define OUTLSB  0
#define OUTNS   1

// Acquisition Mode 5202
#define ACQMODE_SPECT	0x01  // Spectroscopy Mode (Energy)
#define ACQMODE_TIMING	0x02  // Timing Mode 
#define ACQMODE_TSPECT	0x03  // Spectroscopy + Timing Mode (Energy + Tstamp)
#define ACQMODE_COUNT	0x04  // Counting Mode (MCS)
#define ACQMODE_WAVE	0x08  // Waveform Mode

static constexpr std::string_view acqMode(uint8_t const & acq_mode)
{
  switch(acq_mode & 0x0F)
  {
    case ACQMODE_SPECT  : return "SPECTRO";       break;
    case ACQMODE_TIMING : return "TIMING";        break;
    case ACQMODE_TSPECT : return "TIMING&SPECTRO";break;
    case ACQMODE_COUNT  : return "COUNTING";      break;
    case ACQMODE_WAVE   : return "WAVE";          break;
  }
}

// Data Qualifier 5202
#define DTQ_SPECT	0x01  // Spectroscopy Mode (Energy)
#define DTQ_TIMING	0x02  // Timing Mode 
#define DTQ_TSPECT	0x03  // Spectroscopy + Timing Mode (Energy + Tstamp)
#define DTQ_COUNT 	0x04  // Counting Mode (MCS)
#define DTQ_WAVE	0x08  // Waveform Mode
#define DTQ_RTSTAMP 0x80  // Relative Timestamp

// static constexpr bool relTimestamp(uint8_t const & acq_mode)
// {
//   return (acq_mode & 0xF0 == DTQ_RTSTAMP);
// }

// Data Type 5202
#define LG    0x01
#define HG    0x02
#define TOA   0x10
#define TOT   0x20

class MyBinaryData
{
public:
    std::streampos              t_begin, end, mb;
    std::streamoff              t_totsize;
    std::array<std::string, 2>  t_unit = { "LSB", "ns" };
    std::array<std::string, 2>  t_unit_tstamp = { "LSB", "us" };  // time unit in the csv file header for timestamp
    std::string                 t_s_sw_version;
    std::string                 t_s_data_version;   // Version of Data format as string
    uint8_t                     t_force_ns = 0;
    uint16_t                    t_en_bin = 0;
    uint8_t                     t_time_unit = 0;    // ToA or ToT written as int (LSB) or float (ns)
    float                       t_LSB_ns = 0;
    uint8_t                     t_data_format = 0;  // Version of Data format
    uint8_t                     t_acq_mode = 0;
    uint16_t                    t_run_num = 0;      // Run number of the binary file
    uint16_t                    t_evt_size = 0;
    uint16_t                    t_brd_ver = 0;      // Type of FERS board (5202, 5203 ...)
    uint8_t                     t_brd = 0;
    double                      t_tstamp = 0;
    double                      t_rel_tstamp = 0;   // Relative Timestamp of an external trigger
    uint64_t                    t_trigger_ID = 0;
    uint64_t                    t_ch_mask = 0;
    uint16_t                    t_num_of_hit = 0;
    uint64_t                    t_start_run = 0;    // Start of acquisition (epoch ms)
    std::vector<uint8_t>        t_ch_id;      // Channel ID
    std::vector<uint8_t>        t_data_type;  // Data type
    std::vector<uint16_t>       t_PHA_LG;     // Low gain
    std::vector<uint16_t>       t_PHA_HG;     // High gain
    std::vector<uint32_t>       t_ToA_i;      // Time of Arrival (in bins)
    std::vector<uint16_t>       t_ToT_i;      // Time over Threshold (in bins)
    std::vector<float>          t_ToA_f;      // Time of Arrival (in float) 
    std::vector<float>          t_ToT_f;      // Time over Threshold (in float)
    std::vector<uint32_t>       t_counts;     // 

    void Init(uint8_t force_ns, uint8_t mode);
    MyBinaryData() {};
    MyBinaryData(uint8_t force_ns, uint8_t mode); // constructors
    MyBinaryData(std::ifstream& binfile, std::ofstream& csvfile, uint8_t force_ns);
    MyBinaryData(uint8_t mode, std::ofstream& csvfile, uint8_t force_ns, uint8_t format_version, uint16_t en_bin, uint8_t toa_bin); // In the case the header is read from main
    ~MyBinaryData() {}; // distructor

    void ReadHeaderBinfile(std::ifstream& binfile);
    void ComputeBinfileSize(std::ifstream& binfile);
    std::streamoff GetEventsSize() { return MyBinaryData::t_totsize; };  // Return the size of the bin file containing the Events
    std::streamoff GetEventsBegin() { return std::streamoff(MyBinaryData::t_begin); };  // Return the file position where the Events start
    void WriteCsvHeader(std::ofstream& csvfile);

    void ReadEvtHeader(std::ifstream& binfile);
    void ReadTmpEvt(std::ifstream& binfile);
    uint16_t ReadSpectTime(std::ifstream& binfile);
    //uint16_t ReadTime(std::ifstream& binfile);
    uint16_t ReadCnts(std::ifstream& binfile);
    void WriteTmpEvt(std::ofstream& csvfile);
    void ResetValues();
};

// --------------------------------------------------------- //
//                   CPP FILE CONTENT                        //
// --------------------------------------------------------- //

// Read (jump) the header, 
// till 3.0:
//      16 bits (data format version) + 24 bits (software) + 8 bits (Acq Mode) + 64 bits (Start Acquisition)
// after 3.1:
//      16 bits (data format version) + 24 bits (software) + 8 bits (Acq Mode) + 16 bits (EnHisto bin) + 8 bits (Time in LSB or ns) + 32 bits ( value of LSB in ns) + 64 bits (Start Acquisition)
 

MyBinaryData::MyBinaryData(uint8_t force_ns, uint8_t mode) {
    MyBinaryData::Init(force_ns, mode);
}

MyBinaryData::MyBinaryData(uint8_t mode, std::ofstream& csvfile, uint8_t force_ns, uint8_t format_version, uint16_t en_bin, uint8_t toa_bin) { // When header is read in the main
    // The other parameters read in the main can be passed as a map. To be implemented
    MyBinaryData::Init(force_ns, mode);
    MyBinaryData::WriteCsvHeader(csvfile);
}

MyBinaryData::MyBinaryData(std::ifstream& binfile, std::ofstream& csvfile, uint8_t force_ns) {
    // binfile is already opened
    // Which values need to be set?
    MyBinaryData::Init(force_ns, 0);
    MyBinaryData::ReadHeaderBinfile(binfile);
    MyBinaryData::ComputeBinfileSize(binfile);
    MyBinaryData::WriteCsvHeader(csvfile);
}

void MyBinaryData::Init(uint8_t force_ns, uint8_t mode) {
    MyBinaryData::t_force_ns = force_ns;
    MyBinaryData::t_LSB_ns = 0.5;
    MyBinaryData::t_data_format = 0;
    MyBinaryData::t_acq_mode = mode;
    MyBinaryData::t_evt_size = 0;
    MyBinaryData::t_brd = 0;
    MyBinaryData::t_tstamp = 0;
    MyBinaryData::t_trigger_ID = 0;
    MyBinaryData::t_ch_mask = 0;
    MyBinaryData::t_num_of_hit = 0;
    MyBinaryData::t_unit[0] = "LSB";
    MyBinaryData::t_unit[1] = "ns";
    MyBinaryData::t_unit_tstamp[0] = "LSB";
    MyBinaryData::t_unit_tstamp[1] = "us";
}

void MyBinaryData::
ReadHeaderBinfile(std::ifstream& binfile) {
    uint8_t tmp_8 = 0;
    uint16_t tmp_16 = 0;
    uint64_t tmp_64 = 0;
    float tmp_f = 0;

    // Skip BinFile Header size
    //binfile.seekg(1, std::ios::cur);

    // Read Data Format Version
    binfile.read((char*)&tmp_8, sizeof(tmp_8));
    MyBinaryData::t_s_data_version = std::to_string(tmp_8) + ".";
    MyBinaryData::t_data_format = 10 * tmp_8;
    binfile.read((char*)&tmp_8, sizeof(tmp_8));
    MyBinaryData::t_data_format += tmp_8;
    MyBinaryData::t_s_data_version += std::to_string(tmp_8);

    
    //binfile.seekg(3, std::ios::cur); // Skip software version (3 bytes)
    for (int v_i = 0; v_i < 3; ++v_i) {
        binfile.read((char*)&tmp_8, sizeof(tmp_8));
        if (v_i != 0) MyBinaryData::t_s_sw_version += ".";
        MyBinaryData::t_s_sw_version += std::to_string(tmp_8);
    }
    
    if (MyBinaryData::t_data_format >= 32) { // Skip Board version (2 bytes)
        binfile.read((char*)&tmp_16, sizeof(tmp_16));
        MyBinaryData::t_brd_ver = tmp_16;
    }

    if (MyBinaryData::t_data_format >= 32) {
        binfile.read((char*)&tmp_16, sizeof(tmp_16));
        MyBinaryData::t_run_num = tmp_16;
    }

    binfile.read((char*)&tmp_8, sizeof(tmp_8)); // Read acq_mode
    MyBinaryData::t_acq_mode = tmp_8;

    if (MyBinaryData::t_data_format >= 31) { // From V3.1 the header include OutFileUnit(8bits), EN_BIN (16bits), LSB_ns (32bits, float) 
        binfile.read((char*)&tmp_16, sizeof(tmp_16)); // EnBin
        MyBinaryData::t_en_bin = tmp_16;
        binfile.read((char*)&tmp_8, sizeof(tmp_8)); // OutFileUnit - Time in LSB or ns
        MyBinaryData::t_time_unit = tmp_8;
        binfile.read((char*)&tmp_f, sizeof(tmp_f)); // Value of LSB in ns
        MyBinaryData::t_LSB_ns = tmp_f;
    }
    binfile.read((char*)&tmp_64, sizeof(tmp_64));
    MyBinaryData::t_start_run = tmp_64;

    print("Software version", t_s_sw_version);
    print("Board type", t_brd_ver);
    print("Run number", t_run_num);
    print("Acquisition mode", int(t_acq_mode));
    print("Start run", int(t_start_run));
    print("Size event", int(t_evt_size));

    //binfile.seekg(8, std::ios::cur); // Skip 64 bits of the Start acq timestamp, here in all the version
}

void MyBinaryData:: ComputeBinfileSize(std::ifstream& binfile) {
    t_begin = binfile.tellg();    // Position of the reading pointer after the binfile Header
    binfile.seekg(0, std::ios::end);
    end = binfile.tellg();
    binfile.seekg(t_begin, std::ios::beg);
    t_totsize = end - t_begin;
    //t_read_size = t_begin;
}

void MyBinaryData::WriteCsvHeader(std::ofstream& csvfile) {
    auto tp = std::chrono::system_clock::time_point(std::chrono::milliseconds(MyBinaryData::t_start_run));
    // convert time point to local time structure
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::string date = std::asctime(std::localtime(&tt));

    // date.erase(std::remove(date.begin(), date.end(), '\n'), date.cend()); //! COMMENTED BECAUSE OF BUG
    
    // Header in comment block
    csvfile << " //************************************************\n";
    uint8_t time_unit = t_time_unit || t_force_ns;
    if (MyBinaryData::t_data_format >= 32)
        csvfile << "//Board:" << t_brd_ver << "\n//File_Format_Version:" << t_s_data_version << "\n//Janus_Release " << t_s_sw_version << "\n";

    csvfile << "//Acquisition_Mode:";
    if ((MyBinaryData::t_acq_mode&0x0F) == ACQMODE_SPECT) {
        csvfile << "Spectroscopy\n";
        if (MyBinaryData::t_data_format >= 31)
            csvfile << "//Energy_Histo_NBins:" << t_en_bin << "\n";
        if (MyBinaryData::t_data_format >= 32) {
            csvfile << "//Run#:" << t_run_num << "\n//Start_Time_Epoch:" << t_start_run << "\n";
            csvfile << "//Start_Time_DateTime:" << date << "\n";
        }
        csvfile << "//************************************************\n";
        
        csvfile << "TStamp,";
        if ((MyBinaryData::t_acq_mode&0xF0) == DTQ_RTSTAMP)
            csvfile << "Rel_Tstamp,";

        csvfile << "Trg_Id,Board_Id,Num_hits,ChannelMask,CH_Id,Data_type,PHA_LG,PHA_HG\n";
    }
    if ((MyBinaryData::t_acq_mode&0x0F) == ACQMODE_TIMING) {
        csvfile << "Timing\n";
        if (MyBinaryData::t_data_format >= 31) {
            csvfile << "//Time_LSB_Value_ns:" << t_LSB_ns << "\n";
            csvfile << "//Time_Unit:" << MyBinaryData::t_unit[t_time_unit] << "\n";
        }
        if (MyBinaryData::t_data_format >= 32) {
            csvfile << "//Run#:" << t_run_num << "\n//Start_Time_Epoch:" << t_start_run << "\n";
            csvfile << "//Start_Time_DateTime:" << date << "\n";
        }
        csvfile << "//************************************************\n";
        csvfile << "TStamp,Board_Id,Num_hits,CH_Id,Data_type,ToA_" << MyBinaryData::t_unit[t_time_unit] << ",ToT_" << MyBinaryData::t_unit[t_time_unit] << " \n";
    }
    if ((MyBinaryData::t_acq_mode&0x0F) == ACQMODE_TSPECT) {
        csvfile << "SpectTiming\n";
        if (MyBinaryData::t_data_format >= 31) {
            csvfile << "//Energy_Histo_NBins:" << t_en_bin << "\n";
            csvfile << "//Time_LSB_Value_ns:" << t_LSB_ns << "\n";
            csvfile << "//Time_Unit:" << MyBinaryData::t_unit[t_time_unit] << "\n";
        }
        if (MyBinaryData::t_data_format >= 32) {
            csvfile << "//Run#:" << t_run_num << "\n//Start_Time_Epoch:" << t_start_run << "\n";
            csvfile << "//Start_Time_DateTime:" << date << "\n";
        }
        csvfile << " //************************************************\n";

        csvfile << "TStamp,";
        if ((MyBinaryData::t_acq_mode & 0xF0) == DTQ_RTSTAMP)
            csvfile << "Rel_TStamp,";
        csvfile << "Trg_Id,Board_Id,Num_hits,ChannelMask,CH_Id,Data_type,PHA_LG,PHA_HG,ToA_" << MyBinaryData::t_unit[t_time_unit] << ",ToT_" << MyBinaryData::t_unit[t_time_unit] << " \n";
    }
    if ((MyBinaryData::t_acq_mode&0x0F) == ACQMODE_COUNT) {
        csvfile << "Counting mode\n";
        if (MyBinaryData::t_data_format >= 32) {
            csvfile << "//Run#:" << t_run_num << "\n//Start_Time_Epoch:" << t_start_run << "\n";
            csvfile << "//Start_Time_DateTime:" << date << "\n";
        }
        csvfile << " //************************************************\n";

        csvfile << "TStamp,";
        if ((MyBinaryData::t_acq_mode & 0xF0) == DTQ_RTSTAMP)
            csvfile << "Rel_TStamp,";
        csvfile << "Trg_Id,Board_Id,Num_hits,ChannelMask,CH_Id,Counts\n";
    }
}

void MyBinaryData::ReadEvtHeader(std::ifstream& binfile) {
    binfile.read((char*)&t_evt_size, sizeof(t_evt_size));
    binfile.read((char*)&t_brd, sizeof(t_brd));
    binfile.read((char*)&t_tstamp, sizeof(t_tstamp));
    if (t_acq_mode & DTQ_RTSTAMP) binfile.read((char*)&t_rel_tstamp, sizeof(t_rel_tstamp));
}

void MyBinaryData::ReadTmpEvt(std::ifstream& binfile) {
    ReadEvtHeader(binfile);
    uint16_t myrsize = t_evt_size;
    // print(myrsize);

    if ((t_acq_mode&0X0F) != DTQ_TIMING) {
        uint16_t header_size = 216;
        if (t_acq_mode & DTQ_RTSTAMP)
            header_size += 64;    // The RelTstamp is present
        binfile.read((char*)&t_trigger_ID, sizeof(t_trigger_ID));
        binfile.read((char*)&t_ch_mask, sizeof(t_ch_mask));
        myrsize -= header_size / 8;  // 64*3 + 16 + 8
    } else {
        binfile.read((char*)&t_num_of_hit, sizeof(uint16_t));
        myrsize -= 104 / 8;  // 64 + 16*2 + 8
    }

    while (myrsize > 0) {
        uint16_t msize = 0;
        if (t_acq_mode & DTQ_TSPECT) // Spect Or Time
            msize = MyBinaryData::ReadSpectTime(binfile);
        if (t_acq_mode & DTQ_COUNT) // Count mode
            msize = MyBinaryData::ReadCnts(binfile);
        if ((t_acq_mode&0x0F) != DTQ_TIMING) // Number of hits (Chs firing) read in Spect and Count mode, like in time mode (DNIN: is it useful?)
            ++t_num_of_hit;
        myrsize -= msize;
    }
}

uint16_t MyBinaryData::ReadSpectTime(std::ifstream& binfile) {
    uint8_t  tmp_u8;
    uint16_t tmp_u16;
    uint32_t tmp_u32;
    float tmp_f;

    binfile.read((char*)&tmp_u8, sizeof(uint8_t));
    t_ch_id.push_back(tmp_u8);
    binfile.read((char*)&tmp_u8, sizeof(uint8_t));
    t_data_type.push_back(tmp_u8);

    uint16_t mysize = 2;
    if (t_data_type.back() & 0x01) {
        binfile.read((char*)&tmp_u16, sizeof(uint16_t));
        t_PHA_LG.push_back(tmp_u16);
        mysize += 2;
    } else t_PHA_LG.push_back(0);

    if (t_data_type.back() & 0x02) {
        binfile.read((char*)&tmp_u16, sizeof(uint16_t));
        t_PHA_HG.push_back(tmp_u16);
        mysize += 2;
    } else t_PHA_HG.push_back(0);

    if (t_data_type.back() & 0x10) {
        if (MyBinaryData::t_time_unit) { // Default is 0. If ver > 3.1 it can be 1, that means time is given as float
            binfile.read((char*)&tmp_f, sizeof(tmp_f));
            t_ToA_f.push_back(tmp_f);
            mysize += 4;
        } else {
            binfile.read((char*)&tmp_u32, sizeof(uint32_t));
            t_ToA_i.push_back(tmp_u32);
            mysize += 4;
        }
    } else {    // Set to 0 both vectors instead of do another 'if'
        t_ToA_i.push_back(0);
        t_ToA_f.push_back(0);
    }

    if (t_data_type.back() & 0x20) {
        if (MyBinaryData::t_time_unit) { // Default is 0. If ver > 3.1 it can be 1, that means time is given as float
            binfile.read((char*)&tmp_f, sizeof(tmp_f));
            t_ToT_f.push_back(tmp_f);
            mysize += 4;
        } else {
            binfile.read((char*)&tmp_u16, sizeof(uint16_t));
            t_ToT_i.push_back(tmp_u16);
            mysize += 2;
        }
    } else {
        t_ToT_i.push_back(0);
        t_ToT_f.push_back(0);
    }

    // Due to a bug in JanusC, in timing mode the size of the event
    // always summed ToA and ToT till data format version 3.0, so here the size was 'size+=4+2' always.
    // The correct size is computed from data format version 3.1
    if (t_data_format < 31 && ((t_acq_mode & 0x0F) == 0x02))    // Patch for back compatibility
        mysize = 8;

    return mysize;
}

uint16_t MyBinaryData::ReadCnts(std::ifstream& binfile) {
    uint8_t  tmp_u8;
    uint32_t tmp_u32;
    uint16_t mysize;

    binfile.read((char*)&tmp_u8, sizeof(uint8_t));
    t_ch_id.push_back(tmp_u8);
    binfile.read((char*)&tmp_u32, sizeof(uint32_t));
    t_counts.push_back(tmp_u32);
    mysize = 5; // 40 bits->5 bytes

    return mysize;
}

void MyBinaryData::WriteTmpEvt(std::ofstream& csvfile) {
    float time_factor = 1; // Conversion factor from LSB to ns. It changes if force_ns is 1
    if (t_force_ns)
        time_factor = t_LSB_ns;

    //std::string evt_header = std::to_string(t_brd) + "," + std::to_string(t_tstamp);
    std::string evt_header = std::to_string(t_tstamp);
    if ((t_acq_mode & 0XF0) == DTQ_RTSTAMP) evt_header += "," + std::to_string(t_rel_tstamp);
    if (t_acq_mode != ACQMODE_TIMING) {
        evt_header += "," + std::to_string(t_trigger_ID) + "," + std::to_string(t_brd) + "," + std::to_string(t_num_of_hit) + ","; // +std::to_string(ch_mask);
        char tmp[50]; // print mask
        sprintf(tmp, "0x%" PRIx64, t_ch_mask);
        evt_header += tmp;
    } else {
        evt_header += "," + std::to_string(t_brd) + "," + std::to_string(MyBinaryData::t_num_of_hit);
    }

    for (uint32_t hit_i = 0; hit_i < t_ch_id.size(); ++hit_i) {
        //if (!((ch_mask >> hit_i) & 0x1)) 
        //    continue;
        std::string s_data;
        char tmp[50];
        if (t_acq_mode & ACQMODE_TSPECT) { // Spect (b01) Or Time (b10). This part is common for both ACQ mode
            sprintf(tmp, "0x%" PRIx8, t_data_type.at(hit_i));
            s_data = "," + std::to_string(t_ch_id.at(hit_i)) + "," + tmp;
        }
        if (t_acq_mode & ACQMODE_SPECT) {
            if (t_data_type.at(hit_i) & LG)
                s_data += "," + std::to_string(t_PHA_LG.at(hit_i));
            else s_data += ",-1";
            if (t_data_type.at(hit_i) & HG)
                s_data += "," + std::to_string(t_PHA_HG.at(hit_i));
            else s_data += ",-1";
        }
        if (t_acq_mode & ACQMODE_TIMING) {
            if (t_data_type.at(hit_i) & TOA) {
                if (t_time_unit)
                    s_data += "," + std::to_string(t_ToA_f.at(hit_i));
                else if (time_factor == 1)
                    s_data += "," + std::to_string(t_ToA_i.at(hit_i));  // It is not correct to show a float when an integer is expected, despite the decimal part is 0
                else
                    s_data += "," + std::to_string(time_factor * t_ToA_i.at(hit_i));
            } else s_data += ",-1";
            if (t_data_type.at(hit_i) & TOT) {
                if (t_time_unit)
                    s_data += "," + std::to_string(t_ToT_f.at(hit_i));
                else if (time_factor == 1)
                    s_data += "," + std::to_string(t_ToT_i.at(hit_i));
                else
                    s_data += "," + std::to_string(time_factor * t_ToT_i.at(hit_i));
            } else s_data += ",-1";
        }
        if (t_acq_mode == ACQMODE_COUNT)  // Count mode
            s_data += "," + std::to_string((int)t_ch_id.at(hit_i)) + "," + std::to_string((int)t_counts.at(hit_i));
        
        csvfile << evt_header << s_data << "\n";
    }

    //********************************************************
    ResetValues();
}

void MyBinaryData::ResetValues() {
    t_num_of_hit = 0;
    t_ch_id.clear();
    t_data_type.clear();
    t_PHA_LG.clear();
    t_PHA_HG.clear();
    t_ToA_i.clear();
    t_ToT_i.clear();
    t_ToA_f.clear();
    t_ToT_f.clear();
    t_counts.clear();
}

#endif //MYBINARYDATA
