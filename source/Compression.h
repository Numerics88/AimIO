#ifndef __AimCompression_h
#define __AimCompression_h

#include "n88util/Tuple.hpp"
#include "AimIO/Definitions.h"
#include "AimIO/Exception.h"
#include <ostream>


namespace AimIO
{

/// As well as decompressing, handles endianness of data if required.
void Decompress (
    void* out,
    const void* in,
    size_t mem_size,
    aim_storage_format_t type,
    n88::Tuple<3,int> dim,
    n88::Tuple<3,int> off,
    bool encode_64bit);

/// As well as compressing, handles endianness of data if required.
void Compress (
    std::ostream& out,
    const void* in,
    aim_storage_format_t type,
    n88::Tuple<3,int> dim,
    bool encode_64bit);

/// Decompresses without taking offset into account.
///
/// For compresssed data, this should be called with dimensions equal to
/// dim-2*off, and then afterwards call RestoreOffset.
void DecompressNoOffset (
    void* out,
    const void* in,
    size_t mem_size,
    aim_storage_format_t type,
    n88::Tuple<3,int> dim,
    bool encode_64bit);

/// Takes input data of dimensions dim-2*off, and expands it to dimensions
/// of dim, padded the offset region.
void RestoreOffset (
    char* out,
    const char* in,
    n88::Tuple<3,int> dim,
    n88::Tuple<3,int> off);

}  // namespace

#endif
