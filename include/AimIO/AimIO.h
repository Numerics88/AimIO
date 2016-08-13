// Copyright (c) Eric Nodwell
// See LICENSE for details.

#ifndef __AimIO_AimIO_h
#define __AimIO_AimIO_h

#include "AimIO/Definitions.h"
#include "AimIO/Exception.h"
#include <n88util/Tuple.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <boost/cstdint.hpp>

#include "aimio_export.h"


namespace AimIO
{


// For internal use.
struct MemoryBlock
{
  size_t offset;  // offset in file of memory block in bytes
  size_t size;    // size of memory block in bytes
  MemoryBlock(): offset(0), size(0) {}  // Zero on construction
};
typedef std::vector<MemoryBlock> BlockList;


/** Class for reading and writing Scanco AIM files.
  *
  * Refer to the README.md file for limitations and examples.
  */
class AIMIO_EXPORT AimFile
{
  public:

    // Transfer data format
    enum buffer_format_t {
      AIMFILE_TYPE_UNDEFINED,
      AIMFILE_TYPE_CHAR,
      AIMFILE_TYPE_SHORT,
      AIMFILE_TYPE_FLOAT};

    /// Constructors.
    AimFile ();
    AimFile (const char* filename);

    /** Read the AIM file header.
      *
      * Before calling this, the public member variable filename must be set.
      *
      * This method sets a number of public member variables, which can be read
      * to obtain all the meta-data in the AIM file.
      */ 
    void ReadImageInfo ();

    /** Read the AIM image data.
      *
      * You must previously have called ReadImageInfo.
      *
      * In order to know which version of this overloaded method to call,
      * examine the value of the public member variable buffer_type. If you pass
      * a pointer of the wrong type, and exception will be thrown.
      *
      * The value of 'size' should be the product of the dimensions, and also the
      * size of the buffer you allocate. The former is of course already known
      * to the class when this method is called, so this is just a redundant safety
      * check to ensure that your buffer is adequately large.
      */
    void ReadImageData (char* data, size_t size);
    void ReadImageData (short* data, size_t size);
    void ReadImageData (float* data, size_t size);

    /** Write an AIM file.
      *
      * Before calling this, you must set any relevant public member variables.
      * At a minimum, you need to have set the following:
      *   - filename
      *   - dim
      *   - el_size_mm
      * Everything else can be left as default if you like.
      *
      * There is no need to set 'buffer_type', as that will be deduced from
      * the type of pointer that you pass to this overloaded function.
      * Similarly, do not set 'aim_type' unless you want a particular
      * of data compression scheme.
      *
      * A version 3 AIM file will be written unless you change the value of
      * 'version'.
      *
      * It is recommended not to write float data to an AIM file, since
      * there is currently no cross-platform support for floating point
      * numbers. Also, a number of tools that operate on AIM files
      * do not properly handle floating point data.
      */
    void WriteImageData (const char* data);
    void WriteImageData (const short* data);
    void WriteImageData (const float* data);

    std::string               filename;

    // The following are public variables that correspond to meta-data
    // in the AIM file. They will be set after a call to ReadImageData,
    // of alternatively you can set them before a call to WriteImageData.

    /// Version of AIM file.
    ///
    /// The type aim_version_t is defined in Definitions.h .
    ///
    /// For writes, the default is version 3.
    aim_version_t             version;

    /// Processing log.
    std::string               processing_log;

    boost::int32_t            id;

    boost::int32_t            reference;

    /// Type of storage in aim file, including the compression scheme.
    ///
    /// The type aim_storage_format_t is defined in Definitions.h .
    ///
    /// Generally, you can ignore this, and just be concerned with buffer_type.
    /// If you leave it to the default (AIMFILE_TYPE_D1Tundef) on write,
    /// then the most appropriate compression scheme will be selected. After a
    /// write you can examine this to see what AIM storage scheme was actually
    /// used, although I can't imagine why it would matter.
    aim_storage_format_t      aim_type;

    /// The type of data read/written by this class.
    ///
    /// It is crucial to read this value after a call to ReadImageInfo and
    /// before a call to ReadImageData, and to then allocate a buffer of
    /// the correct type. There is no automatic conversion between types,
    /// for example char to short.
    buffer_format_t           buffer_type;

    /// Position.
    ///
    /// This is the coordinates, in pixels, of the center of the voxel (0,0,0).
    n88::Tuple<3,int>         position;

    /// Dimensions of the image  as (x,y,z).
    ///
    /// x is the fastest-varying coordinate and z the slowest.
    ///
    /// This must be set before a call to WriteImageData.
    n88::Tuple<3,int>         dimensions;

    /// Offset
    ///
    /// This determines the width of a "frame" of invalid data around the
    /// edges of the image. It is typically set after the application of a
    /// digital filter to the image.
    ///
    /// Because of some historically inconsistent behaviour, AimIO will
    /// refuse to write a file with non-zero offset using a compressed
    /// format.
    n88::Tuple<3,int>         offset;

    // Obscure meta-data that is not often used:
    n88::Tuple<3,int>         supdim;
    n88::Tuple<3,int>         suppos;
    n88::Tuple<3,int>         subdim;
    n88::Tuple<3,int>         testoff;

    /// The element size in mm.
    ///
    /// This must be set before a call to WriteImageData.
    ///
    /// Note that an advantage of the version 3 AIM file format is that the element
    /// size is encoded using 64 bit integers, which is portable. Version 2
    /// and earlier versions of the AIM file format store this parameter as
    /// floating point values, which can be incorrectly read on different
    /// hardware.
    n88::Tuple<3,float>       element_size;

    /// Associated data is not currently handled by this class, because I haven't
    /// needed it. Support could of course be added to a future version, if needed.
    boost::int32_t            assoc_id;
    boost::int32_t            assoc_nr;
    boost::int32_t            assoc_size;
    boost::int32_t            assoc_type;

  protected:

    void ReadBlockList (std::ifstream& f);
    void ReadHeader (std::ifstream& f);
    void ReadProcessingLog (std::ifstream& f);
    buffer_format_t GetTransferBufferType (aim_storage_format_t storage_type);
    void ReadAnyData (void* data, int buffer_number, aim_storage_format_t type);
    void FillHeader (std::vector<char>& header);
    void WriteAnyData (const void* data);

    BlockList block_list;
};

}  // namespace

#endif
