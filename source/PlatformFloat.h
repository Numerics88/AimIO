// Copyright (c) Eric Nodwell
// See LICENSE for details.

#ifndef __AimIO_PlatformFloat_h
#define __AimIO_PlatformFloat_h

namespace AimIO
{

inline float vms_to_native (float x)
{
  float y;
  reinterpret_cast<char*>(&y)[0] = reinterpret_cast<char*>(&x)[2];
  reinterpret_cast<char*>(&y)[1] = reinterpret_cast<char*>(&x)[3];
  reinterpret_cast<char*>(&y)[2] = reinterpret_cast<char*>(&x)[0];
  reinterpret_cast<char*>(&y)[3] = reinterpret_cast<char*>(&x)[1];
  y /= 4.0;
  return y;
}

inline float native_to_vms (float x)
{
  x *= 4;
  float y;
  reinterpret_cast<char*>(&y)[0] = reinterpret_cast<char*>(&x)[2];
  reinterpret_cast<char*>(&y)[1] = reinterpret_cast<char*>(&x)[3];
  reinterpret_cast<char*>(&y)[2] = reinterpret_cast<char*>(&x)[0];
  reinterpret_cast<char*>(&y)[3] = reinterpret_cast<char*>(&x)[1];
  return y;
}

inline void vms_to_native_inplace (float& x)
{
  float y = x;
  reinterpret_cast<char*>(&x)[0] = reinterpret_cast<char*>(&y)[2];
  reinterpret_cast<char*>(&x)[1] = reinterpret_cast<char*>(&y)[3];
  reinterpret_cast<char*>(&x)[2] = reinterpret_cast<char*>(&y)[0];
  reinterpret_cast<char*>(&x)[3] = reinterpret_cast<char*>(&y)[1];
  x /= 4.0;
}

inline void native_to_vms_inplace (float& x)
{
  float y = 4.0f * x;
  reinterpret_cast<char*>(&x)[0] = reinterpret_cast<char*>(&y)[2];
  reinterpret_cast<char*>(&x)[1] = reinterpret_cast<char*>(&y)[3];
  reinterpret_cast<char*>(&x)[2] = reinterpret_cast<char*>(&y)[0];
  reinterpret_cast<char*>(&x)[3] = reinterpret_cast<char*>(&y)[1];
}

}  // namespace

#endif
