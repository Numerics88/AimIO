// Copyright (c) Steven Boyd
// See LICENSE for details.

#include "AimIO/AimIO.h"
#include "AimIO/IsqIO.h"
#include "AimIO/DateTime.h"
#include "AimIO/Definitions.h"
#include "PlatformFloat.h"  

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>

static void show_usage()
{
  std::cerr << "\n"
            << "ctheader Version 2.0.0. Numerics88 Solutions.\n" 
            << "\n"
            << "Format: ctheader isq_file [-help]\n"
            << "    --meta, -m : show scan meta data\n"
            << "                 [name] [samp] [meas] [site]\n"
            << "    --help, -h : show help\n"
            << "\n"
            << "This is a clone of CTHEADER so behaviours may differ.\n" 
            << std::endl;
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
  } else if (site.compare("5")==0) {
    return "QC";
  }
  
  return site;
}

//-----------------------------------------------------------------------
int main(int argc, char **argv)
  {
    
  AimIO::IsqFile reader;
  
  std::string fname;
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
  
    std::string patient_name = reader.name;
    std::string index_patient = std::to_string( reader.patient_index );
    std::string index_measurement = std::to_string( reader.index_measurement );
    std::string site = std::to_string( reader.site );

    std::string site_name = GetSiteName(site);

    std::cout << "\"" << patient_name << "\"" << " "
              << "\"" << index_patient << "\"" << " "
              << "\"" << index_measurement << "\"" << " "
                << "\"" << site_name << "\""
              << std::endl;
  
    return 0;
  }

  // Print header information
  std::cout << std::endl;
  std::cout << "Type: IMA-Data Sequence" << std::endl;
  std::cout << std::right << std::setw(22) << "Patient Name : "  
            << std::left << std::setw(50) << reader.name << std::endl;
  std::cout << std::right << std::setw(22) << "Patient Index : "  
            << std::left << std::setw(50) << reader.patient_index << std::endl;
  std::cout << std::right << std::setw(22) << "Measurement Index : "  
            << std::left << std::setw(50) << reader.index_measurement << std::endl;
  std::cout << std::endl;
  std::cout << std::right << std::setw(22) << "Creation Date : "  
            << std::left << std::setw(30) << reader.creation_date_string << std::endl;
  std::cout << std::endl;
  std::cout << std::right << std::setw(22) << "Dim X : "
            << std::setw(6) << std::right << reader.dimensions_p[0] << " [p] "
            << std::setw(7) << std::right << reader.dimensions_um[0] << " [um] "
            << std::endl;
  std::cout << std::right << std::setw(22) << "Dim Y : "
            << std::setw(6) << std::right << reader.dimensions_p[1] << " [p] "
            << std::setw(7) << std::right << reader.dimensions_um[1] << " [um] "
            << std::endl;
  std::cout << std::right << std::setw(22) << "Dim Z : "
            << std::setw(6) << std::right << reader.dimensions_p[2] << " [p] "
            << std::setw(7) << std::right << reader.dimensions_um[2] << " [um] "
            << std::endl;
  std::cout << std::endl;
  std::cout << std::right << std::setw(22) << "Size (bytes/blocks) : "  
            << reader.nr_of_bytes << "/" << reader.nr_of_blocks << std::endl;
  std::cout << std::right << std::setw(22) << "Data offset (bytes) : "  
            << std::left << std::setw(6) << reader.data_offset << std::endl;
  std::cout << std::right << std::setw(22) << "Min/Max Value : "  
            << reader.min_data_value << "/" << reader.max_data_value << std::endl;
  std::cout << std::endl;
  std::cout << std::right << std::setw(22) << "Site : "  
            << std::right << std::setw(6) << reader.site << std::endl;
  std::cout << std::right << std::setw(22) << "Scanner-ID : "  
            << std::right << std::setw(6) << reader.scanner_id << std::endl;
  std::cout << std::right << std::setw(22) << "Scanner_type : "  
            << std::right << std::setw(6) << reader.scanner_type << std::endl;
  std::cout << std::right << std::setw(22) << "FOV/Diameter : "  
            << std::right << std::setw(6) << reader.scandist_um << " [µm]" << std::endl;
  std::cout << std::right << std::setw(22) << "Voxel Size : "  
            << std::right << std::setw(6) << std::fixed << std::setprecision(1) << reader.spacing[0] << " [µm]" << std::endl;
  std::cout << std::right << std::setw(22) << "Slice Increment : "  
            << std::right << std::setw(6) << std::fixed << std::setprecision(1) << reader.spacing[2] << " [µm]" << std::endl;
  std::cout << std::right << std::setw(22) << "Z-Position Slice 1 : "  
            << std::right << std::setw(6) << reader.slice_1_pos_um << " [µm]" << std::endl;
  std::cout << std::right << std::setw(22) << "Reference-Line : "  
            << std::right << std::setw(6) << reader.reference_line_um << " [µm]" << std::endl;
  std::cout << std::right << std::setw(22) << "#Samples : "  
            << std::right << std::setw(6) << reader.nr_of_samples << std::endl;
  std::cout << std::right << std::setw(22) << "#Projections : "  
            << std::right << std::setw(6) << reader.nr_of_projections << std::endl;
  std::cout << std::right << std::setw(22) << "Sampletime : "  
            << std::right << std::setw(6) << reader.sampletime_us << " [µs]" << std::endl;
  std::cout << std::right << std::setw(23) << "µ-Scaling : "  
            << std::right << std::setw(6) << reader.mu_scaling << std::endl;
  std::cout << std::right << std::setw(22) << "Reconstruction-Alg : "  
            << std::right << std::setw(6) << (reader.recon_alg!=3 ? "Unknown" : "Conebeam Conv./Backpr.") << std::endl; // codes other than 3 are unknown
  std::cout << std::right << std::setw(22) << "Energy : "  
            << std::right << std::setw(6) << reader.energy << " [V]" << std::endl;
  std::cout << std::right << std::setw(22) << "Intensity : "  
            << std::right << std::setw(6) << reader.intensity << " [µA]" << std::endl;
  std::cout << std::right << std::setw(22) << "Rotated by : "  
            << std::right << std::setw(6) << "0.0" << " [deg]" << std::endl; // rotation is not captured in the ISQ header.
  std::cout << std::right << std::setw(22) << "Holder : "  
            << std::right << std::setw(6) << reader.holder << std::endl;
  
  return 0;
}
