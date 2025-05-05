// Copyright (c) Steven Boyd
// See LICENSE for details.

#include "AimIO/IsqIO.h"
#include "AimIO/AimIO.h"
#include "Compression.h"
#include "PlatformFloat.h"
#include <boost/endian/conversion.hpp>
#include <boost/endian/arithmetic.hpp>
#include <iostream>
#include <sstream>


using namespace boost::endian;
using n88::tuplet;

namespace AimIO
{

// ===========================================================================
// Structs and consts defining AIM files

const char*  version_string = "CTDATA-HEADER_V1";  //  16 char plus \0

typedef struct 
  {
  /*---------------------------------------------*/
  char check[16];
  little_int32_t data_type;
  little_int32_t nr_of_bytes;   /* either one of them     */
  little_int32_t nr_of_blocks;  /* or both, but min. of 1 */
  little_int32_t patient_index; /* 1 block = 512 bytes    */
  little_int32_t scanner_id;
  little_int32_t creation_date[2];
  /*---------------------------------------------*/
  tuplet<3,little_int32_t> dim_p;
  tuplet<3,little_int32_t> dim_um;
  little_int32_t slice_thickness_um;
  little_int32_t slice_increment_um;
  little_int32_t slice_1_pos_um;
  little_int32_t min_data_value;
  little_int32_t max_data_value;
  little_int32_t mu_scaling; /* p(x,y,z)/mu_scaling = value [1/cm] */
  little_int32_t nr_of_samples;
  little_int32_t nr_of_projections;
  little_int32_t scandist_um;
  little_int32_t scanner_type;
  little_int32_t sampletime_us;
  little_int32_t index_measurement;
  little_int32_t site; /* Coded value */
  little_int32_t reference_line_um;
  little_int32_t recon_alg; /* Coded value */
  char name[40];
  little_int32_t energy; /*V */
  little_int32_t intensity; /* uA */
  little_int32_t fill[83];
  /*---------------------------------------------*/
  little_int32_t data_offset; /* in 512-byte-blocks */
} ima_data_type;



// ===========================================================================
// Methods

// ---------------------------------------------------------------------------
IsqFile::IsqFile ()
  :
  name(40,' '),
  version (AimIO::ISQFILE_VERSION_1),
  data_type (0),
  nr_of_bytes (0),
  nr_of_blocks (0),
  patient_index (0),
  scanner_id (0),
  creation_date (0),
  dimensions_p (0,0,0),
  dimensions_um (0,0,0),
  offset (0,0,0),
  slice_thickness_um (0),
  slice_increment_um (0),
  slice_1_pos_um (0),
  min_data_value (0),
  max_data_value (0),
  mu_scaling (0),
  nr_of_samples (0),
  nr_of_projections (0),
  scandist_um (0),
  scanner_type (0),
  sampletime_us (0),
  index_measurement (0),
  site (0),
  reference_line_um (0),
  recon_alg (0),
  energy (0),
  intensity (0),
  data_offset (0)
  {}


// ---------------------------------------------------------------------------
IsqFile::IsqFile (const char* fn)
  :
  filename (fn),
  name(40,' '),
  version (AimIO::ISQFILE_VERSION_1),
  data_type (0),
  nr_of_bytes (0),
  nr_of_blocks (0),
  patient_index (0),
  scanner_id (0),
  creation_date (0),
  dimensions_p (0,0,0),
  dimensions_um (0,0,0),
  offset (0,0,0),
  slice_thickness_um (0),
  slice_increment_um (0),
  slice_1_pos_um (0),
  min_data_value (0),
  max_data_value (0),
  mu_scaling (0),
  nr_of_samples (0),
  nr_of_projections (0),
  scandist_um (0),
  scanner_type (0),
  sampletime_us (0),
  index_measurement (0),
  site (0),
  reference_line_um (0),
  recon_alg (0),
  energy (0),
  intensity (0),
  data_offset (0)
  {}


// ---------------------------------------------------------------------------
void IsqFile::ReadBlockList (std::ifstream& f)
{

  // Read in first 16 bytes
  std::vector<char> buffer (512);
  f.read (&(buffer[0]), 512);

  // Check if the header string valid
  if (strncmp (&(buffer[0]), version_string, 16) != 0) {
    throw_aimio_exception ("Only file version CTDATA-HEADER_V1 can be read.");
  }
  
  // ISQ files have one memory block with header and another containing the image data
  int nr_mb = 2;
  this->block_list.resize (nr_mb);

  // Header is the first 512 bytes
  this->block_list[0].size = 512;
  this->block_list[0].offset = 0;
  
  // The number of empty 512 data blocks is defined in the header block at offset 508.
  this->block_list[1].offset = 512 * (1 + little_to_native(*(reinterpret_cast<boost::int32_t*>(&(buffer[0]) + 508))));
  // ISQ data size is equal to nr_of_bytes defined in header block at offset 20 minus 
  // the offset number of bytes to the start of ISQ data (equivalent to dimx*dimy*dimz*size) 
  this->block_list[1].size = little_to_native(*(reinterpret_cast<boost::int32_t*>(&(buffer[0]) + 20))) - this->block_list[1].offset;
  
}

// ---------------------------------------------------------------------------
void IsqFile::ReadHeader (std::ifstream& f)
{
  aimio_assert (this->block_list.size());
  
  if (this->block_list[0].size == sizeof(ima_data_type) )
  {
    ima_data_type fd;
    f.seekg (this->block_list[0].offset);
    f.read ((char*)&fd, sizeof(ima_data_type));

    // std::cout << "----------------------------------------------------------------" << std::endl;
    // std::cout << "fill[80] = " << fd.fill[80] << " --> mystery value" << std::endl;
    // std::cout << "fill[81] = " << fd.fill[81] << " --> mystery value" << std::endl;
    // std::cout << "----------------------------------------------------------------" << std::endl;
    
    fd.name[40-1]='\0'; // ensures terminating character at end of string
    this->name = fd.name;
    this->patient_index = fd.patient_index;
    this->index_measurement = fd.index_measurement;
    
    this->creation_date = (boost::int64_t)fd.creation_date;
    
    this->dimensions_p = fd.dim_p;
    this->dimensions_um = fd.dim_um;

    this->nr_of_bytes = fd.nr_of_bytes;
    this->nr_of_blocks = fd.nr_of_blocks;
    this->data_offset = this->block_list[1].offset;
    this->min_data_value = fd.min_data_value;
    this->max_data_value = fd.max_data_value;
    
    this->site = fd.site;
    this->scanner_id = fd.scanner_id;
    this->scanner_type = fd.scanner_type;
    this->scandist_um = fd.scandist_um;
    this->slice_thickness_um = fd.slice_thickness_um;
    this->slice_increment_um = fd.slice_increment_um;
    this->slice_1_pos_um = fd.slice_1_pos_um;
    this->reference_line_um = fd.reference_line_um;
    this->nr_of_samples = fd.nr_of_samples;
    this->nr_of_projections = fd.nr_of_projections;
    this->sampletime_us = fd.sampletime_us;
    this->mu_scaling = fd.mu_scaling;
    this->recon_alg = fd.recon_alg; // Conebeam???";
    this->energy = fd.energy;
    this->intensity = fd.intensity;
    this->holder = fd.fill[78]; // the holder number is hidden in the fill field

    this->data_type     = fd.data_type; // ISQ files use integer 3
  }

  else {
    throw_aimio_exception ("Unrecognized ISQ data type.");
  }

  this->buffer_type = ISQFILE_TYPE_SHORT;
}

// ---------------------------------------------------------------------------
void IsqFile::ReadImageInfo ()
{
  // Open file.
  std::ifstream f (this->filename.c_str(), std::ios_base::in | std::ios_base::binary);
  if (!f) {
    throw_aimio_exception (std::string("Unable to open file ") + filename); }
  f.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

  this->ReadBlockList (f);
  this->ReadHeader (f);
}


// ---------------------------------------------------------------------------
void IsqFile::ReadAnyIsqData
  (
  void* data,
  int buffer_number,
  AimIO::aim_storage_format_t type
  )
{
  // Open file.
  std::ifstream f (this->filename.c_str(), std::ios_base::in | std::ios_base::binary);
  if (!f) {
    throw_aimio_exception (std::string("Unable to open file ") + filename); }
  f.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

  std::vector<char> buffer (this->block_list[buffer_number].size);
  f.seekg (this->block_list[buffer_number].offset);
  f.read (&(buffer[0]), this->block_list[buffer_number].size);

  AimIO::Decompress (data,
                     &(buffer[0]),
                     buffer.size(),
                     type,
                     this->dimensions_p,
                     this->offset,
                     (this->version == AIMFILE_VERSION_30)); // boolean is always false
                     
}

// ---------------------------------------------------------------------------
void IsqFile::ReadImageData (short* data, size_t size)
{
  aimio_assert (this->block_list.size() >= 2);
  aimio_assert (this->buffer_type == ISQFILE_TYPE_SHORT);
  aimio_assert (size == long_product(this->dimensions_p));
  this->ReadAnyIsqData (data, 1, AIMFILE_TYPE_D1Tshort);
}

}  // namespace
