// Copyright (c) 2015 David Gobbi
// https://github.com/dgobbi/vtk-dicom/blob/master/Source/vtkScancoCTReader.cxx#L208

#ifndef __DateTime_h
#define __DateTime_h

#include <ostream>
#include <iostream>
#include <sstream>
#include <boost/cstdint.hpp>

// Convert char data to 32-bit int (little-endian).
inline boost::int32_t vms_to_native_int32 (const void *data)
{
    const unsigned char *cp = static_cast<const unsigned char *>(data);
    return (cp[0] | (cp[1] << 8) | (cp[2] << 16) | (cp[3] << 24));
}

namespace AimIO
{

/** Formats date and time.
  *
  * This will create a string formatted as per the VMS norm: 
  *
  * DD-MMM-YYYY HH:MM:SS.CC
  *
  * David Gobbi has developed some handy methods for converting the 2*int placeholder
  * into date and time parameters. See:
  * https://github.com/dgobbi/vtk-dicom/blob/master/Source/vtkScancoCTReader.cxx#L208
  *
  */
void FormatDateTime (
    std::string& formatted_date,
    const int year,
    const int month,
    const int day,
    const int hour,
    const int minute,
    const int second,
    const int millis);

/** Converts the encoded data on VMS systems.
  *
  * David Gobbi has developed methods for converting the 2*int placeholder
  * into date and time parameters. This code is from that implementation.
  *
  */
void DecodeDate (
    boost::uint32_t low,
    boost::uint32_t high,
    int& year, 
    int& month, 
    int& day,
    int& hour, 
    int& minute, 
    int& second, 
    int& millis);


}  // namespace

#endif


