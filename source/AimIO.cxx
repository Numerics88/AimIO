// Copyright (c) Eric Nodwell
// See LICENSE for details.

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

const char*  version030_string = "AIMDATA_V030   ";  //  15 char plus \0

typedef union {
  char c;
  boost::int32_t pad;
} char_occupy_4bytes_t;

typedef struct
  {
  little_int32_t id;
  little_int32_t dat;
  little_int32_t nr;
  little_int32_t size;
  little_int32_t type;
} D1FileData020;

typedef struct
  {
  little_int32_t id;
  little_int32_t nr;
  little_int32_t size;
  little_int32_t type;
} D1FileData030;

typedef struct
  {
  little_int32_t           proc_log;
  little_int32_t           dat;
  little_int32_t           id;
  little_int32_t           ref;
  little_int32_t           type;
  tuplet<3,little_int32_t>  dim;
  tuplet<3,little_int32_t>  off;
  tuplet<3,little_int32_t>  subdim;
  tuplet<3,little_int32_t>  pos;
  float                    el_size_mm;
  D1FileData020            assoc;
} D3FileImage010;

typedef struct
  {
  little_int32_t           proc_log;
  little_int32_t           dat;
  little_int32_t           id;
  little_int32_t           ref;
  little_int32_t           type;
  tuplet<3,little_int32_t>  dim;
  tuplet<3,little_int32_t>  off;
  tuplet<3,little_int32_t>  subdim;
  tuplet<3,little_int32_t>  pos;
  float                    el_size_mm;
  D1FileData020            assoc;
  char_occupy_4bytes_t     version;
} D3FileImage011;

typedef struct
  {
  char_occupy_4bytes_t     version;
  little_int32_t           proc_log;
  little_int32_t           dat;
  little_int32_t           id;
  little_int32_t           ref;
  little_int32_t           type;
  tuplet<3,little_int32_t>  pos;
  tuplet<3,little_int32_t>  dim;
  tuplet<3,little_int32_t>  off;
  tuplet<3,little_int32_t>  supdim;
  tuplet<3,little_int32_t>  suppos;
  tuplet<3,little_int32_t>  subdim;
  tuplet<3,little_int32_t>  testoff;
  tuplet<3,float>           el_size_mm;
  D1FileData020            assoc;
} D3FileImage020;

typedef struct
  {
  char_occupy_4bytes_t     version;
  little_int32_t           id;
  little_int32_t           ref;
  little_int32_t           type;
  tuplet<3,little_int64_t>  pos;
  tuplet<3,little_int64_t>  dim;
  tuplet<3,little_int64_t>  off;
  tuplet<3,little_int64_t>  supdim;
  tuplet<3,little_int64_t>  suppos;
  tuplet<3,little_int64_t>  subdim;
  tuplet<3,little_int64_t>  testoff;
  tuplet<3,little_int64_t>  el_size_nano;
  D1FileData030            assoc;
} D3FileImage030;


// ===========================================================================
// Methods

// ---------------------------------------------------------------------------
AimFile::AimFile ()
  :
  version (AIMFILE_VERSION_30),
  id (0),
  reference (0),
  aim_type (AIMFILE_TYPE_D1Tundef),
  buffer_type (AIMFILE_TYPE_UNDEFINED),
  position (0,0,0),
  dimensions (0,0,0),
  offset (0,0,0),
  supdim (0,0,0),
  suppos (0,0,0),
  subdim (0,0,0),
  testoff (0,0,0),
  element_size (0,0,0),
  assoc_id (0),
  assoc_nr (0),
  assoc_size (0),
  assoc_type (1),
  byte_offset (0)
  {}


// ---------------------------------------------------------------------------
AimFile::AimFile (const char* fn)
  :
  filename (fn),
  version (AIMFILE_VERSION_30),
  id (0),
  reference (0),
  aim_type (AIMFILE_TYPE_D1Tundef),
  buffer_type (AIMFILE_TYPE_UNDEFINED),
  position (0,0,0),
  dimensions (0,0,0),
  offset (0,0,0),
  supdim (0,0,0),
  suppos (0,0,0),
  subdim (0,0,0),
  testoff (0,0,0),
  element_size (0,0,0),
  assoc_id (0),
  assoc_nr (0),
  assoc_size (0),
  assoc_type (1),
  byte_offset (0)
  {}


// ---------------------------------------------------------------------------
void AimFile::ReadBlockList (std::ifstream& f)
{

  // Read in pre-header
  std::vector<char> buffer (24);  // pre-header max size is 24 bytes
  f.read (&(buffer[0]), 24);

  // Check if the first 2bytes = version030_string
  // and initialises the 64 resp. 32 bit flag
  bool file_64bit_flag;
  size_t head_mb_size = 0;
  size_t memory_offset = 0;
  if (strncmp (&(buffer[0]), version030_string, 15) == 0) {
    file_64bit_flag = true;
    head_mb_size = little_to_native(*(reinterpret_cast<boost::int64_t*>(&(buffer[0]) + 16)));
    memory_offset = 16;
  }
  else {
    /*  Aims up to Version 020 have 20-byte or 16-byte 'pre'-header */
    file_64bit_flag = false;
    head_mb_size = little_to_native(*(reinterpret_cast<boost::int32_t*>(&(buffer[0]))));
    if(head_mb_size > 20) {
      throw_aimio_exception ("File neither 32bit version nor AIM_V030.");
    }
    memory_offset = 0;
  }
  memory_offset += head_mb_size;

  // Number of blocks
  int nr_mb;
  if (file_64bit_flag) {
    nr_mb = head_mb_size/sizeof(boost::int64_t) - 1;  // First entry was pre-header size.
  }
  else {
    nr_mb = head_mb_size/sizeof(boost::int32_t) - 1;
  }
  this->block_list.resize (nr_mb); 

  // Read memory block list
  if (file_64bit_flag) {
    buffer.resize(head_mb_size);
    f.seekg (16);
    f.read (&(buffer[0]), head_mb_size);
  }

  for (int i=0; i<nr_mb; ++i)
  {
    this->block_list[i].offset = memory_offset;
    if (file_64bit_flag) {
      this->block_list[i].size = little_to_native(
          *(reinterpret_cast<boost::int64_t*>(&(buffer[0]))+i+1));
    }
    else {
      this->block_list[i].size = little_to_native(
          *(reinterpret_cast<boost::int32_t*>(&(buffer[0]))+i+1));
    }
    memory_offset += this->block_list[i].size;
  }
}


// ---------------------------------------------------------------------------
AimFile::buffer_format_t AimFile::GetTransferBufferType 
  (aim_storage_format_t storage_type)
  {
  if (storage_type == AIMFILE_TYPE_D1Tchar ||
      storage_type == AIMFILE_TYPE_D1TbinCmp ||
      storage_type == AIMFILE_TYPE_D1TcharCmp ||
      storage_type == AIMFILE_TYPE_D3Tbit8)
    { return AIMFILE_TYPE_CHAR; }
  if (storage_type == AIMFILE_TYPE_D1Tshort)
    { return AIMFILE_TYPE_SHORT; }
  if (storage_type == AIMFILE_TYPE_D1Tfloat)
    { return AIMFILE_TYPE_FLOAT; }
  std::cout << "=========== storage_type " << storage_type << "\n";
  throw_aimio_exception ("Unsupported AIM data type.");
  return AIMFILE_TYPE_CHAR;  // avoid compiler warning
  }


// ---------------------------------------------------------------------------
void AimFile::ReadHeader (std::ifstream& f)
{
  aimio_assert (this->block_list.size());

  if (this->block_list[0].size == sizeof(D3FileImage030) )
  {
    D3FileImage030 fd;
    f.seekg (this->block_list[0].offset);
    f.read ((char*)&fd, sizeof(D3FileImage030));
    this->version      = AIMFILE_VERSION_30;
    this->id           = fd.id;
    this->reference    = fd.ref;
    this->aim_type     = (aim_storage_format_t)(1*fd.type);
    this->position     = fd.pos;
    this->dimensions   = fd.dim;
    this->offset       = fd.off;
    this->supdim       = fd.supdim;
    this->suppos       = fd.suppos;
    this->subdim       = fd.subdim;
    this->testoff      = fd.testoff;
    this->element_size = n88::tuplet<3,float>(fd.el_size_nano)/1.E6f;
    this->assoc_id     = fd.assoc.id;
    this->assoc_nr     = fd.assoc.nr;
    this->assoc_size   = fd.assoc.size;
    this->assoc_type   = fd.assoc.type;
    this->byte_offset  = this->block_list[2].offset;
  }

  else if (this->block_list[0].size == sizeof(D3FileImage020) )
  {
    D3FileImage020 fd;
    // D3FileImage020_no_endian fd;
    f.seekg (this->block_list[0].offset);
    f.read ((char*)&fd, sizeof(D3FileImage020));
    this->version      = AIMFILE_VERSION_20;
    this->id           = fd.id;
    this->reference    = fd.ref;
    this->aim_type     = (aim_storage_format_t)(1*fd.type);
    this->position     = fd.pos;
    this->dimensions   = fd.dim;
    this->offset       = fd.off;
    this->supdim       = fd.supdim;
    this->suppos       = fd.suppos;
    this->subdim       = fd.subdim;
    this->testoff      = fd.testoff;
    this->element_size = fd.el_size_mm;
    vms_to_native_inplace (this->element_size[0]);
    vms_to_native_inplace (this->element_size[1]);
    vms_to_native_inplace (this->element_size[2]);
    this->assoc_id     = fd.assoc.id;
    this->assoc_nr     = fd.assoc.nr;
    this->assoc_size   = fd.assoc.size;
    this->assoc_type   = fd.assoc.type;
    this->byte_offset  = this->block_list[2].offset;
  }

  else if (this->block_list[0].size == sizeof(D3FileImage011) )
  {
    D3FileImage011 fd;
    f.seekg (this->block_list[0].offset);
    f.read ((char*)&fd, sizeof(D3FileImage011));
    this->version      = AIMFILE_VERSION_11;
    this->id           = fd.id;
    this->reference    = fd.ref;
    this->aim_type     = (aim_storage_format_t)(1*fd.type);
    this->position     = fd.pos;
    this->dimensions   = fd.dim;
    this->offset       = fd.off;
    this->subdim       = fd.subdim;
    float es           = vms_to_native (fd.el_size_mm);
    this->element_size = n88::tuplet<3,float>(es,es,es);
    this->assoc_id     = fd.assoc.id;
    this->assoc_nr     = fd.assoc.nr;
    this->assoc_size   = fd.assoc.size;
    this->assoc_type   = fd.assoc.type;
    this->byte_offset  = this->block_list[2].offset;
  }

  else if (this->block_list[0].size == sizeof(D3FileImage010) )
  {
    D3FileImage010 fd;
    f.seekg (this->block_list[0].offset);
    f.read ((char*)&fd, sizeof(D3FileImage010));
    this->version      = AIMFILE_VERSION_10;
    this->id           = fd.id;
    this->reference    = fd.ref;
    this->aim_type     = (aim_storage_format_t)(1*fd.type);
    this->position     = fd.pos;
    this->dimensions   = fd.dim;
    this->offset       = fd.off;
    this->subdim       = fd.subdim;
    float es           = vms_to_native (fd.el_size_mm);
    this->element_size = n88::tuplet<3,float>(es,es,es);
    this->assoc_id     = fd.assoc.id;
    this->assoc_nr     = fd.assoc.nr;
    this->assoc_size   = fd.assoc.size;
    this->assoc_type   = fd.assoc.type;
    this->byte_offset  = this->block_list[2].offset;
    }

  else {
    throw_aimio_exception ("Unrecognized AIM data type.");
  }

  this->buffer_type = this->GetTransferBufferType (this->aim_type);
}


// ---------------------------------------------------------------------------
void AimFile::ReadProcessingLog (std::ifstream& f)
{
  aimio_assert (this->block_list.size() > 1);

  this->processing_log.clear();
  if (this->block_list[1].size < 2)
     { return; }

  std::vector<char> buffer (this->block_list[1].size+1);
  f.seekg (this->block_list[1].offset);
  f.read (&(buffer[0]), this->block_list[1].size);
  buffer[this->block_list[1].size] = 0;   // guarantee null termination.
  this->processing_log = &(buffer[0]);   // copy operation
}


// ---------------------------------------------------------------------------
void AimFile::ReadImageInfo ()
{
  // Open file.
  std::ifstream f (this->filename.c_str(), std::ios_base::in | std::ios_base::binary);
  if (!f) {
    throw_aimio_exception (std::string("Unable to open file ") + filename); }
  f.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

  this->ReadBlockList (f);
  this->ReadHeader (f);
  this->ReadProcessingLog(f);
}


// ---------------------------------------------------------------------------
void AimFile::ReadAnyData
  (
  void* data,
  int buffer_number,
  aim_storage_format_t type
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
                     this->dimensions,
                     this->offset,
                     (this->version == AIMFILE_VERSION_30));
}

// ---------------------------------------------------------------------------
void AimFile::ReadImageData (char* data, size_t size)
{
  aimio_assert (this->block_list.size() >= 3);
  aimio_assert (this->buffer_type == AIMFILE_TYPE_CHAR);
  aimio_assert (size == long_product(this->dimensions));
  this->ReadAnyData (data, 2, this->aim_type);
}

// ---------------------------------------------------------------------------
void AimFile::ReadImageData (short* data, size_t size)
{
  aimio_assert (this->block_list.size() >= 3);
  aimio_assert (this->buffer_type == AIMFILE_TYPE_SHORT);
  aimio_assert (size == long_product(this->dimensions));
  this->ReadAnyData (data, 2, this->aim_type);
}

// ---------------------------------------------------------------------------
void AimFile::ReadImageData (float* data, size_t size)
{
  aimio_assert (this->block_list.size() >= 3);
  aimio_assert (this->buffer_type == AIMFILE_TYPE_FLOAT);
  aimio_assert (size == long_product(this->dimensions));
  this->ReadAnyData (data, 2, this->aim_type);
}

// ---------------------------------------------------------------------------
void AimFile::FillHeader (std::vector<char>& header)
{
  header.clear();

  if (this->version == AIMFILE_VERSION_30)
  {
    D3FileImage030 fd;
    fd.version.pad     = 0;   // avoid padding byte with random mem.
    fd.version.c       = 030;
    fd.id              = this->id;
    fd.ref             = this->reference;
    fd.type            = int(this->aim_type);
    fd.pos             = this->position;
    fd.dim             = this->dimensions;
    fd.off             = this->offset;
    fd.supdim          = this->subdim;
    fd.suppos          = this->suppos;
    fd.subdim          = this->subdim;
    fd.testoff         = this->testoff;
    fd.el_size_nano    = this->element_size * 1.E6f;
    fd.assoc.id        = this->assoc_id;
    fd.assoc.nr        = this->assoc_nr;
    fd.assoc.size      = this->assoc_size;
    fd.assoc.type      = this->assoc_type;
    header.resize (sizeof(fd));
    memcpy (&(header[0]), &fd, sizeof(fd));
  }

  else if (this->version == AIMFILE_VERSION_20)
  {
    D3FileImage020 fd;
    fd.version.pad     = 0;   // avoid padding byte with random mem.
    fd.version.c       = 020;
    fd.proc_log        = 0;
    fd.dat             = 0;
    fd.id              = this->id;
    fd.ref             = this->reference;
    fd.type            = int(this->aim_type);
    fd.pos             = this->position;
    fd.dim             = this->dimensions;
    fd.off             = this->offset;
    fd.supdim          = this->subdim;
    fd.suppos          = this->suppos;
    fd.subdim          = this->subdim;
    fd.testoff         = this->testoff;
    fd.el_size_mm      = this->element_size;
    native_to_vms_inplace (fd.el_size_mm[0]);
    native_to_vms_inplace (fd.el_size_mm[1]);
    native_to_vms_inplace (fd.el_size_mm[2]);
    fd.assoc.id        = this->assoc_id;
    fd.assoc.dat       = 0;
    fd.assoc.nr        = this->assoc_nr;
    fd.assoc.size      = this->assoc_size;
    fd.assoc.type      = this->assoc_type;
    header.resize (sizeof(fd));
    memcpy (&(header[0]), &fd, sizeof(fd));
  }

  else if (this->version == AIMFILE_VERSION_11)
  {
    D3FileImage011 fd;
    fd.version.pad     = 0;   // avoid padding byte with random mem.
    fd.version.c       = 011;
    fd.proc_log        = 0;
    fd.dat             = 0;
    fd.id              = this->id;
    fd.ref             = this->reference;
    fd.type            = int(this->aim_type);
    fd.pos             = this->position;
    fd.dim             = this->dimensions;
    fd.off             = this->offset;
    fd.subdim          = this->subdim;
    fd.el_size_mm      = this->element_size[0];
    native_to_vms_inplace (fd.el_size_mm);
    fd.assoc.id        = this->assoc_id;
    fd.assoc.dat       = 0;
    fd.assoc.nr        = this->assoc_nr;
    fd.assoc.size      = this->assoc_size;
    fd.assoc.type      = this->assoc_type;
    header.resize (sizeof(fd));
    memcpy (&(header[0]), &fd, sizeof(fd));
  }

  else if (this->version == AIMFILE_VERSION_10)
  {
    D3FileImage010 fd;
    fd.id              = this->id;
    fd.proc_log        = 0;
    fd.dat             = 0;
    fd.ref             = this->reference;
    fd.type            = int(this->aim_type);
    fd.pos             = this->position;
    fd.dim             = this->dimensions;
    fd.off             = this->offset;
    fd.subdim          = this->subdim;
    fd.el_size_mm      = this->element_size[0];
    native_to_vms_inplace (fd.el_size_mm);
    fd.assoc.id        = this->assoc_id;
    fd.assoc.dat       = 0;
    fd.assoc.nr        = this->assoc_nr;
    fd.assoc.size      = this->assoc_size;
    fd.assoc.type      = this->assoc_type;
    header.resize (sizeof(fd));
    memcpy (&(header[0]), &fd, sizeof(fd));
  }

  else
  {
    throw_aimio_exception ("Unrecognized AIM data type.");
  }
}


// ---------------------------------------------------------------------------
void AimFile::WriteAnyData
  (
  const void* data
  )
{
  std::vector<char> header;
  this->FillHeader (header);

  // Have to compress the data before writing to see how large it will be.
  std::ostringstream os;
  Compress (os, data, this->aim_type, this->dimensions, (this->version == AIMFILE_VERSION_30));
  std::string compressed_data = os.str();

  // Construct block table
  this->block_list.clear();
  this->block_list.resize (4);  // zeroed on construction
  this->block_list[0].size = header.size();
  this->block_list[1].size = this->processing_log.size() + 1;
  this->block_list[2].size = compressed_data.size();
  if (this->version == AIMFILE_VERSION_30)
    { this->block_list[0].offset = 16; }
  for (int i=0; i<3; ++i)
    { this->block_list[i+1].offset = this->block_list[i].offset
                                   + this->block_list[i].size; }
  std::ofstream f (this->filename.c_str(), std::ios_base::out | std::ios_base::binary);
  if (!f) {
    throw_aimio_exception (std::string("Unable to open file ") + filename);
  }
  f.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

  // File identifier
  if (this->version == AIMFILE_VERSION_30) {
    f.write (version030_string, 16); }

  // Pre-header
  if (this->version == AIMFILE_VERSION_30)
  {
    boost::int64_t x = (this->block_list.size() + 1)  * sizeof(boost::int64_t);
    f.write (reinterpret_cast<char*>(&x), sizeof(boost::int64_t));
    for (int i=0; i<this->block_list.size(); ++i)
    {
      x = this->block_list[i].size;
      f.write (reinterpret_cast<char*>(&x), sizeof(boost::int64_t));
    }
  }
  else
  {
    boost::int32_t x = (this->block_list.size() + 1)  * sizeof(boost::int32_t);
    f.write (reinterpret_cast<char*>(&x), sizeof(boost::int32_t));
    for (int i=0; i<this->block_list.size(); ++i)
    {
      x = this->block_list[i].size;
      f.write (reinterpret_cast<char*>(&x), sizeof(boost::int32_t));
    }
  }

  f.write (&(header[0]), this->block_list[0].size);
  f.write (this->processing_log.c_str(), this->block_list[1].size);
  f.write (compressed_data.data(), this->block_list[2].size);
}


// ---------------------------------------------------------------------------
void AimFile::WriteImageData (const char* data)
{

  // Intelligent selection of compression scheme if not explicitly set.
  if (this->aim_type == AIMFILE_TYPE_D1Tundef)
  {
    if ((this->offset != tuplet<3,int>(0,0,0)))
    {
      // Compression is incompatible with non-zero offset.
      this->aim_type = AIMFILE_TYPE_D1Tchar;
    }
    else
    {
      // Count number of values
      size_t N = long_product(this->dimensions);
      char value1 = data[0];
      size_t i=1;
      while (data[i] == value1)
      {
        ++i;
        if (i==N) break;
      }
      if (i==N)
      {
        // There is only one value
        this->aim_type = AIMFILE_TYPE_D1TbinCmp;
      }
      else
      {
        char value2 = data[i];
        while ((data[i] == value1) || (data[i] == value2))
        {
          ++i;
          if (i==N) break;
        }
        if (i==N)
        {
          // There are two values
          this->aim_type = AIMFILE_TYPE_D1TbinCmp;
        }
        else
        {
          // There are more than two values
          this->aim_type = AIMFILE_TYPE_D1TcharCmp;
        }
      }
    }
  }

  // Verify that settings are consistent.
  n88_verbose_assert ((this->aim_type == AIMFILE_TYPE_D1Tchar ||
                       this->aim_type == AIMFILE_TYPE_D1TbinCmp ||
                       this->aim_type == AIMFILE_TYPE_D3Tbit8 ||
                       this->aim_type == AIMFILE_TYPE_D1TcharCmp),
    "Incompatible storage type for char.");

  if (this->aim_type == AIMFILE_TYPE_D3Tbit8 ||
      this->aim_type == AIMFILE_TYPE_D1TcharCmp ||
      this->aim_type == AIMFILE_TYPE_D1TbinCmp)
  {
    aimio_verbose_assert ((this->offset == tuplet<3,int>(0,0,0)),
      "Non-zero offset incompatible with compression.");
  }

  this->WriteAnyData (data);
}

// ---------------------------------------------------------------------------
void AimFile::WriteImageData (const short* data)
{

  if (this->aim_type == AIMFILE_TYPE_D1Tundef)
  {
    this->aim_type = AIMFILE_TYPE_D1Tshort;
  }

  n88_verbose_assert (this->aim_type == AIMFILE_TYPE_D1Tshort,
    "Incompatible storage type for short.");

  this->WriteAnyData (data);
}

// ---------------------------------------------------------------------------
void AimFile::WriteImageData (const float* data)
{

  if (this->aim_type == AIMFILE_TYPE_D1Tundef)
  {
    this->aim_type = AIMFILE_TYPE_D1Tfloat;
  }

  n88_verbose_assert (this->aim_type == AIMFILE_TYPE_D1Tfloat,
    "Incompatible storage type for float.");

  this->WriteAnyData (data);
}

}  // namespace
