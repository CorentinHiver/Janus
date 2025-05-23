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

#include "BinaryData_5202.h"
#include "BinaryData_5203.h"

#define VERSION     "3.2.1"

// Acquisition Mode 5203 (CSTART/STOP and STREAMING as in 5202)
#define ACQMODE_COMMONSTART 0x02            // The same for A5202
#define ACQMODE_COMMONSTOP  0x12            // The same for A5202
#define ACQMODE_STREAMING   0x22
#define ACQMODE_TRGMATCHING 0x32

//// Acquisition Mode 5202
//#define ACQMODE_SPECT		0x01  // Spectroscopy Mode (Energy)
//#define ACQMODE_TSPECT		0x03  // Spectroscopy + Timing Mode (Energy + Tstamp)
//#define ACQMODE_COUNT		0x04  // Counting Mode (MCS)
//#define ACQMODE_WAVE		0x08  // Waveform Mode
//
//// Data Qualifier 5202
//#define DTQ_SPECT			0x01  // Spectroscopy Mode (Energy)
//#define DTQ_TIMING			0x02  // Timing Mode 
//#define DTQ_COUNT 			0x04  // Counting Mode (MCS)
//#define DTQ_WAVE			0x08  // Waveform Mode
//#define DTQ_TSPECT			0x03  // Spectroscopy + Timing Mode (Energy + Tstamp)
//
//// Data Type 5202
//#define LG                  0x01
//#define HG                  0x02
//#define TOA                 0x10
//#define TOT                 0x20

#define MEASMODE_LEADONLY   0x01
#define MEASMODE_LEADTRAIL  0x03
#define MEASMODE_LEADTOT8   0x05
#define MEASMODE_LEADTOT11  0x09

#define OUTNS   1
#define OUTLSB  0

#ifdef _WIN32
#include <windows.h>
#define my_sprintf sprintf_s
#else
#define  my_sprintf sprintf
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif


class t_BinaryDataFERS
{
private:
	t_BinaryData t_data_5202;
	t_BinaryData_5203 t_data_5203;
    
    std::streampos          t_begin, end, mb;   //                                              - COMMON
    std::streamoff          t_totsize;      // dimension of the binary file in input            - COMMON

	std::string             t_s_data_version;   // Version of Data format as string             - COMMON
	std::string             t_s_sw_version;     // Version of Janus Release                     - COMMON
	uint16_t                t_brd_ver;      // Type of FERS board (5202, 5203 ...)              - COMMON
	uint8_t                 t_data_format;  // Version of Data format                                                   - COMMON

    void InitFERS(uint8_t force_ns, uint8_t mode);

public:
    t_BinaryDataFERS() {};
    t_BinaryDataFERS(uint8_t force_ns, uint8_t mode); // constructors
    t_BinaryDataFERS(std::ifstream& binfile, std::ofstream& csvfile, uint8_t force_ns);
    t_BinaryDataFERS(uint8_t mode, std::ofstream& csvfile, uint8_t force_ns, uint8_t format_version, uint16_t en_bin, uint8_t toa_bin); // In the case the header is read from main
    ~t_BinaryDataFERS() {}; // distructor

    void ReadHeaderBinfileFERS(std::ifstream& binfile);
    void ComputeBinfileSizeFERS(std::ifstream& binfile);
    void InitAnalisys(std::ifstream& binfile, std::ofstream& csvfile, uint8_t force_ns);

    std::streamoff GetEventsSize() { return t_BinaryDataFERS::t_totsize; };  // Return the size of the bin file containing the Events
    std::streamoff GetEventsBegin() { return std::streamoff(t_BinaryDataFERS::t_begin); };  // Return the file position where the Events start

    void ReadEvtHeaderFERS(std::ifstream& binfile);
    //uint16_t ReadEvtHeader5202(std::ifstream& binfile);      //   - 5202
    //uint16_t ReadEvtHeader5203(std::ifstream& binfile);      //   - 5203
    void ReadTmpEvtFERS(std::ifstream& binfile);
    //uint16_t ReadStreamingEvent(std::ifstream& binfile);     //   - 5203
    //uint16_t ReadCStartTMatchEvent(std::ifstream& binfile);  //   - 5203
    //uint16_t ReadSpectTime(std::ifstream& binfile);          //   - 5202
    //uint16_t ReadCnts(std::ifstream& binfile);               //   - 5202
    void WriteTmpEvtFERS(std::ofstream& csvfile);
};

t_BinaryDataFERS::t_BinaryDataFERS(uint8_t force_ns, uint8_t mode) {
    t_BinaryDataFERS::InitFERS(force_ns, mode);
}

//t_BinaryDataFERS::t_BinaryDataFERS(uint8_t mode, std::ofstream& csvfile, uint8_t force_ns, uint8_t format_version, uint16_t en_bin, uint8_t toa_bin) { // When header is read in the main
//    // The other parameters read in the main can be passed as a map. To be implemented
//    t_BinaryDataFERS::Init(force_ns, mode);
//    t_BinaryDataFERS::WriteCsvHeader(csvfile);
//}

t_BinaryDataFERS::t_BinaryDataFERS(std::ifstream& binfile, std::ofstream& csvfile, uint8_t force_ns) {
    // binfile is already opened
    // Which values need to be set?
    t_BinaryDataFERS::InitFERS(force_ns, 0);    // 
    t_BinaryDataFERS::ComputeBinfileSizeFERS(binfile);
    t_BinaryDataFERS::ReadHeaderBinfileFERS(binfile);
    binfile.seekg(t_BinaryDataFERS::t_begin, std::ios::beg);
    if (t_brd_ver == 5202)
        t_data_5202 = t_BinaryData(binfile, csvfile, force_ns);
    else if (t_brd_ver == 5203)
        t_data_5203 = t_BinaryData_5203(binfile, csvfile, force_ns);
}

void t_BinaryDataFERS::InitFERS(uint8_t force_ns, uint8_t mode) {
    t_BinaryDataFERS::t_s_data_version = "";
    t_BinaryDataFERS::t_s_sw_version = "";
    t_brd_ver = 0;
    t_data_format = 0;
}

void t_BinaryDataFERS::InitAnalisys(std::ifstream& binfile, std::ofstream& csvfile, uint8_t force_ns) {
    t_BinaryDataFERS::ComputeBinfileSizeFERS(binfile);
    t_BinaryDataFERS::ReadHeaderBinfileFERS(binfile);
    binfile.seekg(t_BinaryDataFERS::t_begin, std::ios::beg);
    if (t_brd_ver == 5202)
        t_data_5202 = t_BinaryData(binfile, csvfile, force_ns);
    else if (t_brd_ver == 5203)
        t_data_5203 = t_BinaryData_5203(binfile, csvfile, force_ns);
}

void t_BinaryDataFERS::ComputeBinfileSizeFERS(std::ifstream& binfile) {
    t_BinaryDataFERS::t_begin = binfile.tellg();    // Position of the reading pointer after the binfile Header
    binfile.seekg(0, std::ios::end);
    end = binfile.tellg();
    binfile.seekg(t_BinaryDataFERS::t_begin, std::ios::beg);
    t_BinaryDataFERS::t_totsize = end - t_BinaryDataFERS::t_begin;
    //t_read_size = t_begin;
}

void t_BinaryDataFERS::ReadHeaderBinfileFERS(std::ifstream& binfile) {
    uint8_t tmp_8 = 0;
    uint16_t tmp_16 = 0;

    // Read Data Format Version
    binfile.read((char*)&tmp_8, sizeof(tmp_8));
    t_BinaryDataFERS::t_s_data_version = std::to_string(tmp_8) + ".";
    t_BinaryDataFERS::t_data_format = 10 * tmp_8;

    binfile.read((char*)&tmp_8, sizeof(tmp_8));
    t_BinaryDataFERS::t_s_data_version += std::to_string(tmp_8);
    t_BinaryDataFERS::t_data_format += tmp_8;

    //binfile.seekg(3, std::ios::cur); // Skip software version (3 bytes)
    for (int i = 0; i < 3; ++i) {   // Get software version on 3 bytes
        binfile.read((char*)&tmp_8, sizeof(tmp_8));
        if (i != 0) t_BinaryDataFERS::t_s_sw_version += ".";
        t_BinaryDataFERS::t_s_sw_version += std::to_string(tmp_8);
    }

    if (t_BinaryDataFERS::t_data_format >= 32) { // Skip Board version (2 bytes)
        binfile.read((char*)&tmp_16, sizeof(tmp_16));
        t_BinaryDataFERS::t_brd_ver = tmp_16;
    } else
        t_brd_ver = 5202;
}


void t_BinaryDataFERS::ReadTmpEvtFERS(std::ifstream& binfile) {
    if (t_brd_ver == 5202)
        t_data_5202.ReadTmpEvt(binfile);
    else if (t_brd_ver == 5203)
        t_data_5203.ReadTmpEvt(binfile);
}

void t_BinaryDataFERS::WriteTmpEvtFERS(std::ofstream& csvfile) {
    if (t_brd_ver == 5202)
        t_data_5202.WriteTmpEvt(csvfile);
    else if (t_brd_ver == 5203)
        t_data_5203.WriteTmpEvt(csvfile);
}