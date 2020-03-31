/*=========================================================================

  Copyright (c) 2020, Numerics88 Solutions Ltd.
  All rights reserved.

=========================================================================*/

#include "AimIO/AimIO.h"
#include "Compression.h"
#include "PlatformFloat.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>
#include <sstream>
#include <iostream>

//-----------------------------------------------------------------------
int main(int argc, char **argv)
  {
    
	AimIO::AimFile reader;
	
  std::cout << "Running AIX.\n";
	
	reader.filename = "/Users/skboyd/Documents/data/aim/test50.aim";
	reader.ReadImageInfo();
	
	std::cout << "The dimensions are " << reader.dimensions << "\n";
	std::cout << "The position is  " << reader.position << "\n";
	std::cout << "The processing log is " << reader.processing_log << "\n";
	
	return 0;
}