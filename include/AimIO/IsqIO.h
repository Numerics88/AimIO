// Copyright (c) Eric Nodwell
// See LICENSE for details.

#ifndef __AimIO_IsqIO_h
#define __AimIO_IsqIO_h

#include "AimIO/Definitions.h"
#include "AimIO/Exception.h"
#include "AimIO/AimIO.h"
#include <n88util/tuplet.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <boost/cstdint.hpp>

#include "aimio_export.h"


namespace AimIO
{

/** Class for reading and writing Scanco ISQ files.
  *
  * Refer to the README.md file for limitations and examples.
  */
class AIMIO_EXPORT IsqFile
{
  public:

    // Transfer data format
    enum buffer_format_t {
      ISQFILE_TYPE_SHORT};

    // Constructors.
    IsqFile ();
    IsqFile (const char* filename);

    /** Read the ISQ file header.
      *
      * Before calling this, the public member variable filename must be set.
      *
      * This method sets a number of public member variables, which can be read
      * to obtain all the meta-data in the ISQ file.
      */ 
    void ReadIsqImageInfo ();

    /** Read the ISQ image data.
      *
      * You must previously have called ReadImageInfo.
      *
      */
    void ReadIsqImageData (short* data, size_t size);

    std::string               filename;

    // The following are public variables that correspond to meta-data
    // in the ISQ file. They will be set after a call to ReadImageData,
    // of alternatively you can set them before a call to WriteImageData.

    // Version of ISQ file.
    //
    // The type aim_version_t is defined in Definitions.h .
    AimIO::aim_version_t      version;

    boost::int32_t            data_type; // file type: (not to be confused with char, short, etc)
                                         //   ISQ = 3
                                         //   GOBJ = 6
                                         //   RSQ = 1
                                         //   RAD = 9
                                         //   PSQ = ?
                                         //   MSQ = ?
    
    boost::int32_t            nr_of_bytes;
    boost::int32_t            nr_of_blocks;
    boost::int32_t            patient_index;
    boost::int32_t            scanner_id;
    
    boost::int64_t            creation_date;

    // Dimensions of the image as (x,y,z) in pixels and micrometers.
    //
    // x is the fastest-varying coordinate and z the slowest.
    n88::tuplet<3,int>        dimensions_p;
    n88::tuplet<3,int>        dimensions_um;

    n88::tuplet<3,int>        offset; // always zero for ISQ files

    boost::int32_t            slice_thickness_um;
    boost::int32_t            slice_increment_um;
    boost::int32_t            slice_1_pos_um;
    boost::int32_t            min_data_value;
    boost::int32_t            max_data_value;
    boost::int32_t            mu_scaling;
    boost::int32_t            nr_of_samples;
    boost::int32_t            nr_of_projections;
    boost::int32_t            scandist_um;
    boost::int32_t            scanner_type;
    boost::int32_t            sampletime_us;
    boost::int32_t            index_measurement;
    boost::int32_t            site;
    boost::int32_t            reference_line_um;
    boost::int32_t            recon_alg;

    std::string               name;
    
    boost::int32_t            energy;
    boost::int32_t            intensity;
    boost::int32_t            holder;

    // The data offset for the start of image data.
    boost::int32_t            data_offset;

    // The type of data read/written by this class.
    //
    // ISQ files are always 2 byte unsigned integers
    buffer_format_t           buffer_type;

  protected:

    void ReadIsqBlockList (std::ifstream& f);
    void ReadIsqHeader (std::ifstream& f);
    void ReadAnyIsqData (void* data, int buffer_number, AimIO::aim_storage_format_t type);

    BlockList block_list;
};

}  // namespace

#endif
