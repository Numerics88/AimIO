// Copyright (c) Eric Nodwell
// See LICENSE for details.

#ifndef __AimIO_Definitions_h
#define __AimIO_Definitions_h

// These are definitions related to AIM files that need to be
// be public.

namespace AimIO
{

enum aim_version_t {
  AIMFILE_VERSION_10,
  AIMFILE_VERSION_11,
  AIMFILE_VERSION_20,
  AIMFILE_VERSION_30};

// These are internal storage types for AIM files.
// More possible types exist than this, but this seems to be
// sufficient for practical cases.
enum aim_storage_format_t {
  AIMFILE_TYPE_D1Tundef    = 0,
  AIMFILE_TYPE_D1Tchar     = (1<<16) + sizeof(char),
  AIMFILE_TYPE_D1TbinCmp   = (21<<16) + sizeof(char),   // compression suitable for 2 values
  AIMFILE_TYPE_D3Tbit8     = (6<<16) + sizeof(unsigned char),   // compression suitable for 2 values
  AIMFILE_TYPE_D1TcharCmp  = (8<<16) + 2*sizeof(char),
  AIMFILE_TYPE_D1Tshort    = (2<<16) + sizeof(short),
  AIMFILE_TYPE_D1Tfloat    = (4<<16) + sizeof(float)
};

}  // namespace

#endif
