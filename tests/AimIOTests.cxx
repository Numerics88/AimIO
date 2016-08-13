// Copyright (c) Eric Nodwell
// See LICENSE for details.

#include "AimIO/AimIO.h"

#include <gtest/gtest.h>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using n88::tuplet;

char* test_dir = 0;

const char* TEST_AIM_LOG =
"!\n"
"! Processing Log\n"
"!\n"
"!-------------------------------------------------------------------------------\n"
"Created by                    ISQ_TO_AIM (IPL)                                  \n"
"Time                          13-MAY-2016 12:15:48.35                           \n"
"Original file                 dk0:[xtremect2.data.00000984.00004818]d0004808.isq;\n"
"Original Creation-Date        12-MAY-2016 14:17:12.96                           \n"
"Orig-ISQ-Dim-p                                   2304       2304        168\n"
"Orig-ISQ-Dim-um                                139852     139852      10197\n"
"!-------------------------------------------------------------------------------\n"
"Patient Name                  CAMOS_0709                                        \n"
"Index Patient                                     984\n"
"Index Measurement                                4818\n"
"!-------------------------------------------------------------------------------\n"
"Site                                               38\n"
"Scanner ID                                       3401\n"
"Scanner type                                        9\n"
"Position Slice 1 [um]                          171479\n"
"No. samples                                      2304\n"
"No. projections per 180                           900\n"
"Scan Distance [um]                             139852\n"
"Integration time [us]                           43000\n"
"Reference line [um]                            148979\n"
"Reconstruction-Alg.                                 3\n"
"Energy [V]                                      68000\n"
"Intensity [uA]                                   1470\n"
"Angle-Offset [mdeg]                                 0\n"
"Default-Eval                                      112\n"
"!-------------------------------------------------------------------------------\n"
"Mu_Scaling                                       8192\n"
"Calibration Data              68 kVp, BH: 200 mg HA/ccm, Scaling 8192, 0.2 CU   \n"
"Calib. default unit type      2 (Density)                                       \n"
"Density: unit                 mg HA/ccm                                         \n"
"Density: slope                         1.66252405e+03\n"
"Density: intercept                    -3.98609009e+02\n"
"HU: mu water                                  0.23660\n"
"!-------------------------------------------------------------------------------\n"
"Parameter name                Linear Attenuation                                \n"
"Parameter units               [1/cm]                                            \n"
"Minimum value                                -0.13696\n"
"Maximum value                                 1.05774\n"
"Average value                                 0.29759\n"
"Standard deviation                            0.14680\n"
"Scaled by factor                                 8192\n"
"Minimum data value                        -1122.00000\n"
"Maximum data value                         8665.00000\n"
"Average data value                         2437.83130\n"
"Standard data deviation                    1202.55200\n"
"!-------------------------------------------------------------------------------\n";

const char* TEST_GAUSS_LOG =
"!\n"
"! Processing Log\n"
"!\n"
"!-------------------------------------------------------------------------------\n"
"Created by                    ISQ_TO_AIM (IPL)                                  \n"
"Time                          13-MAY-2016 12:15:48.35                           \n"
"Original file                 dk0:[xtremect2.data.00000984.00004818]d0004808.isq;\n"
"Original Creation-Date        12-MAY-2016 14:17:12.96                           \n"
"Orig-ISQ-Dim-p                                   2304       2304        168\n"
"Orig-ISQ-Dim-um                                139852     139852      10197\n"
"!-------------------------------------------------------------------------------\n"
"Patient Name                  CAMOS_0709                                        \n"
"Index Patient                                     984\n"
"Index Measurement                                4818\n"
"!-------------------------------------------------------------------------------\n"
"Site                                               38\n"
"Scanner ID                                       3401\n"
"Scanner type                                        9\n"
"Position Slice 1 [um]                          171479\n"
"No. samples                                      2304\n"
"No. projections per 180                           900\n"
"Scan Distance [um]                             139852\n"
"Integration time [us]                           43000\n"
"Reference line [um]                            148979\n"
"Reconstruction-Alg.                                 3\n"
"Energy [V]                                      68000\n"
"Intensity [uA]                                   1470\n"
"Angle-Offset [mdeg]                                 0\n"
"Default-Eval                                      112\n"
"!-------------------------------------------------------------------------------\n"
"Mu_Scaling                                       8192\n"
"Calibration Data              68 kVp, BH: 200 mg HA/ccm, Scaling 8192, 0.2 CU   \n"
"Calib. default unit type      2 (Density)                                       \n"
"Density: unit                 mg HA/ccm                                         \n"
"Density: slope                         1.66252405e+03\n"
"Density: intercept                    -3.98609009e+02\n"
"HU: mu water                                  0.23660\n"
"!-------------------------------------------------------------------------------\n"
"Parameter (before) name       Linear Attenuation                                \n"
"Parameter units               [1/cm]                                            \n"
"Minimum value                                -0.13696\n"
"Maximum value                                 1.05774\n"
"Average value                                 0.29759\n"
"Standard deviation                            0.14680\n"
"Scaled by factor                                 8192\n"
"Minimum data value                        -1122.00000\n"
"Maximum data value                         8665.00000\n"
"Average data value                         2437.83130\n"
"Standard data deviation                    1202.55200\n"
"!-------------------------------------------------------------------------------\n"
"Procedure:                    D3P_SupGaussLowPass()                             \n"
"! \n"
"sigma                                         0.80000\n"
"support                                             2\n"
"! \n"
"Parameter name                Linear Attenuation                                \n"
"Parameter units               [1/cm]                                            \n"
"Minimum value                                 0.00574\n"
"Maximum value                                 0.91125\n"
"Average value                                 0.29747\n"
"Standard deviation                            0.12354\n"
"Scaled by factor                           8192.00000\n"
"Minimum data value                           47.00000\n"
"Maximum data value                         7465.00000\n"
"Average data value                         2436.83838\n"
"Standard data deviation                    1012.01801\n"
"!-------------------------------------------------------------------------------\n";


// Create a test fixture class.
class AimIOTests : public ::testing::Test
{};

// --------------------------------------------------------------------
// test implementations

TEST_F (AimIOTests, ReadImage_V2_bincmp)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_bincmp_v2.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_20, reader.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1TbinCmp, reader.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_CHAR, reader.buffer_type);
  ASSERT_EQ ((tuplet<3,int>(195,212,50)), reader.position);
  ASSERT_EQ ((tuplet<3,int>(25,25,25)), reader.dimensions);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.offset);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.supdim);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.suppos);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.subdim);
  ASSERT_EQ ((tuplet<3,float>(0.034,0.034,0.034)), reader.element_size);
  // Just check length and first and last few characters of processing log.
  ASSERT_EQ (1596, reader.processing_log.size());
  ASSERT_EQ (std::string("!\n! "), reader.processing_log.substr(0,4));
  ASSERT_EQ (std::string("---\n"), reader.processing_log.substr(1592,4));

  size_t N = long_product(reader.dimensions);
  std::vector<char> data (N);
  reader.ReadImageData (data.data(), N);

  // Since it is a lot of data, just compare first and last rows.
  char first[25] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0};
  char last[25] = {0,0,0,127,127,127,0,0,0,0,0,0,0,0,0,0,127,127,127,127,127,127,127,127,127};

  for (size_t i=0; i<25; ++i)
  {
    ASSERT_EQ (first[i], data[i]);
  }
  for (size_t i=0; i<25; ++i)
  {
    ASSERT_EQ (last[i], data[i+25*(25*25-1)]);
  }
}


TEST_F (AimIOTests, ReadImage_V3_bincmp)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_bincmp_v3.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_30, reader.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1TbinCmp, reader.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_CHAR, reader.buffer_type);
  ASSERT_EQ ((tuplet<3,int>(195,212,50)), reader.position);
  ASSERT_EQ ((tuplet<3,int>(25,25,25)), reader.dimensions);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.offset);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.supdim);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.suppos);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.subdim);
  ASSERT_EQ ((tuplet<3,float>(0.034,0.034,0.034)), reader.element_size);
  // Just check length and first and last few characters of processing log.
  ASSERT_EQ (1596, reader.processing_log.size());
  ASSERT_EQ (std::string("!\n! "), reader.processing_log.substr(0,4));
  ASSERT_EQ (std::string("---\n"), reader.processing_log.substr(1592,4));

  size_t N = long_product(reader.dimensions);
  std::vector<char> data (N);
  reader.ReadImageData (data.data(), N);

  // Since it is a lot of data, just compare first and last rows.
  char first[25] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0};
  char last[25] = {0,0,0,127,127,127,0,0,0,0,0,0,0,0,0,0,127,127,127,127,127,127,127,127,127};

  for (size_t i=0; i<25; ++i)
  {
    ASSERT_EQ (first[i], data[i]);
  }
  for (size_t i=0; i<25; ++i)
  {
    ASSERT_EQ (last[i], data[i+25*(25*25-1)]);
  }
}


TEST_F (AimIOTests, CopyImage_V2_bincmp)
{
  boost::filesystem::path fin = boost::filesystem::path(test_dir) / "test_bincmp_v2.aim";
  AimIO::AimFile reader;
  reader.filename = fin.string();
  reader.ReadImageInfo();
  size_t N = long_product(reader.dimensions);
  std::vector<char> data (N);
  reader.ReadImageData (data.data(), N);

  const char* fcopy = "copy_of_test_bincmp_v2.aim";
  AimIO::AimFile writer;
  writer.filename = fcopy;

  writer.position = reader.position;
  writer.dimensions = reader.dimensions;
  writer.element_size = reader.element_size;
  writer.processing_log = reader.processing_log;
  writer.version = AimIO::AIMFILE_VERSION_20;

  writer.WriteImageData (data.data());

  // Now read in copied file and check everything.
  AimIO::AimFile reader2;
  reader2.filename = fcopy;
  reader2.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_20, reader2.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1TbinCmp, reader2.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_CHAR, reader2.buffer_type);
  ASSERT_EQ (reader.position, reader2.position);
  ASSERT_EQ (reader.dimensions, reader2.dimensions);
  ASSERT_EQ (reader.offset, reader2.offset);
  ASSERT_EQ (reader.supdim, reader2.supdim);
  ASSERT_EQ (reader.suppos, reader2.suppos);
  ASSERT_EQ (reader.subdim, reader2.subdim);
  ASSERT_EQ (reader.element_size, reader2.element_size);
  ASSERT_EQ (reader.processing_log.size(), reader2.processing_log.size());
  ASSERT_EQ (reader.processing_log, reader2.processing_log);

  std::vector<char> data2 (N);
  reader2.ReadImageData (data2.data(), N);
  for (size_t i=0; i<N; ++i)
  {
    ASSERT_EQ (data[i], data2[i]);
  }
}


TEST_F (AimIOTests, CopyImage_V3_bincmp)
{
  boost::filesystem::path fin = boost::filesystem::path(test_dir) / "test_bincmp_v3.aim";
  AimIO::AimFile reader;
  reader.filename = fin.string();
  reader.ReadImageInfo();
  size_t N = long_product(reader.dimensions);
  std::vector<char> data (N);
  reader.ReadImageData (data.data(), N);

  const char* fcopy = "copy_of_test_bincmp_v3.aim";
  AimIO::AimFile writer;
  writer.filename = fcopy;

  writer.position = reader.position;
  writer.dimensions = reader.dimensions;
  writer.element_size = reader.element_size;
  writer.processing_log = reader.processing_log;
  writer.version = AimIO::AIMFILE_VERSION_30;

  writer.WriteImageData (data.data());

  // Now read in copied file and check everything.
  AimIO::AimFile reader2;
  reader2.filename = fcopy;
  reader2.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_30, reader2.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1TbinCmp, reader2.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_CHAR, reader2.buffer_type);
  ASSERT_EQ (reader.position, reader2.position);
  ASSERT_EQ (reader.dimensions, reader2.dimensions);
  ASSERT_EQ (reader.offset, reader2.offset);
  ASSERT_EQ (reader.supdim, reader2.supdim);
  ASSERT_EQ (reader.suppos, reader2.suppos);
  ASSERT_EQ (reader.subdim, reader2.subdim);
  ASSERT_EQ (reader.element_size, reader2.element_size);
  ASSERT_EQ (reader.processing_log.size(), reader2.processing_log.size());
  ASSERT_EQ (reader.processing_log, reader2.processing_log);

  std::vector<char> data2 (N);
  reader2.ReadImageData (data2.data(), N);
  for (size_t i=0; i<N; ++i)
  {
    ASSERT_EQ (data[i], data2[i]);
  }
}


TEST_F (AimIOTests, ReadImage_V2_short)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_short_v2.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_20, reader.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1Tshort, reader.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_SHORT, reader.buffer_type);
  ASSERT_EQ ((tuplet<3,int>(995,895,0)), reader.position);
  ASSERT_EQ ((tuplet<3,int>(108,108,100)), reader.dimensions);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.offset);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.supdim);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.suppos);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.subdim);
  ASSERT_NEAR (0.0607, reader.element_size[0], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[1], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[2], 1E-5);
  ASSERT_EQ (std::string(TEST_AIM_LOG), reader.processing_log);

  size_t N = long_product(reader.dimensions);
  std::vector<short> data (N);
  reader.ReadImageData (data.data(), N);

  // Since it is a lot of data, just compare first and last 20 values.
  short first[20] = {3128,1964,989,1254,2227,2682,2954,2959,2468,1567,1081,1102,1690,1805,2151,2723,3098,3025,3828,5473};
  short last[20] = {2316,1719,934,1361,2243,2229,1767,1640,1805,2296,3352,4142,3551,2810,2257,1801,1800,2413,2665,2248};

  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (first[i], data[i]);
  }
  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (last[i], data[N-20+i]);
  }
}


TEST_F (AimIOTests, ReadImage_V3_short)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_short_v3.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_30, reader.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1Tshort, reader.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_SHORT, reader.buffer_type);
  ASSERT_EQ ((tuplet<3,int>(995,895,0)), reader.position);
  ASSERT_EQ ((tuplet<3,int>(108,108,100)), reader.dimensions);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.offset);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.supdim);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.suppos);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.subdim);
  ASSERT_NEAR (0.0607, reader.element_size[0], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[1], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[2], 1E-5);
  ASSERT_EQ (std::string(TEST_AIM_LOG), reader.processing_log);

  size_t N = long_product(reader.dimensions);
  std::vector<short> data (N);
  reader.ReadImageData (data.data(), N);

  // Since it is a lot of data, just compare first and last 20 values.
  short first[20] = {3128,1964,989,1254,2227,2682,2954,2959,2468,1567,1081,1102,1690,1805,2151,2723,3098,3025,3828,5473};
  short last[20] = {2316,1719,934,1361,2243,2229,1767,1640,1805,2296,3352,4142,3551,2810,2257,1801,1800,2413,2665,2248};

  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (first[i], data[i]);
  }
  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (last[i], data[N-20+i]);
  }
}


TEST_F (AimIOTests, CopyImage_V2_short)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_short_v2.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();
  size_t N = long_product(reader.dimensions);
  std::vector<short> data (N);
  reader.ReadImageData (data.data(), N);

  const char* fcopy = "copy_of_test_short_v2.aim";
  AimIO::AimFile writer;
  writer.filename = fcopy;

  writer.position = reader.position;
  writer.dimensions = reader.dimensions;
  writer.element_size = reader.element_size;
  writer.processing_log = reader.processing_log;
  writer.version = AimIO::AIMFILE_VERSION_20;

  writer.WriteImageData (data.data());

  // Now read in copied file and check everything.
  AimIO::AimFile reader2;
  reader2.filename = fcopy;
  reader2.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_20, reader2.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1Tshort, reader2.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_SHORT, reader2.buffer_type);
  ASSERT_EQ (reader.position, reader2.position);
  ASSERT_EQ (reader.dimensions, reader2.dimensions);
  ASSERT_EQ (reader.offset, reader2.offset);
  ASSERT_EQ (reader.supdim, reader2.supdim);
  ASSERT_EQ (reader.suppos, reader2.suppos);
  ASSERT_EQ (reader.subdim, reader2.subdim);
  ASSERT_EQ (reader.element_size, reader2.element_size);
  ASSERT_EQ (reader.processing_log, reader2.processing_log);

  std::vector<short> data2 (N);
  reader.ReadImageData (data2.data(), N);

  for (size_t i=0; i<N; ++i)
  {
    ASSERT_EQ (data[i], data2[i]);
  }
}


TEST_F (AimIOTests, CopyImage_V3_short)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_short_v3.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();
  size_t N = long_product(reader.dimensions);
  std::vector<short> data (N);
  reader.ReadImageData (data.data(), N);

  const char* fcopy = "copy_of_test_short_v3.aim";
  AimIO::AimFile writer;
  writer.filename = fcopy;

  writer.position = reader.position;
  writer.dimensions = reader.dimensions;
  writer.element_size = reader.element_size;
  writer.processing_log = reader.processing_log;
  writer.version = AimIO::AIMFILE_VERSION_30;

  writer.WriteImageData (data.data());

  // Now read in copied file and check everything.
  AimIO::AimFile reader2;
  reader2.filename = fcopy;
  reader2.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_30, reader2.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1Tshort, reader2.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_SHORT, reader2.buffer_type);
  ASSERT_EQ (reader.position, reader2.position);
  ASSERT_EQ (reader.dimensions, reader2.dimensions);
  ASSERT_EQ (reader.offset, reader2.offset);
  ASSERT_EQ (reader.supdim, reader2.supdim);
  ASSERT_EQ (reader.suppos, reader2.suppos);
  ASSERT_EQ (reader.subdim, reader2.subdim);
  ASSERT_EQ (reader.element_size, reader2.element_size);
  ASSERT_EQ (reader.processing_log, reader2.processing_log);

  std::vector<short> data2 (N);
  reader.ReadImageData (data2.data(), N);

  for (size_t i=0; i<N; ++i)
  {
    ASSERT_EQ (data[i], data2[i]);
  }
}


TEST_F (AimIOTests, ReadImage_V2_short_offset)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_short_offset_v2.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_20, reader.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1Tshort, reader.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_SHORT, reader.buffer_type);
  ASSERT_EQ ((tuplet<3,int>(995,895,0)), reader.position);
  ASSERT_EQ ((tuplet<3,int>(108,108,100)), reader.dimensions);
  ASSERT_EQ ((tuplet<3,int>(2,2,2)), reader.offset);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.supdim);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.suppos);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.subdim);
  ASSERT_NEAR (0.0607, reader.element_size[0], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[1], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[2], 1E-5);
  ASSERT_EQ (std::string(TEST_GAUSS_LOG), reader.processing_log);

  size_t N = long_product(reader.dimensions);
  std::vector<short> data (N);
  reader.ReadImageData (data.data(), N);

  // Since it is a lot of data, just compare 20 values near middle
  size_t s = 589086;
  short sample[20] = {2047,2075,1944,1676,1513,1836,2452,2681,2422,2120,2101,2370,2902,3757,4662,5244,5269,4626,3575,2689};

  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (sample[i], data[s+i]);
  }
}


TEST_F (AimIOTests, ReadImage_V3_short_offset)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_short_offset_v3.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_30, reader.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1Tshort, reader.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_SHORT, reader.buffer_type);
  ASSERT_EQ ((tuplet<3,int>(995,895,0)), reader.position);
  ASSERT_EQ ((tuplet<3,int>(108,108,100)), reader.dimensions);
  ASSERT_EQ ((tuplet<3,int>(2,2,2)), reader.offset);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.supdim);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.suppos);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.subdim);
  ASSERT_NEAR (0.0607, reader.element_size[0], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[1], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[2], 1E-5);
  ASSERT_EQ (std::string(TEST_GAUSS_LOG), reader.processing_log);

  size_t N = long_product(reader.dimensions);
  std::vector<short> data (N);
  reader.ReadImageData (data.data(), N);

  // Since it is a lot of data, just compare 20 values near middle
  size_t s = 589086;
  short sample[20] = {2047,2075,1944,1676,1513,1836,2452,2681,2422,2120,2101,2370,2902,3757,4662,5244,5269,4626,3575,2689};

  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (sample[i], data[s+i]);
  }
}


TEST_F (AimIOTests, CopyImage_V2_short_offset)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_short_offset_v2.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();
  size_t N = long_product(reader.dimensions);
  std::vector<short> data (N);
  reader.ReadImageData (data.data(), N);

  const char* fcopy = "copy_of_test_short_offset_v2.aim";
  AimIO::AimFile writer;
  writer.filename = fcopy;

  writer.position = reader.position;
  writer.dimensions = reader.dimensions;
  writer.offset =reader.offset;
  writer.element_size = reader.element_size;
  writer.processing_log = reader.processing_log;
  writer.version = AimIO::AIMFILE_VERSION_20;

  writer.WriteImageData (data.data());

  // Now read in copied file and check everything.
  AimIO::AimFile reader2;
  reader2.filename = fcopy;
  reader2.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_20, reader2.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1Tshort, reader2.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_SHORT, reader2.buffer_type);
  ASSERT_EQ (reader.position, reader2.position);
  ASSERT_EQ (reader.dimensions, reader2.dimensions);
  ASSERT_EQ (reader.offset, reader2.offset);
  ASSERT_EQ (reader.supdim, reader2.supdim);
  ASSERT_EQ (reader.suppos, reader2.suppos);
  ASSERT_EQ (reader.subdim, reader2.subdim);
  ASSERT_EQ (reader.element_size, reader2.element_size);
  ASSERT_EQ (reader.processing_log, reader2.processing_log);

  std::vector<short> data2 (N);
  reader.ReadImageData (data2.data(), N);

  for (size_t i=0; i<N; ++i)
  {
    ASSERT_EQ (data[i], data2[i]);
  }
}


TEST_F (AimIOTests, CopyImage_V3_short_offset)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_short_offset_v3.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();
  size_t N = long_product(reader.dimensions);
  std::vector<short> data (N);
  reader.ReadImageData (data.data(), N);

  const char* fcopy = "copy_of_test_short_offset_v3.aim";
  AimIO::AimFile writer;
  writer.filename = fcopy;

  writer.position = reader.position;
  writer.dimensions = reader.dimensions;
  writer.offset =reader.offset;
  writer.element_size = reader.element_size;
  writer.processing_log = reader.processing_log;
  writer.version = AimIO::AIMFILE_VERSION_30;

  writer.WriteImageData (data.data());

  // Now read in copied file and check everything.
  AimIO::AimFile reader2;
  reader2.filename = fcopy;
  reader2.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_30, reader2.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1Tshort, reader2.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_SHORT, reader2.buffer_type);
  ASSERT_EQ (reader.position, reader2.position);
  ASSERT_EQ (reader.dimensions, reader2.dimensions);
  ASSERT_EQ (reader.offset, reader2.offset);
  ASSERT_EQ (reader.supdim, reader2.supdim);
  ASSERT_EQ (reader.suppos, reader2.suppos);
  ASSERT_EQ (reader.subdim, reader2.subdim);
  ASSERT_EQ (reader.element_size, reader2.element_size);
  ASSERT_EQ (reader.processing_log, reader2.processing_log);

  std::vector<short> data2 (N);
  reader.ReadImageData (data2.data(), N);

  for (size_t i=0; i<N; ++i)
  {
    ASSERT_EQ (data[i], data2[i]);
  }
}


TEST_F (AimIOTests, ReadImage_V2_charcmp)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_charcmp_v2.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_20, reader.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1TcharCmp, reader.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_CHAR, reader.buffer_type);
  ASSERT_EQ ((tuplet<3,int>(998,898,3)), reader.position);
  ASSERT_EQ ((tuplet<3,int>(102,102,94)), reader.dimensions);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.offset);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.supdim);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.suppos);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.subdim);
  ASSERT_NEAR (0.0607, reader.element_size[0], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[1], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[2], 1E-5);
  // Skip checking processing log

  size_t N = long_product(reader.dimensions);
  std::vector<char> data (N);
  reader.ReadImageData (data.data(), N);

  // Since it is a lot of data, just compare first and last 20 values.
  short first[20] = {0,0,0,0,0,0,0,0,0,0,0,4,2,2,2,0,0,0,3,3};
  short last[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (first[i], data[i]);
  }
  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (last[i], data[N-20+i]);
  }
}


TEST_F (AimIOTests, ReadImage_V3_charcmp)
{
  boost::filesystem::path filename = boost::filesystem::path(test_dir) / "test_charcmp_v3.aim";

  AimIO::AimFile reader;
  reader.filename = filename.string();
  reader.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_30, reader.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1TcharCmp, reader.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_CHAR, reader.buffer_type);
  ASSERT_EQ ((tuplet<3,int>(998,898,3)), reader.position);
  ASSERT_EQ ((tuplet<3,int>(102,102,94)), reader.dimensions);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.offset);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.supdim);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.suppos);
  ASSERT_EQ ((tuplet<3,int>(0,0,0)), reader.subdim);
  ASSERT_NEAR (0.0607, reader.element_size[0], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[1], 1E-5);
  ASSERT_NEAR (0.0607, reader.element_size[2], 1E-5);
  // Skip checking processing log

  size_t N = long_product(reader.dimensions);
  std::vector<char> data (N);
  reader.ReadImageData (data.data(), N);

  // Since it is a lot of data, just compare first and last 20 values.
  short first[20] = {0,0,0,0,0,0,0,0,0,0,0,4,2,2,2,0,0,0,3,3};
  short last[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (first[i], data[i]);
  }
  for (size_t i=0; i<20; ++i)
  {
    ASSERT_EQ (last[i], data[N-20+i]);
  }
}


TEST_F (AimIOTests, CopyImage_V2_charcmp)
{
  boost::filesystem::path fin = boost::filesystem::path(test_dir) / "test_charcmp_v2.aim";
  AimIO::AimFile reader;
  reader.filename = fin.string();
  reader.ReadImageInfo();
  size_t N = long_product(reader.dimensions);
  std::vector<char> data (N);
  reader.ReadImageData (data.data(), N);

  const char* fcopy = "copy_of_test_charcmp_v2.aim";
  AimIO::AimFile writer;
  writer.filename = fcopy;

  writer.position = reader.position;
  writer.dimensions = reader.dimensions;
  writer.element_size = reader.element_size;
  writer.processing_log = reader.processing_log;
  writer.version = AimIO::AIMFILE_VERSION_20;

  writer.WriteImageData (data.data());

  // Now read in copied file and check everything.
  AimIO::AimFile reader2;
  reader2.filename = fcopy;
  reader2.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_20, reader2.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1TcharCmp, reader2.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_CHAR, reader2.buffer_type);
  ASSERT_EQ (reader.position, reader2.position);
  ASSERT_EQ (reader.dimensions, reader2.dimensions);
  ASSERT_EQ (reader.offset, reader2.offset);
  ASSERT_EQ (reader.supdim, reader2.supdim);
  ASSERT_EQ (reader.suppos, reader2.suppos);
  ASSERT_EQ (reader.subdim, reader2.subdim);
  ASSERT_EQ (reader.element_size, reader2.element_size);
  ASSERT_EQ (reader.processing_log.size(), reader2.processing_log.size());
  ASSERT_EQ (reader.processing_log, reader2.processing_log);

  std::vector<char> data2 (N);
  reader2.ReadImageData (data2.data(), N);
  for (size_t i=0; i<N; ++i)
  {
    ASSERT_EQ (data[i], data2[i]);
  }
}

TEST_F (AimIOTests, CopyImage_V3_charcmp)
{
  boost::filesystem::path fin = boost::filesystem::path(test_dir) / "test_charcmp_v3.aim";
  AimIO::AimFile reader;
  reader.filename = fin.string();
  reader.ReadImageInfo();
  size_t N = long_product(reader.dimensions);
  std::vector<char> data (N);
  reader.ReadImageData (data.data(), N);

  const char* fcopy = "copy_of_test_charcmp_v3.aim";
  AimIO::AimFile writer;
  writer.filename = fcopy;

  writer.position = reader.position;
  writer.dimensions = reader.dimensions;
  writer.element_size = reader.element_size;
  writer.processing_log = reader.processing_log;
  writer.version = AimIO::AIMFILE_VERSION_30;

  writer.WriteImageData (data.data());

  // Now read in copied file and check everything.
  AimIO::AimFile reader2;
  reader2.filename = fcopy;
  reader2.ReadImageInfo();

  ASSERT_EQ (AimIO::AIMFILE_VERSION_30, reader2.version);
  ASSERT_EQ (AimIO::AIMFILE_TYPE_D1TcharCmp, reader2.aim_type);
  ASSERT_EQ (AimIO::AimFile::AIMFILE_TYPE_CHAR, reader2.buffer_type);
  ASSERT_EQ (reader.position, reader2.position);
  ASSERT_EQ (reader.dimensions, reader2.dimensions);
  ASSERT_EQ (reader.offset, reader2.offset);
  ASSERT_EQ (reader.supdim, reader2.supdim);
  ASSERT_EQ (reader.suppos, reader2.suppos);
  ASSERT_EQ (reader.subdim, reader2.subdim);
  ASSERT_EQ (reader.element_size, reader2.element_size);
  ASSERT_EQ (reader.processing_log.size(), reader2.processing_log.size());
  ASSERT_EQ (reader.processing_log, reader2.processing_log);

  std::vector<char> data2 (N);
  reader2.ReadImageData (data2.data(), N);
  for (size_t i=0; i<N; ++i)
  {
    ASSERT_EQ (data[i], data2[i]);
  }
}

// --------------------------------------------------------------------
// main: custom in order to handle argument.

int main (int argc, char** argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  if (argc != 2)
  {
    std::cerr << "Need to specify location of test data\n";
    exit(-1);
  }
  test_dir = argv[1];
  return RUN_ALL_TESTS();
}
