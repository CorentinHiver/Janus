/*******************************************************************************
*
* CAEN SpA - Software Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
*\note TERMS OF USE :
*This program is free software; you can redistribute itand /or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation.This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.The user relies on the
* software, documentation and results solely at his own risk.
****************************************************************************** */

#ifdef Win32
#pragma once
#endif

#include <cstdint>
#include <cstdio>
#include <cinttypes>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>
#include <array>

#ifdef _WIN32
#include <windows.h>
#define my_sprintf sprintf_s
#else
#define  my_sprintf sprintf
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

class t_BinaryData_5203
{
    private:
        std::streampos                      t_begin, end, mb;   
        std::streamoff                      t_totsize;      // dimension of the binary file in input
        std::array<std::string, 2>          t_unit;   // = { "LSB", "ns" };    // time unit in the csv file header
        std::array<std::string, 2>          t_unit_tstamp;    // = { "LSB", "us" };  // time unit in the csv file header for timestamp
        uint8_t                             t_force_ns;     // force the conversion of time in ns (if is in LSB)
        uint8_t                             t_time_unit;    // ToA or ToT written as int (LSB) or float (ns). 0: LSB, 1: ns
        float                               t_ToA_LSB_ns;   // ToA LSB value in ns
        float                               t_ToT_LSB_ns;   // ToT LSB value in ns
        float                               t_Tstamp_LSB_ns;    // Tstamp LSB value in ns
        uint8_t                             t_data_format;      // Version of Data format
        std::string                         t_s_data_version;   // Version of Data format as string
        std::string                         t_s_sw_version;     // Version of Janus Release
        uint16_t                            t_run_num;      // Run number of the binary file
        uint16_t                            t_brd_ver;      // Type of FERS board (5202, 5203 ...)
        uint16_t                            t_acq_mode;     // Acquisition mode
        uint8_t                             t_meas_mode;    // Measurement mode
        uint64_t                            t_start_run;    // Start of acquisition (epoch ms)
        uint16_t                            t_evt_size;     // Size of each event of binary file. It is used to guide the conversion
        uint8_t                             t_brd;          // NOT USED IN 5203
        double                              t_tstamp_d;     // Timestamp as double
        uint64_t                            t_tstamp_64;    // Timestamp as uint64
        double                              t_rel_tstamp_d;     // Relative Timestamp as double
        uint64_t                            t_rel_tstamp_64;    // Relative Timestamp as uint64
        uint64_t                            t_trigger_ID;   //
        uint16_t                            t_num_of_hit;   // Number of hit in the event, can be used as second level check
        std::vector<uint8_t>                t_brd_id;
        std::vector<uint8_t>                t_ch_id;
        std::vector<uint8_t>                t_edge;
        std::vector<uint32_t>               t_ToA_i;        // ToA as uint32
        std::vector<uint64_t>               t_ToA_64;       // ToA as uint64 (Streaming mode only)
        std::vector<uint16_t>               t_ToT_i;        // ToT as in uint16
        std::vector<double>                 t_ToA_d;        // ToA as double (Streaming mode only)
        std::vector<float>                  t_ToA_f;        // ToA as float
        std::vector<float>                  t_ToT_f;        // ToT as float

        void Init(uint8_t force_ns, uint8_t mode);
        std::string WriteMeasMode();
        std::string WriteAcqMode();
    public:
        t_BinaryData_5203() {};
        t_BinaryData_5203(uint8_t force_ns, uint8_t mode); // constructors
        t_BinaryData_5203(std::ifstream & binfile, std::ofstream & csvfile, uint8_t force_ns);
        t_BinaryData_5203(uint8_t mode, std::ofstream & csvfile, uint8_t force_ns, uint8_t format_version, uint16_t en_bin, uint8_t toa_bin); // In the case the header is read from main
        ~t_BinaryData_5203() {}; // distructor

        void ReadHeaderBinfile(std::ifstream & binfile);
        void ComputeBinfileSize(std::ifstream & binfile);
        std::streamoff GetEventsSize() { return t_BinaryData_5203::t_totsize; };  // Return the size of the bin file containing the Events
        std::streamoff GetEventsBegin() { return std::streamoff(t_BinaryData_5203::t_begin); };  // Return the file position where the Events start
        void WriteCsvHeader(std::ofstream & csvfile);

        uint16_t ReadEvtHeader(std::ifstream & binfile);
        void ReadTmpEvt(std::ifstream & binfile);
        uint16_t ReadStreamingEvent(std::ifstream & binfile);
        uint16_t ReadCStartTMatchEvent(std::ifstream & binfile);
        void WriteTmpEvt(std::ofstream & csvfile);
        void ResetValues5203();
};

#define ACQMODE_COMMONSTART 0x02
#define ACQMODE_COMMONSTOP  0x12
#define ACQMODE_STREAMING   0x22
#define ACQMODE_TRGMATCHING 0x32

#define MEASMODE_LEADONLY   0x01
#define MEASMODE_LEADTRAIL  0x03
#define MEASMODE_LEADTOT8   0x05
#define MEASMODE_LEADTOT11  0x09

#define OUTLSB  0
#define OUTNS   1


t_BinaryData_5203::t_BinaryData_5203(uint8_t force_ns, uint8_t mode) {
    t_BinaryData_5203::Init(force_ns, mode);
}

t_BinaryData_5203::t_BinaryData_5203(uint8_t mode, std::ofstream& csvfile, uint8_t force_ns, uint8_t format_version, uint16_t en_bin, uint8_t toa_bin) { // When header is read in the main
    // The other parameters read in the main can be passed as a map. To be implemented
    t_BinaryData_5203::Init(force_ns, mode);
    t_BinaryData_5203::WriteCsvHeader(csvfile);
}

t_BinaryData_5203::t_BinaryData_5203(std::ifstream& binfile, std::ofstream& csvfile, uint8_t force_ns) {
    // binfile is already opened
    // Which values need to be set?
    t_BinaryData_5203::Init(force_ns, 0);    // 
    t_BinaryData_5203::ReadHeaderBinfile(binfile);
    t_BinaryData_5203::ComputeBinfileSize(binfile);
    t_BinaryData_5203::WriteCsvHeader(csvfile);
}

void t_BinaryData_5203::Init(uint8_t force_ns, uint8_t mode) {
    t_BinaryData_5203::t_s_data_version = "";
    t_BinaryData_5203::t_s_sw_version = "";
    t_BinaryData_5203::t_force_ns = force_ns;
    t_BinaryData_5203::t_ToA_LSB_ns = 0.5;
    t_BinaryData_5203::t_ToT_LSB_ns = 0.5;
    t_BinaryData_5203::t_data_format = 0;
    t_BinaryData_5203::t_acq_mode = mode;
    t_BinaryData_5203::t_evt_size = 0;
    t_BinaryData_5203::t_brd = 0;
    t_BinaryData_5203::t_tstamp_d = 0;
    t_BinaryData_5203::t_tstamp_64 = 0;
    t_BinaryData_5203::t_trigger_ID = 0;
    t_BinaryData_5203::t_num_of_hit = 0;
    t_BinaryData_5203::t_unit[0] = "LSB";
    t_BinaryData_5203::t_unit[1] = "ns";
    t_BinaryData_5203::t_unit_tstamp[0] = "LSB";
    t_BinaryData_5203::t_unit_tstamp[1] = "us";
}

void t_BinaryData_5203::ReadHeaderBinfile(std::ifstream& binfile) {
    uint8_t tmp_8 = 0;
    uint16_t tmp_16 = 0;
    uint64_t tmp_64 = 0;
    float tmp_f = 0;
    // Read Data Format Version
    binfile.read((char*)&tmp_8, sizeof(tmp_8));
    t_BinaryData_5203::t_s_data_version = std::to_string(tmp_8) + ".";
    t_BinaryData_5203::t_data_format = 10 * tmp_8;
    
    binfile.read((char*)&tmp_8, sizeof(tmp_8));
    t_BinaryData_5203::t_s_data_version += std::to_string(tmp_8);
    t_BinaryData_5203::t_data_format += tmp_8;

    //binfile.seekg(3, std::ios::cur); // Skip software version (3 bytes)
    for (int i = 0; i < 3; ++i) {   // Get software version on 3 bytes
        binfile.read((char*)&tmp_8, sizeof(tmp_8));
        if (i != 0) t_BinaryData_5203::t_s_sw_version += ".";
        t_BinaryData_5203::t_s_sw_version += std::to_string(tmp_8);
    }

    if (t_BinaryData_5203::t_data_format >= 32) { // Skip Board version (2 bytes)
        binfile.read((char*)&tmp_16, sizeof(tmp_16));
        t_BinaryData_5203::t_brd_ver = tmp_16;
    }
    if (t_BinaryData_5203::t_data_format >= 31) { // Skip Run Number (2 bytes)
        binfile.read((char*)&tmp_16, sizeof(tmp_16));
        t_BinaryData_5203::t_run_num = tmp_16;
    }
    
    binfile.read((char*)&tmp_16, sizeof(tmp_16)); // Read acq_mode
    t_BinaryData_5203::t_acq_mode = tmp_16;

    binfile.read((char*)&tmp_8, sizeof(tmp_8)); // Read meas_mode
    t_BinaryData_5203::t_meas_mode = tmp_8;

    binfile.read((char*)&tmp_8, sizeof(tmp_8)); // OutFileUnit - Time in LSB or ns
    t_BinaryData_5203::t_time_unit = tmp_8;
    // t_force_ns = t_time_unit (should be equal, but is better doing a check)

    binfile.read((char*)&tmp_f, sizeof(tmp_f)); // Value of ToA LSB in ps
    t_BinaryData_5203::t_ToA_LSB_ns = tmp_f / 1e3;

    binfile.read((char*)&tmp_f, sizeof(tmp_f)); // Value of ToT LSB in ps
    t_BinaryData_5203::t_ToT_LSB_ns = tmp_f / 1e3;

    binfile.read((char*)&tmp_f, sizeof(tmp_f)); // Valute of Tstamp LSB in ps
    t_BinaryData_5203::t_Tstamp_LSB_ns = tmp_f / 1e6; // Tstamp Clk is 12.8 ns

    binfile.read((char*)&tmp_64, sizeof(tmp_64));
    t_BinaryData_5203::t_start_run = tmp_64;
    //binfile.seekg(8, std::ios::cur); // Skip 64 bits of the Start acq timestamp, here in all the version
}

void t_BinaryData_5203::ComputeBinfileSize(std::ifstream& binfile) {
    t_BinaryData_5203::t_begin = binfile.tellg();    // Position of the reading pointer after the binfile Header
    binfile.seekg(0, std::ios::end);
    end = binfile.tellg();
    binfile.seekg(t_BinaryData_5203::t_begin, std::ios::beg);
    t_BinaryData_5203::t_totsize = end - t_BinaryData_5203::t_begin;
    //t_read_size = t_begin;
}

std::string t_BinaryData_5203::WriteMeasMode() {
    if (t_BinaryData_5203::t_meas_mode == 0x01)
        return "MeasMode: Lead Only";
    if (t_BinaryData_5203::t_meas_mode == 0x03)
        return "MeasMode: Lead and Trail";
    if (t_BinaryData_5203::t_meas_mode == 0x05)
        return "MeasMode: Lead and ToT (8 bits)";
    if (t_BinaryData_5203::t_meas_mode == 0x09)
        return "MeasMode: Lead and ToT (11 bits)";
    return "MEASMODE READ ERROR";

}

std::string t_BinaryData_5203::WriteAcqMode() {
    if (t_BinaryData_5203::t_acq_mode == 0x02)
        return "AcqMode: Common Start";
    if (t_BinaryData_5203::t_acq_mode == 0x12)
        return "AcqMode: Common Stop";
    if (t_BinaryData_5203::t_acq_mode == 0x22)
        return "AcqMode: Streaming";
    if (t_BinaryData_5203::t_acq_mode == 0x32)
        return "AcqMode: Trigger Matching";
    return "ACQMODE READ ERROR";
}

void t_BinaryData_5203::WriteCsvHeader(std::ofstream& csvfile) {
    auto tp = std::chrono::system_clock::time_point(std::chrono::milliseconds(t_BinaryData_5203::t_start_run));
    // convert time point to local time structure
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::string date = std::asctime(std::localtime(&tt));

    date.erase(std::remove(date.begin(), date.end(), '\n'), date.cend());

    uint8_t time_unit = t_time_unit || t_force_ns;
    csvfile << "//************************************************\n";
    if (t_brd_ver) {
        csvfile << "//Board:" << t_brd_ver << "\n//File_Format_Version:" << t_s_data_version << "\n//Janus_Release " << t_s_sw_version << "\n";
    } else {
        csvfile << "//Janus Release " << t_s_sw_version << "\n";
        csvfile << "//File Format Version " << t_s_data_version << "\n";
    }
    csvfile << "//Acquisition_Mode:" << t_BinaryData_5203::WriteAcqMode() << " \n";
    csvfile << "//Measurement_Mode:" << t_BinaryData_5203::WriteMeasMode() << " \n";
    csvfile << "//Time unit:" << t_BinaryData_5203::t_unit[time_unit] << " \n";
    csvfile << "//TStamp unit:" << t_BinaryData_5203::t_unit_tstamp[time_unit] << " \n";
    csvfile << "//ToA LSB value_ps:" << t_ToA_LSB_ns * 1e3 << "\n";
    csvfile << "//ToT LSB value_ps:" << t_ToT_LSB_ns * 1e3 << "\n";
    csvfile << "//TStamp LSB value_ns:" << t_Tstamp_LSB_ns * 1e3 << "\n";
    csvfile << "//Run#:" << t_run_num << "\n";
    csvfile << "//Start_Time_Epoch:" << t_start_run << "\nStart_Time_DateTime:" << date << "\n";
    if (t_acq_mode == ACQMODE_COMMONSTART || t_acq_mode == ACQMODE_COMMONSTOP)
        csvfile << "TStamp_" << t_BinaryData_5203::t_unit_tstamp[time_unit] << ",Trigger_ID,Board_Id,Num_hits,Ch_Id,ToA_" << t_BinaryData_5203::t_unit[time_unit];
    else csvfile << "TStamp_" << t_BinaryData_5203::t_unit_tstamp[time_unit] << ",Trigger_ID,Board_Id,Num_hits,Ch_Id,Edge,ToA_" << t_BinaryData_5203::t_unit[time_unit];
    //std::cout << "TStamp unit: " << t_unit_tstamp[time_unit] << std::endl;
    
    if (t_meas_mode != MEASMODE_LEADONLY)
        csvfile << ",ToT_" << t_BinaryData_5203::t_unit[time_unit] << " \n";
    else csvfile << "\n";
}


uint16_t t_BinaryData_5203::ReadEvtHeader(std::ifstream& binfile) {
    uint16_t msize = 0;
    binfile.read((char*)&t_evt_size, sizeof(t_evt_size));
    msize += 2;
    if (t_time_unit == OUTNS)
        binfile.read((char*)&t_tstamp_d, sizeof(t_tstamp_d));
    else
        binfile.read((char*)&t_tstamp_64, sizeof(t_tstamp_64));
    msize += 8;
    if (t_BinaryData_5203::t_acq_mode != ACQMODE_STREAMING) {
        binfile.read((char*)&t_trigger_ID, sizeof(t_trigger_ID));
        msize += 8;
    }
    binfile.read((char*)&t_num_of_hit, sizeof(t_num_of_hit));
    msize += 2;

    return msize;

}

void t_BinaryData_5203::ReadTmpEvt(std::ifstream& binfile) {
    uint16_t hsize = ReadEvtHeader(binfile);
    uint16_t myrsize = t_evt_size;
    myrsize -= hsize;

    // Common Start/Stop + TrgMatch
    // LSB: ToA uint32_t ToT uint16_t
    // ns: ToA float    ToT float
    // Streming
    // LSB: ToA uint64_t  ToT uint16_t
    // ns: ToA double   ToT float
    while (myrsize > 0) {
        uint16_t msize = 0;
        if (t_acq_mode != ACQMODE_STREAMING)
            msize = ReadCStartTMatchEvent(binfile);
        else
            msize = ReadStreamingEvent(binfile);
        ++t_num_of_hit;
        myrsize -= msize;
    }
}

uint16_t t_BinaryData_5203::ReadStreamingEvent(std::ifstream& binfile) {
    uint16_t mysize = 0;
    uint8_t tmp_u8;
    uint16_t tmp_u16;
    uint64_t tmp_u64;
    float tmp_f;
    double tmp_d;

    binfile.read((char*)&tmp_u8, sizeof(uint8_t));  // board
    t_brd_id.push_back(tmp_u8); 
    binfile.read((char*)&tmp_u8, sizeof(uint8_t));  // channel
    t_ch_id.push_back(tmp_u8);
    binfile.read((char*)&tmp_u8, sizeof(uint8_t));  // edge
    t_edge.push_back(tmp_u8);
    mysize += 3;

    if (t_BinaryData_5203::t_time_unit) {
        binfile.read((char*)&tmp_d, sizeof(double));
        t_ToA_d.push_back(tmp_d);
        mysize += 8;
        if (t_BinaryData_5203::t_meas_mode != MEASMODE_LEADONLY) {
            binfile.read((char*)&tmp_f, sizeof(float));
            t_ToT_f.push_back(tmp_f);
            mysize += 4;
        }
    } else {
        binfile.read((char*)&tmp_u64, sizeof(uint64_t));
        t_ToA_64.push_back(tmp_u64);
        mysize += 8;
        if (t_BinaryData_5203::t_meas_mode != MEASMODE_LEADONLY) {
            binfile.read((char*)&tmp_u16, sizeof(uint16_t));
            if (t_BinaryData_5203::t_meas_mode == MEASMODE_LEADTOT8) tmp_u16 = 0xFF & tmp_u16;
            t_ToT_i.push_back(tmp_u16);
            mysize += 2;
        }
    }

    return mysize;
}

uint16_t t_BinaryData_5203::ReadCStartTMatchEvent(std::ifstream& binfile) {
    uint16_t mysize = 0;
    uint8_t tmp_u8;
    uint16_t tmp_u16;
    uint32_t tmp_u32;
    float tmp_f;

    binfile.read((char*)&tmp_u8, sizeof(uint8_t));  // board
    t_brd_id.push_back(tmp_u8);
    binfile.read((char*)&tmp_u8, sizeof(uint8_t));  // channel
    t_ch_id.push_back(tmp_u8);
    mysize += 2;
    if (t_BinaryData_5203::t_acq_mode == ACQMODE_TRGMATCHING) {
        binfile.read((char*)&tmp_u8, sizeof(uint8_t));  // edge
        t_edge.push_back(tmp_u8);
        mysize += 1;
    }

    if (t_BinaryData_5203::t_time_unit) {
        binfile.read((char*)&tmp_f, sizeof(float));
        t_ToA_f.push_back(tmp_f);
        mysize += 4;
        if (t_BinaryData_5203::t_meas_mode != MEASMODE_LEADONLY) {
            binfile.read((char*)&tmp_f, sizeof(float));
            if (t_BinaryData_5203::t_meas_mode == MEASMODE_LEADTOT8) tmp_u16 = 0xFF & tmp_u16;
            t_ToT_f.push_back(tmp_f);
            mysize += 4;
        }
    } else {
        binfile.read((char*)&tmp_u32, sizeof(uint32_t));
        t_ToA_i.push_back(tmp_u32);
        mysize += 4;
        if (t_BinaryData_5203::t_meas_mode != MEASMODE_LEADONLY) {
            binfile.read((char*)&tmp_u16, sizeof(uint16_t));
            if (t_BinaryData_5203::t_meas_mode == MEASMODE_LEADTOT8) tmp_u16 = 0xFF & tmp_u16;
            t_ToT_i.push_back(tmp_u16);
            mysize += 2;
        }
    }

    return mysize;
}

void t_BinaryData_5203::WriteTmpEvt(std::ofstream& csvfile) {
    // Write common header: TStamp + TrgID
    std::string evt_header;
    if (t_time_unit == OUTNS) evt_header += std::to_string(t_tstamp_d);
    else if (t_time_unit == OUTLSB && t_force_ns == 1) evt_header += std::to_string(t_tstamp_64 * t_Tstamp_LSB_ns);
    else evt_header += std::to_string(t_tstamp_64);
    evt_header += "," + std::to_string(t_trigger_ID);
    //std::string evt_header = std::to_string(t_brd) + "," + std::to_string(t_tstamp);

    // Write Data: brd, ch, edge (Not in CStart/CStep), ToA, ToT
    for (uint32_t i = 0; i < t_ch_id.size(); ++i) {
        std::string s_data;

        s_data += std::to_string(t_brd_id.at(i)) + "," + std::to_string(t_num_of_hit) + ',' + std::to_string(t_ch_id.at(i));
        if (t_acq_mode != ACQMODE_COMMONSTART && t_acq_mode != ACQMODE_COMMONSTOP)
            s_data += "," + std::to_string(t_edge.at(i));

        if (t_time_unit == OUTNS) {
            if (t_acq_mode == ACQMODE_STREAMING) s_data += "," + std::to_string(t_ToA_d.at(i));
            else s_data += "," + std::to_string(t_ToA_f.at(i));
        } else if (t_time_unit == OUTLSB && t_force_ns == 1) {
            if (t_acq_mode == ACQMODE_STREAMING) s_data += "," + std::to_string(t_ToA_64.at(i) * t_ToA_LSB_ns);
            else s_data += "," + std::to_string(t_ToA_i.at(i) * t_ToA_LSB_ns);
        } else {
            if (t_acq_mode == ACQMODE_STREAMING) s_data += "," + std::to_string(t_ToA_64.at(i));
            else s_data += "," + std::to_string(t_ToA_i.at(i));
        }

        if (t_meas_mode != MEASMODE_LEADONLY) {
            if (t_time_unit == OUTNS) s_data += "," + std::to_string(t_ToT_f.at(i));
            else if (t_time_unit == OUTLSB && t_force_ns == 1) s_data += "," + std::to_string(t_ToT_i.at(i) * t_ToT_LSB_ns);
            else s_data += "," + std::to_string(t_ToT_i.at(i));
        }
        csvfile << evt_header << "," << s_data << "\n";
    }

    ResetValues5203();
}

void t_BinaryData_5203::ResetValues5203() {
    t_num_of_hit = 0;
    if (t_ch_id.size()) t_ch_id.clear();
    if (t_brd_id.size()) t_brd_id.clear();
    if (t_edge.size())  t_edge.clear();
    if (t_ToA_i.size()) t_ToA_i.clear();
    if (t_ToT_i.size()) t_ToT_i.clear();
    if (t_ToA_f.size()) t_ToA_f.clear();
    if (t_ToT_f.size()) t_ToT_f.clear();
    if (t_ToA_d.size()) t_ToA_d.clear();
    if (t_ToA_64.size()) t_ToA_64.clear();
}