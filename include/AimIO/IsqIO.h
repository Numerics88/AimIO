// Copyright (c) Steven Boyd
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
    void ReadImageInfo ();

    /** Read the ISQ image data.
      *
      * You must previously have called ReadImageInfo.
      *
      */
    void ReadImageData (short* data, size_t size);

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

    void ReadBlockList (std::ifstream& f);
    void ReadHeader (std::ifstream& f);
    void ReadAnyIsqData (void* data, int buffer_number, AimIO::aim_storage_format_t type);

    BlockList block_list;
};

}  // namespace

#endif


// Copied from:
// https://www.scanco.ch/faq-customers.php
//
// ISQ Header format ISQ files consist of a standard 512-byte header, optional extended
// header blocks and the data part. Because the size of the extended header can vary,
// one has to read the data-offset (last 4 bytes of the 512-byte header) to be able to
// get to the start of the data (image) part. The offset is in 512-byte blocks, meaning
// if you encounter a value of 6, the data starts at byte 3584 (512 for standard
// header + 6x512 for the extended header). Here is the header structure in c code:
//
// typedef struct {
// /*---------------------------------------------*/
// char check[16];
// int data_type;
// int nr_of_bytes; /* either one of them */
// int nr_of_blocks; /* or both, but min. of 1 */
// int patient_index; /* 1 block = 512 bytes */
// int scanner_id;
// int creation_date[2];
// /*---------------------------------------------*/
// int dimx_p;
// int dimy_p;
// int dimz_p;
// int dimx_um;
// int dimy_um;
// int dimz_um;
// int slice_thickness_um;
// int slice_increment_um;
// int slice_1_pos_um;
// int min_data_value;
// int max_data_value;
// int mu_scaling; /* p(x,y,z)/mu_scaling = value [1/cm] */
// int nr_of_samples;
// int nr_of_projections;
// int scandist_um;
// int scanner_type;
// int sampletime_us;
// int index_measurement;
// int site; /* Coded value */
// int reference_line_um;
// int recon_alg; /* Coded value */
// char name[40];
// int energy; /*V */
// int intensity; /* uA */
// int fill[83];
// /*---------------------------------------------*/
// int data_offset; /* in 512-byte-blocks */
// } ima_data_type, *ima_data_typeP;
//
// The first 16 bytes are a string 'CTDATA-HEADER_V1', used to identify the type of data.
//
// The 'int' are all 4-byte integers.
//
// dimx_p is the dimension in pixels, dimx_um the dimension in microns.
//
// So dimx_p is at byte-offset 44, then dimy_p at 48, dimz_p (=number of slices) at 52.
//
// The microCT calculates so called 'x-ray linear attenuation' values. These (float) values
// are scaled with 'mu_scaling' (see header, e.g. 4096) to get to the signed 2-byte integers
// values that we save in the .isq file. e.g. Pixel value 8192 corresponds to lin. att. coeff.
// of 2.0 [1/cm] (8192/4096)
//
// Following to the headers is the data part. It is in 2-byte short integers (signed) and
// starts from the top-left pixel of slice 1 to the left, then the next line follows, until
// the last pixel of the last sclice in the lower right.
//
// To check the header on the VMS machine (byte by byte):
//
// $ dump/dec c0001235.ISQ/block=count=1
//
// This shows the bytes on the left, and interpretation as ASCII on the right, the column on
// the far right shows you byte number of right-most number per line: The bytes are displayed
// from RIGHT TO LEFT! (--> data_offset, bytes 508-511, is number in lower left (!) corner
