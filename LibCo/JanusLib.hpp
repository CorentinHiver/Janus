#ifndef JANUSLIB
#define JANUSLIB

/******************************************************************************
* 
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the 
* software, documentation and results solely at his own risk.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "FERSlib.h"
// #include "MultiPlatform.h"

#ifdef _WIN32
	#define popen _popen
	#define DEFAULT_GNUPLOT_PATH "..\\gnuplot\\gnuplot.exe"
	#define GNUPLOT_TERMINAL_TYPE "wxt"
	#define PATH_SEPARATOR "\\"
	#define CONFIG_FILE_PATH ""
	#define DATA_FILE_PATH "."
	#define WORKING_DIR ""
	#define EXE_NAME "JanusC.exe"
#else
    #include <unistd.h>
    #include <stdint.h>   /* C99 compliant compilers: uint64_t */
    #include <ctype.h>    /* toupper() */
    #include <sys/time.h>
	#define DEFAULT_GNUPLOT_PATH	"gnuplot"
	#define GNUPLOT_TERMINAL_TYPE "x11"
	#define PATH_SEPARATOR "/"
	#define EXE_NAME "JanusC"
	#ifndef Sleep
		#define Sleep(x) usleep((x)*1000)
	#endif
	#ifdef _ROOT_
		#define CONFIG_FILE_PATH _ROOT_"/Janus/config/"
		#define DATA_FILE_PATH _ROOT_"/Janus/"
		#define WORKING_DIR _ROOT_"/Janus/"
	#else
		#define CONFIG_FILE_PATH ""
		#define DATA_FILE_PATH "./DataFiles"
		#define WORKING_DIR ""
	#endif
#endif

#define SW_RELEASE_NUM			"4.0.0"
#define SW_RELEASE_DATE			"31/01/2025"
#define FILE_LIST_VER			"3.3"

#ifdef _WIN32
#define CONFIG_FILENAME			"Janus_Config.txt"
#define RUNVARS_FILENAME		"RunVars.txt"
#define PIXMAP_FILENAME			"pixel_map.txt"
#else	// with eclipse the Debug run from Janus folder, and the executable in Janus/Debug
#define CONFIG_FILENAME			"Janus_Config.txt"
#define RUNVARS_FILENAME		"RunVars.txt"
#define PIXMAP_FILENAME			"pixel_map.txt"
#endif

//****************************************************************************
// Definition of limits and sizes
//****************************************************************************
#define MAX_NBRD						     16	// max. number of boards 
#define MAX_NCNC						     8	// max. number of concentrators
#define MAX_NCH							     64	// max. number of channels 
#define MAX_GW							     20	// max. number of generic write commads
#define MAX_NTRACES						   8	// Max num traces in the plot

// *****************************************************************
// Parameter options
// *****************************************************************
#define OUTFILE_RAW_LL					 0x0001
#define OUTFILE_LIST_BIN				 0x0002
#define OUTFILE_LIST_ASCII			 0x0004
#define OUTFILE_LIST_CSV				 0x0008
#define OUTFILE_SPECT_HISTO			 0x0010
#define OUTFILE_ToA_HISTO				 0x0020
#define OUTFILE_TOT_HISTO				 0x0040
#define OUTFILE_STAIRCASE				 0x0080
#define OUTFILE_RUN_INFO				 0x0100
#define OUTFILE_MCS_HISTO				 0x0200
#define OUTFILE_SYNC					   0x0400
#define OUTFILE_SERVICE_INFO		 0x0800

#define OF_UNIT_LSB						   0
#define OF_UNIT_NS						   1

#define PLOT_E_SPEC_LG					 0
#define PLOT_E_SPEC_HG					 1
#define PLOT_TOA_SPEC					   2
#define PLOT_TOT_SPEC					   3
#define PLOT_CHTRG_RATE					 4
#define PLOT_WAVE						     5
#define PLOT_2D_CNT_RATE				 6
#define PLOT_2D_CHARGE_LG				 7
#define PLOT_2D_CHARGE_HG				 8
#define PLOT_SCAN_THR					   9
#define PLOT_SCAN_HOLD_DELAY		 10
#define PLOT_MCS_TIME					   11	// DNIN those value have to be sorted later

#define DATA_ANALYSIS_CNT				 0x0001
#define DATA_ANALYSIS_MEAS			 0x0002
#define DATA_ANALYSIS_HISTO			 0x0004
#define DATA_MONITOR					   0x0008

#define SMON_CHTRG_RATE					 0
#define SMON_CHTRG_CNT					 1
#define SMON_HIT_RATE					   2
#define SMON_HIT_CNT					   3
#define SMON_PHA_RATE					   4
#define SMON_PHA_CNT					   5

#define STOPRUN_MANUAL					 0
#define STOPRUN_PRESET_TIME			 1
#define STOPRUN_PRESET_COUNTS		 2

#define EVBLD_DISABLED					 0
#define EVBLD_TRGTIME_SORTING	   1
#define EVBLD_TRGID_SORTING			 2

#define TEST_PULSE_DEST_ALL 		-1
#define TEST_PULSE_DEST_EVEN		-2
#define TEST_PULSE_DEST_ODD			-3
#define TEST_PULSE_DEST_NONE		-4

#define CITIROC_CFG_FROM_REGS		 0
#define CITIROC_CFG_FROM_FILE		 1

#define SCPARAM_BRD						   0
#define SCPARAM_MIN						   1
#define SCPARAM_MAX						   2
#define SCPARAM_STEP					   3
#define SCPARAM_DWELL					   4
										
#define HDSPARAM_BRD					   0
#define HDSPARAM_MIN					   1
#define HDSPARAM_MAX					   2
#define HDSPARAM_STEP					   3
#define HDSPARAM_NMEAN				   4


// Temperatures
#define TEMP_BOARD						   0
#define TEMP_FPGA						     1


// Acquisition Status Bits
#define ACQSTATUS_SOCK_CONNECTED 1	// GUI connected through socket
#define ACQSTATUS_HW_CONNECTED	 2	// Hardware connected
#define ACQSTATUS_READY					 3	// ready to start (HW connected, memory allocated and initialized)
#define ACQSTATUS_RUNNING				 4	// acquisition running (data taking)
#define ACQSTATUS_RESTARTING		 5	// Restarting acquisition
#define ACQSTATUS_EMPTYING			 6	// Acquiring data still in the boards buffers after the software stop
#define ACQSTATUS_STAIRCASE			 10	// Running Staircase
#define ACQSTATUS_RAMPING_HV		 11	// Switching HV ON or OFF
#define ACQSTATUS_UPGRADING_FW	 12	// Upgrading the FW
#define ACQSTATUS_HOLD_SCAN			 13	// Running Scan Hold
#define ACQSTATUS_ERROR					 -1	// Error


//****************************************************************************
// struct that contains the configuration parameters (HW and SW)
//****************************************************************************
typedef struct Config_t {

	// System info 
	char ConnPath[MAX_NBRD][200];	// IP address of the board
	int NumBrd;                     // Tot number of connected boards
	int NumCh;						// Number of channels 

	int EnLiveParamChange;			// Enable param change while running (when disabled, Janus will stops and restarts the acq. when a param changes)
	int AskHVShutDownOnExit;		// Ask if the HV must be shut down before quitting
	int OutFileEnableMask;			// Enable/Disable output files 
	char DataFilePath[500];			// Output file data path
	uint8_t EnableMaxFileSize;		// Enable the Limited size for list output files. Value set in MaxOutFileSize parameter
	float MaxOutFileSize;			// Max size of list output files in bytes. Minimum size allowed 1 MB
	uint8_t EnableRawDataRead;		// Enable the readout from RawData file
	uint8_t OutFileUnit;			// Unit for time measurement in output files (0 = LSB, 1 = ns)
	int EnableJobs;					// Enable Jobs
	int JobFirstRun;				// First Run Number of the job
	int JobLastRun;					// Last Run Number of the job
	float RunSleep;					// Wait time between runs of one job
	int StartRunMode;				// Start Mode (this is a HW setting that defines how to start/stop the acquisition in the boards)
	int StopRunMode;				// Stop Mode (unlike the start mode, this is a SW setting that deicdes the stop criteria)
	int RunNumber_AutoIncr;			// auto increment run number after stop
	float PresetTime;				// Preset Time (Real Time in s) for auto stop
	int PresetCounts;				// Preset Number of counts (triggers)
	int EventBuildingMode;			// Event Building Mode
	int TstampCoincWindow;			// Time window (ns) in event buiding based on trigger Tstamp
	int DataAnalysis;				// Data Analysis Enable/disable mask

	int EHistoNbin;					// Number of channels (bins) in the Energy Histogram
	int ToAHistoNbin;				// Number of channels (bins) in the ToA Histogram
	int8_t ToARebin;				// Rebin factor for ToA histogram. New bin = 0.5*Rebin ns
	float ToAHistoMin;				// Minimum time value for ToA Histogram. Maximum is Min+0.5*Rebin*Nbin
	int ToTHistoNbin;				// Number of channels (bins) in the ToT Histogram
	int MCSHistoNbin;				// Number of channels (bins) in the MCS Histogram

	int CitirocCfgMode;				// 0=from regs, 1=from file
	//uint16_t Pedestal;				// Common pedestal added to all channels

	//                                                                       
	// Acquisition Setup (HW settings)
	//                                                                       
	// Board Settings
	uint32_t AcquisitionMode;						// ACQMODE_COUNT, ACQMODE_SPECT, ACQMODE_TIMING, ACQMODE_WAVE
	uint32_t EnableToT;								// Enable readout of ToT (time over threshold)

	uint32_t TriggerMask;	// Variable needed in plot.c. There no handle is passed
	//uint32_t WaveformLength;
	float PtrgPeriod;

} Janus_Config_t;


typedef struct RunVars_t {
	int ActiveBrd;				// Active board
	int ActiveCh;				// Active channel
	int PlotType;				// Plot Type
	int SMonType;				// Statistics Monitor Type
	int Xcalib;					// X-axis calibration
	int RunNumber;				// Run Number (used in output file name; -1 => no run number)
	char PlotTraces[MAX_NTRACES][100];	// Plot Traces (format: "0 3 X" => Board 0, Channel 3, From X[B board - F offline - S from file)
	int StaircaseCfg[5];		// Staircase Params: Board MinThr Maxthr Step Dwell
	int HoldDelayScanCfg[5];	// Hold Delay Scan Params: Board MinDelay MaxDelay Step Nmean
} RunVars_t;




//****************************************************************************
// Global Variables
//****************************************************************************
extern Janus_Config_t J_cfg;				// struct containing all acquisition parameters
extern RunVars_t RunVars;			// struct containing run time variables
extern int handle[FERSLIB_MAX_NBRD];		// board handles
extern int cnc_handle[FERSLIB_MAX_NCNC];	// concentrator handles
//extern int ActiveCh, ActiveBrd;		// Board and Channel active in the plot
extern int AcqRun;					// Acquisition running
extern int AcqStatus;				// Acquisition Status
extern int SockConsole;				// 0: use stdio console, 1: use socket console
extern char ErrorMsg[250];			// Error Message











//****************************************//
//          paramparser                   //
//****************************************//







#define PARSEMODE_FIRST_CALL		0x01
#define PARSEMODE_PARSE_CONNECTION	0x02
#define PARSEMODE_PARSE_ALL			0x04

int ParseConfigFile(FILE* f_ini, Janus_Config_t* J_cfg, int ParseMode)
{
	int i, b;
	int brd, ch;  // target board/ch defined as ParamName[b][ch] (-1 = all)
	int brd_l, brd_h;  // low/high index for multiboard settings
	char tstr[1000], str1[1000], * parval, * tparval, * parname, * token;

	if (ParseMode & PARSEMODE_PARSE_CONNECTION) 
		memset(J_cfg, 0, sizeof(Janus_Config_t));
		//J_cfg->NumBrd = 0;
	
	if (ParseMode & PARSEMODE_FIRST_CALL) { // initialize J_cfg when it is call by Janus. No when it is call for overwrite parameters 
		int num_brd = J_cfg->NumBrd;
		char mpath[MAX_NBRD][200];
		for (int i = 0; i < MAX_NBRD; ++i) 
			sprintf(mpath[i], "%s", J_cfg->ConnPath[i]);

		memset(J_cfg, 0, sizeof(Janus_Config_t));
		/* Default settings */
		strcpy(J_cfg->DataFilePath, "DataFiles");
		J_cfg->NumBrd = num_brd;	
		for (int i = 0; i < MAX_NBRD; ++i) 
			sprintf(J_cfg->ConnPath[i], "%s", mpath[i]);

		J_cfg->NumCh = 64;
		J_cfg->EHistoNbin = 4096;
		J_cfg->ToAHistoNbin = 4096;
		J_cfg->ToARebin = 1;
		J_cfg->ToAHistoMin = 0;
		J_cfg->ToTHistoNbin = 4096;
		J_cfg->MCSHistoNbin = 4096;
		J_cfg->AcquisitionMode = 0;
		J_cfg->EnableToT = 1;
		J_cfg->TriggerMask = 0;
		J_cfg->PtrgPeriod = 0;
		//J_cfg->TD_CoarseThreshold = 0;
		J_cfg->EnLiveParamChange = 1;
		J_cfg->AskHVShutDownOnExit = 1;
		J_cfg->MaxOutFileSize = 1e9; // 1 GB
		J_cfg->EnableRawDataRead = 0;
		J_cfg->EnableMaxFileSize = 0;

	}
	
	//read config file and assign parameters 
	while(!feof(f_ini)) {
		brd = -1;
		ch = -1;
		ValidParameterName = 0;
		ValidParameterValue = 1;
		ValidUnits = 1;

		// Read a line from the file
		fgets(tstr, sizeof(tstr), f_ini);
		if (strlen(tstr) < 2) continue;

		// skip comments
		if (tstr[0] == '#' || strlen(tstr) <= 2) continue;
		if (strstr(tstr, "#") != NULL) tparval = strtok(tstr, "#");
		else tparval = tstr;

		// Get param name (str) and values (parval)
		sscanf(tparval, "%s", str1);
		tparval += strlen(str1);
		parval = trim(tparval);

		// Search for boards and channels
		parname = strtok(trim(str1), "[]"); // Param name with [brd][ch]
		token = strtok(NULL, "[]");
		if (token != NULL) {
			sscanf(token, "%d", &brd);
			if ((token = strtok(NULL, "[]")) != NULL)
				sscanf(token, "%d", &ch);
		}
		if (brd != -1) {
			brd_l = brd;
			brd_h = brd + 1;
		}
		else {
			brd_l = 0;
			brd_h = (J_cfg->NumBrd > 0) ? J_cfg->NumBrd : FERSLIB_MAX_NBRD;
		}

		if (brd_l < 0 || brd_l >(FERSLIB_MAX_NBRD - 1)) { // brd_l cannot be larger than FERSLIB_MAX_NBRD - 1
			Con_printf("LCSw", "%s: brd %d index out of range\n", parname, brd_l);
			continue;
		}

		// Some name replacement for back compatibility
		if (streq(parname, "TriggerSource"))		sprintf(parname, "BunchTrgSource");
		if (streq(parname, "DwellTime"))			sprintf(parname, "PtrgPeriod");
		if (streq(parname, "TrgTimeWindow"))		sprintf(parname, "TstampCoincWindow");
		if (streq(parname, "Hit_HoldOff"))			sprintf(parname, "Trg_HoldOff");
		if (streq(parname, "PairedCnt_CoincWin"))	sprintf(parname, "ChTrg_Width");

		// START PARAM PARSER
		if (ParseMode & PARSEMODE_PARSE_CONNECTION) {
			if (streq(parname, "Open")) {
				if (brd == -1) {
					Con_printf("LCSw", "%s: cannot be a common setting (must be in a [BOARD] section)\n", parname);
				}
				else {
					if (streq(J_cfg->ConnPath[brd], ""))
						J_cfg->NumBrd++;
					strcpy(J_cfg->ConnPath[brd], parval);

					if (streq(J_cfg->ConnPath[brd], ""))
						Con_printf("LCSw", "%s: connection path cannot be empty\n", parval);
				}
			}
			continue; // Skip the rest of the code
		} 

		if (streq(parname, "Open")) continue; // Skip Open param when not parse connection

		if (streq(parname, "EventBuildingMode")) {
			if		(streq(parval, "DISABLED"))		J_cfg->EventBuildingMode = EVBLD_DISABLED;
			else if	(streq(parval, "TRGTIME_SORTING"))J_cfg->EventBuildingMode = EVBLD_TRGTIME_SORTING;
			else if	(streq(parval, "TRGID_SORTING"))	J_cfg->EventBuildingMode = EVBLD_TRGID_SORTING;
			else 	ValidParameterValue = 0;
		}
		if (streq(parname, "DataAnalysis")) {
			if (streq(parval, "NONE") || streq(parval, "DISABLED"))	J_cfg->DataAnalysis = 0;
			else if (streq(parval, "CNT_ONLY"))		J_cfg->DataAnalysis = DATA_ANALYSIS_CNT;
			else if (streq(parval, "ALL"))			J_cfg->DataAnalysis = DATA_ANALYSIS_CNT | DATA_ANALYSIS_HISTO | DATA_ANALYSIS_MEAS;
			else if (strstr(parval, "MASK"))			J_cfg->DataAnalysis = GetHex32(trim(strtok(parval, "MASK")));
			else 	ValidParameterValue = 0;
		}
		if (streq(parname, "OF_OutFileUnit")) {
			if (streq(parval, "LSB"))					J_cfg->OutFileUnit = OF_UNIT_LSB;
			else if (streq(parval, "ns"))				J_cfg->OutFileUnit = OF_UNIT_NS;
			else 	ValidParameterValue = 0;
		}
		if (streq(parname, "CitirocCfgMode")) {
			if		(streq(parval, "FROM_FILE"))		J_cfg->CitirocCfgMode = CITIROC_CFG_FROM_FILE;
			else if	(streq(parval, "FROM_REGS"))		J_cfg->CitirocCfgMode = CITIROC_CFG_FROM_REGS;
			else 	ValidParameterValue = 0;
		}

		if (streq(parname, "DataFilePath"))				GetDatapath(parval, J_cfg);
		if (streq(parname, "EHistoNbin"))				J_cfg->EHistoNbin			= GetNbin(parval);
		if (streq(parname, "ToAHistoNbin"))				J_cfg->ToAHistoNbin			= GetNbin(parval);
		if (streq(parname, "ToARebin"))					J_cfg->ToARebin				= GetInt(parval);
		if (streq(parname, "ToAHistoMin"))				J_cfg->ToAHistoMin			= GetTime(parval, "ns");
		if (streq(parname, "ToTHistoNbin"))				J_cfg->ToTHistoNbin			= GetNbin(parval);
		if (streq(parname, "MCSHistoNbin"))				J_cfg->MCSHistoNbin			= GetNbin(parval);
		if (streq(parname, "JobFirstRun"))				J_cfg->JobFirstRun			= GetInt(parval);
		if (streq(parname, "JobLastRun"))				J_cfg->JobLastRun			= GetInt(parval);
		if (streq(parname, "RunSleep"))					J_cfg->RunSleep				= GetTime(parval, "s");
		if (streq(parname, "EnableJobs"))				J_cfg->EnableJobs			= GetInt(parval);
		if (streq(parname, "EnLiveParamChange"))		J_cfg->EnLiveParamChange	= GetInt(parval);
		if (streq(parname, "OutFileEnableMask"))		J_cfg->OutFileEnableMask	= GetHex32(parval);
		if (streq(parname, "OF_EnMaxSize"))				J_cfg->EnableMaxFileSize	= GetInt(parval);
		if (streq(parname, "OF_MaxSize"))				J_cfg->MaxOutFileSize		= GetBytes(parval);
		if (streq(parname, "EnableRawDataRead"))		J_cfg->EnableRawDataRead	= GetInt(parval);
		if (streq(parname, "OF_ListLL"))				J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_RAW_LL, GetInt(parval));
		if (streq(parname, "OF_ListBin"))				J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_LIST_BIN, GetInt(parval));
		if (streq(parname, "OF_ListCSV"))				J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_LIST_CSV, GetInt(parval));
		if (streq(parname, "OF_ListAscii"))				J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_LIST_ASCII, GetInt(parval));
		if (streq(parname, "OF_Sync"))					J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_SYNC, GetInt(parval));
		if (streq(parname, "OF_SpectHisto"))			J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_SPECT_HISTO, GetInt(parval));
		if (streq(parname, "OF_ToAHisto"))				J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_ToA_HISTO, GetInt(parval));
		if (streq(parname, "OF_ToTHisto"))				J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_TOT_HISTO, GetInt(parval));
		if (streq(parname, "OF_Staircase"))				J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_STAIRCASE, GetInt(parval));
		if (streq(parname, "OF_RunInfo"))				J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_RUN_INFO, GetInt(parval));
		if (streq(parname, "OF_ServiceInfo"))			J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_SERVICE_INFO, GetInt(parval));
		if (streq(parname, "OF_MCS"))					J_cfg->OutFileEnableMask	= SETBIT(J_cfg->OutFileEnableMask, OUTFILE_MCS_HISTO, GetInt(parval));
		if (streq(parname, "TstampCoincWindow"))		J_cfg->TstampCoincWindow	= (uint32_t)GetTime(parval, "ns");
		if (streq(parname, "PresetTime"))				J_cfg->PresetTime			= GetTime(parval, "s");
		if (streq(parname, "PresetCounts"))				J_cfg->PresetCounts			= GetInt(parval);
		if (streq(parname, "RunNumber_AutoIncr"))		J_cfg->RunNumber_AutoIncr	= GetInt(parval);
		if (streq(parname, "AskHVShutDownOnExit"))		J_cfg->AskHVShutDownOnExit  = GetInt(parval);

			
		if (streq(parname, "Load")) {
			FILE* n_cfg;
			n_cfg = fopen(parval, "r");
			if (n_cfg != NULL) {
				Con_printf("LCSm", "Loading Additional config file \"%s\"\n", parval);
				ParseConfigFile(n_cfg, J_cfg, ParseMode & 0xE);
				fclose(n_cfg);
			}
			else {
				Con_printf("LCSw", "WARNING: Loading Macro: Macro file \"%s\" not found\n", parval);
				ValidParameterValue = 0;
			}
		}

		// if exists, load the extra settings contained in PostConfig.txt
		static int PostConfigDone = 0;
		if (!PostConfigDone) {
			FILE* pcfg = fopen("PostConfig.txt", "r");
			PostConfigDone = 1;
			if (pcfg != NULL) {
				Con_printf("LCSm", "Reading additional configuration file PostConfig.txt\n");
				ParseConfigFile(pcfg, J_cfg, ParseMode);
				fclose(pcfg);
				PostConfigDone = 0;
			}
		}

		//if (!ValidParameterName) IsJanusParam = 0;

		// Param name not find in Janus param list. Can be a param of the lib. Call the set_param
		if (ParseMode & PARSEMODE_PARSE_ALL) {
			if (!ValidParameterName) {
				// Append the '[ch]' if ch != 0
				if (ch >= 0)
					sprintf(parname, "%s[%d]", parname, ch);
				for (b = brd_l; b < brd_h; b++) {
					int ret;
					//printf("%s %s\n", parname, parval);
					ret = FERS_SetParam(handle[b], parname, parval);
					ValidParameterName = (ret == FERSLIB_ERR_INVALID_PARAM) ? 0 : 1;
					ValidParameterValue = (ret == FERSLIB_ERR_INVALID_PARAM_VALUE) ? 0 : 1;
					ValidUnits = (ret == FERSLIB_ERR_INVALID_PARAM_UNIT) ? 0 : 1;
				}
			}
		}

		if (!ValidParameterName)
			Con_printf("LCSw", "WARNING: %s: unkwown parameter\n", parname);
		else if (!ValidParameterValue)
			Con_printf("LCSw", "WARNING: %s: unkwown value '%s'\n", parname, parval);
		else if (!ValidUnits)
			Con_printf("LCSw", "WARNING: %s: unkwown units. Janus use as default V, mA, ns\n", parname);
	}

	if (!(ParseMode & PARSEMODE_FIRST_CALL)) return 0; // The code below must be executed just on the first call


	if (J_cfg->EHistoNbin > (1 << ENERGY_NBIT))	J_cfg->EHistoNbin = (1 << ENERGY_NBIT);
	if (J_cfg->ToAHistoNbin > (1 << TOA_NBIT))	J_cfg->ToAHistoNbin = (1 << TOA_NBIT);	// DNIN: misleading. This is just for plot visualization
	if (J_cfg->ToTHistoNbin > (1 << TOT_NBIT))	J_cfg->ToTHistoNbin = (1 << TOT_NBIT);
	int ediv = 1;

	J_cfg->AcquisitionMode = FERS_GetParam_int(handle[0], "AcquisitionMode");
	J_cfg->StartRunMode = FERS_GetParam_int(handle[0], "StartRunMode");
	J_cfg->StopRunMode = FERS_GetParam_int(handle[0], "StopRunMode");

	J_cfg->PtrgPeriod = FERS_GetParam_float(handle[0], "PtrgPeriod");
	J_cfg->TriggerMask = FERS_GetParam_hex(handle[0], "TriggerMask");


#ifdef linux
	if (J_cfg->DataFilePath[strlen(J_cfg->DataFilePath)-1] != '/')	sprintf(J_cfg->DataFilePath, "%s/", J_cfg->DataFilePath);
#else
	if (J_cfg->DataFilePath[strlen(J_cfg->DataFilePath)-1] != '\\')	sprintf(J_cfg->DataFilePath, "%s\\", J_cfg->DataFilePath);
#endif

	// Force options when connection is offline
	for (i = 0; i < J_cfg->NumBrd; ++i) {
		if (strstr(J_cfg->ConnPath[i], "offline") != NULL) {
			if (J_cfg->StopRunMode != STOPRUN_MANUAL) {
				Con_printf("LCSw", "WARNING: Manual Stop Run must be set when Raw Data are processed\n");
				J_cfg->StopRunMode = STOPRUN_MANUAL;
			}
			if (J_cfg->EnableJobs != 0) {
				Con_printf("LCSw", "WARNING: Jobs are not permitted when Raw Data are processed\n");
				J_cfg->EnableJobs = 0;
			}
			break;
		}
	}

	// SetParam for RawData saving
	char buffLS[50];
	char buffMS[50];
	sprintf(buffLS, "%" PRIu8, J_cfg->EnableMaxFileSize);
	sprintf(buffMS, "%f", J_cfg->MaxOutFileSize);
	for (int i = 0; i < J_cfg->NumBrd; ++i) {
		FERS_SetParam(handle[i], "OF_RawDataPath", J_cfg->DataFilePath);
		FERS_SetParam(handle[i], "OF_LimitedSize", buffLS);
		FERS_SetParam(handle[i], "MaxSizeOutputDataFile", buffMS);
	}

	return 0;
}

#endif //JANUSLIB