/*=========================================================================

  Copyright (c) 2020, Numerics88 Solutions Ltd.
  All rights reserved.

=========================================================================*/

#include "AimIO/AimIO.h"
#include "AimIO/Definitions.h"
#include "Compression.h"
#include "PlatformFloat.h"  

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>
#include <sstream>
#include <iostream>
#include <regex>
#include <iomanip>

static void show_usage()
{
  std::cerr << "\n"
            << "Format: aix aim_file [-help] [-log] [-meta]\n"
            << "    --log, -l  : show processing log\n"
            << "    --meta, -m : show scan meta data\n"
            << "                 [name] [samp] [meas] [site]\n"
            << "    --help, -h : show help\n"
            << "\n"
            << "This is a clone of AIX so behaviours may differ.\n" 
            << std::endl;
}

// Given a log from a Scanco AIM and a specific search string, finds the value
std::string GetFieldFromLog(const std::string& log, const std::string& field) {

  int s1 = log.find(field);                  // Find field string start position
  if (s1 == std::string::npos) {             // If field not found, return result
    return "not_found";
  }
  int s2 = s1 + field.length();              // Calculate field string end 
  int s3 = log.find_first_not_of(" ",s2);    // Look forwards to find start of result
  int s4 = log.find("\n",s2);                // Find end of line
  int s5 = s4 - s3;                          // Difference is the result length
  std::string result = log.substr(s3,s5);    // Grab the rough result
  int s6 = result.find_last_not_of(" ");     // Remove trailing blank space
  result = result.substr(0,s6+1);            // Grab the result

  return result;
}

// Translates the Scanco site codes 
std::string GetSiteName(const std::string& site) {
  if (site.compare("20")==0) {
    return "RL";
  } else if (site.compare("21")==0) {
    return "RR";
  } else if (site.compare("38")==0) {
    return "TL";
  } else if (site.compare("39")==0) {
    return "TR";
  } else if (site.compare("4")==0) {
    return "CU";
  }
  
  return site;
}

//-----------------------------------------------------------------------
int main(int argc, char **argv)
  {
    
  AimIO::AimFile reader;
  
  std::string fname;
  bool show_log = false;
  bool show_meta = false;
  
  if (argc < 2) {
    show_usage();
    return 1;
  }
  
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      show_usage();
      return 0;
    } else if ((arg == "-l") || (arg == "--log")) {
      show_log = true;
    } else if ((arg == "-m") || (arg == "--meta")) {
      show_meta = true;
    } else {
      fname = arg;
    }
  }

  // Check if file exists
  if (!boost::filesystem:: exists(fname)) {
    std::cout << "Arguments:\n"
              << "fname = " << fname << "\n"
              << "show_log = " << show_log << "\n"
              << "show_meta = " << show_meta << "\n"
              << std::endl;
    std::cout << "ERROR! File does not exist: " << fname << std::endl;
    return 1;
  }

  // Read the file
  reader.filename = fname;
  reader.ReadImageInfo();

  // Show meta data in a format that fits well with FEA workflow.
  if (show_meta) {
    
    std::string patient_name = GetFieldFromLog(reader.processing_log,"Patient Name");
    std::string index_patient = GetFieldFromLog(reader.processing_log,"Index Patient");
    std::string index_measurement = GetFieldFromLog(reader.processing_log,"Index Measurement");
    std::string site = GetFieldFromLog(reader.processing_log,"Site");
    
    std::string site_name = GetSiteName(site);
    
    std::cout << "\"" << patient_name << "\"" << " "
              << "\"" << index_patient << "\"" << " "
              << "\"" << index_measurement << "\"" << " "
              << "\"" << site_name << "\""
              << std::endl;
    
    return 0;
  }
  
  // Print standard examine information
  std::cout << "!%  Image Data starts at byte offset " << reader.byte_offset << std::endl;
  switch( reader.version ) {
    case AimIO::AIMFILE_VERSION_10:
      std::cout << "!> Reading AimVersion010" << std::endl;
      break;
    case AimIO::AIMFILE_VERSION_11:
      std::cout << "!> Reading AimVersion011" << std::endl;
      break;
    case AimIO::AIMFILE_VERSION_20:
      std::cout << "!> Reading AimVersion020" << std::endl;
      break;
    case AimIO::AIMFILE_VERSION_30:
      std::cout << "!> Reading AimVersion030" << std::endl;
      break;
    default:
      std::cout << "ERROR! Unknown version of AIM file." << std::endl;
  }
  std::cout << std::endl;
  std::cout << "!-------------------------------------------------------------------------------" << std::endl;
  std::cout << "!> " << std::left << std::setw(30) << "Volume" 
            << std::left << std::setw(50) << fname << std::endl;
  std::cout << "!> " << std::left << std::setw(30) << "AIM Version" 
            << std::setw(20) << std::right << std::fixed << std::setprecision(1) << (float)reader.version << std::endl;
  std::cout << "!>" << std::endl;
  std::cout << "!> " << std::left << std::setw(30) << "dim" << "   "
            << std::setw(7) << std::right << reader.dimensions[0] << " "
            << std::setw(7) << std::right << reader.dimensions[1] << " "
            << std::setw(7) << std::right << reader.dimensions[2] 
            << std::endl;
  std::cout << "!> " << std::left << std::setw(30) << "off" << "   "
            << std::setw(7) << std::right << reader.offset[0] << " "
            << std::setw(7) << std::right << reader.offset[1] << " "
            << std::setw(7) << std::right << reader.offset[2] 
            << std::endl;
  std::cout << "!> " << std::left << std::setw(30) << "pos" << "   "
            << std::setw(7) << std::right << reader.position[0] << " "
            << std::setw(7) << std::right << reader.position[1] << " "
            << std::setw(7) << std::right << reader.position[2] 
            << std::endl;
  std::cout << "!> " << std::left << std::setw(30) << "element size in mm" << "   "
            << std::setw(7) << std::right << std::fixed << std::setprecision(4) << reader.element_size[0] << " "
            << std::setw(7) << std::right << std::fixed << std::setprecision(4) << reader.element_size[1] << " "
            << std::setw(7) << std::right << std::fixed << std::setprecision(4) << reader.element_size[2] 
            << std::endl;
  std::cout << "!> " << std::left << std::setw(30) << "phys dim in mm" << "   "
            << std::setw(7) << std::right << std::fixed << std::setprecision(4) << reader.element_size[0]*reader.dimensions[0] << " "
            << std::setw(7) << std::right << std::fixed << std::setprecision(4) << reader.element_size[1]*reader.dimensions[1] << " "
            << std::setw(7) << std::right << std::fixed << std::setprecision(4) << reader.element_size[2]*reader.dimensions[2] 
            << std::endl;
  std::cout << "!>" << std::endl;
    
  switch( reader.aim_type ) {
    case AimIO::AIMFILE_TYPE_D1Tchar:
      std::cout << "!> " << std::left << std::setw(30) << "Type of data" 
                << std::setw(20) << "D1Tchar" << std::endl;
      break;
    case AimIO::AIMFILE_TYPE_D1TbinCmp:
      std::cout << "!> " << std::left << std::setw(30) << "Type of data" 
                << std::setw(20) << "BinCmp     1 byte/voxel" << std::endl; // D1TbinCmp
      break;
    case AimIO::AIMFILE_TYPE_D3Tbit8:
      std::cout << "!> " << std::left << std::setw(30) << "Type of data" 
                << std::setw(20) << "D3Tbit8" << std::endl;
      break;
    case AimIO::AIMFILE_TYPE_D1TcharCmp:
      std::cout << "!> " << std::left << std::setw(30) << "Type of data" 
                << std::setw(20) << "D1TcharCmp" << std::endl;
      break;
    case AimIO::AIMFILE_TYPE_D1Tshort:
      std::cout << "!> " << std::left << std::setw(30) << "Type of data" 
                << std::setw(20) << "Short      2 byte/voxel" << std::endl; // D1Tshort
    break;
    case AimIO::AIMFILE_TYPE_D1Tfloat:
      std::cout << "!> " << std::left << std::setw(30) << "Type of data" 
                << std::setw(20) << "D1Tfloat" << std::endl;
      break;
    default:
      std::cout << "!> " << std::left << std::setw(30) << "Type of data" 
                << std::setw(20) << "unknown" << std::endl;
  }
  std::cout << "!-------------------------------------------------------------------------------" << std::endl;
  
  // Print the processing log
  if (show_log) {
    std::cout << reader.processing_log << "\n";
  }
  
  return 0;
}
