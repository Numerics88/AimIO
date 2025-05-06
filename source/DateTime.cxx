// Copyright (c) Steven Boyd
// See LICENSE for details.
// From David Gobbi, https://github.com/dgobbi/vtk-dicom/blob/master/Source/vtkScancoCTReader.cxx#L208

#include "AimIO/DateTime.h"
#include <iostream>
#include <sstream>
#include <format>
#include <iomanip>
// #include <boost/endian/conversion.hpp>
// #include <boost/endian/arithmetic.hpp>
#include <boost/cstdint.hpp>
//
// using namespace boost::endian;

namespace AimIO
{

void FormatDateTime 
  (
  std::string& formatted_date,
  const int year,
  const int month,
  const int day,
  const int hour,
  const int minute,
  const int second,
  const int millis
  )
{

  int m = ((month > 12 || month < 1) ? 0 : month);
  static const char *months[] = { "XXX", "JAN", "FEB", "MAR", "APR", "MAY",
                                  "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
      
  std::stringstream buffer;
  buffer << std::setw(2) << std::setfill(' ') << day << "-"
         << std::setw(3) << months[m] << "-"
         << std::setw(4) << year << " "
         << std::setw(2) << std::setfill('0') << hour << ":"
         << std::setw(2) << std::setfill('0') << minute << ":"
         << std::setw(2) << std::setfill('0') << second << "."
         << std::setw(2) << std::setprecision(2) << (float)(millis/10)
         << std::endl;

  formatted_date = buffer.str();
}

void DecodeDate (
    boost::int32_t low,
    boost::int32_t high,
    int& year, 
    int& month, 
    int& day,
    int& hour, 
    int& minute, 
    int& second, 
    int& millis)
{
  // This is the offset between the astronomical "Julian day", which counts
  // days since January 1, 4713BC, and the "VMS epoch", which counts from
  // November 17, 1858:
  const uint64_t julianOffset = 2400001;
  const uint64_t millisPerSecond = 1000;
  const uint64_t millisPerMinute = 60 * 1000;
  const uint64_t millisPerHour = 3600 * 1000;
  const uint64_t millisPerDay = 3600 * 24 * 1000;

  // Read the date as a long integer with units of 1e-7 seconds
  boost::int32_t d1 = vms_to_native_int32(&low);
  boost::int32_t d2 = vms_to_native_int32(&high);
  boost::uint64_t tVMS = d1 + (static_cast<uint64_t>(d2) << 32);
  boost::uint64_t time = tVMS/10000 + julianOffset*millisPerDay;

  boost::int64_t y, m, d;
  boost::int64_t julianDay = static_cast<boost::int64_t>(time / millisPerDay);
  time -= millisPerDay*julianDay;

  // Gregorian calendar starting from October 15, 1582
  // This algorithm is from Henry F. Fliegel and Thomas C. Van Flandern
  boost::int64_t ell, n, i, j;
  ell = julianDay + 68569;
  n = (4 * ell) / 146097;
  ell = ell - (146097 * n + 3) / 4;
  i = (4000 * (ell + 1)) / 1461001;
  ell = ell - (1461 * i) / 4 + 31;
  j = (80 * ell) / 2447;
  d = ell - (2447 * j) / 80;
  ell = j / 11;
  m = j + 2 - (12 * ell);
  y = 100 * (n - 49) + i + ell;

  // Return the result
  year = y;
  month = m;
  day = d;
  hour = static_cast<boost::int64_t>(time / millisPerHour);
  time -= hour*millisPerHour;
  minute = static_cast<boost::int64_t>(time / millisPerMinute);
  time -= minute*millisPerMinute;
  second = static_cast<boost::int64_t>(time / millisPerSecond);
  time -= second*millisPerSecond;
  millis = static_cast<boost::int64_t>(time);
}

}  // namespace
