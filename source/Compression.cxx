// Copyright (c) Eric Nodwell
// See LICENSE for details.

#include "Compression.h"
#include "PlatformFloat.h"
#include <boost/cstdint.hpp>
#include <boost/endian/conversion.hpp>
#include <cstring>

using namespace boost::endian;

using n88::Tuple;


namespace AimIO
{

struct D1charCmp_t {
  char          value;
  unsigned char length;
  D1charCmp_t (char v, unsigned char l): value(v), length(l) {}
};


void Decompress
  (
  void* void_out,
  const void* void_in,
  size_t compressed_size,
  aim_storage_format_t type,
  Tuple<3,int> dim,
  Tuple<3,int> off,
  bool encode_64bit
  )
{
  if (type == AIMFILE_TYPE_D1TcharCmp ||
      type == AIMFILE_TYPE_D1TbinCmp)
  {
    Tuple<3,int> dim_no_off = dim - off*2;
    std::vector<char> temp (long_product (dim_no_off));
    DecompressNoOffset (
      &(temp[0]),
      void_in,
      compressed_size,
      type,
      dim_no_off,
      encode_64bit);
    RestoreOffset (
      reinterpret_cast<char*>(void_out),
      &(temp[0]),
      dim,
      off);
  }
  else
  {
    DecompressNoOffset (
      void_out,
      void_in,
      compressed_size,
      type,
      dim,
      encode_64bit);
  }
}


void DecompressNoOffset
  (
  void* void_out,
  const void* void_in,
  size_t compressed_size,
  aim_storage_format_t type,
  Tuple<3,int> dim,
  bool encode_64bit
  )
{

  if (type == AIMFILE_TYPE_D3Tbit8)
  {
    const unsigned char* compressed = reinterpret_cast<const unsigned char*>(void_in);
    const unsigned char* compressed_end = compressed + compressed_size;
    char* raw = reinterpret_cast<char*>(void_out);

    Tuple<3,int> c_dim = (dim + 1)/2;
    n88_assert (compressed_size == long_product(c_dim)+1);

    char value = compressed[long_product(c_dim)];

    for (size_t k_r=0; k_r<dim[2]; ++k_r)
    {
      size_t k_c = k_r/2;
      for (size_t j_r=0; j_r<dim[1]; ++j_r)
          {
        size_t j_c = j_r/2;
        for (size_t i_r=0; i_r<dim[0]; ++i_r)
        {
          size_t i_c = i_r/2;
          const unsigned char* c = compressed + i_c + c_dim[0]*(j_c + c_dim[1]*k_c);
          int bit_pos = (k_r%2)*4 + (j_r%2)*2 + (i_r%2);
          *raw = (*c & (1<<bit_pos)) != 0 ? value : 0 ;
          ++raw;
        }
      }
    }
  }

  else if (type == AIMFILE_TYPE_D1TcharCmp)
  {
    const D1charCmp_t* compressed = reinterpret_cast<const D1charCmp_t*>(void_in);
    const D1charCmp_t* compressed_end = reinterpret_cast<const D1charCmp_t*>(
      reinterpret_cast<const char*>(compressed) + compressed_size);
    compressed += encode_64bit ? 4 : 2;

    unsigned char current_length = 0;
    char current_value = 0;

    char* raw = reinterpret_cast<char*>(void_out);
    char* raw_end = raw + long_product (dim);
    while (raw != raw_end)
    {
      if (current_length == 0)
      {
        n88_assert (compressed < compressed_end);
        current_length = compressed->length;
        current_value = compressed->value;
        ++compressed;
      }
      n88_assert (current_length);
      *raw = current_value;
      --current_length;
      ++raw;
    }
    n88_assert (compressed == compressed_end);
  }

  else if (type == AIMFILE_TYPE_D1TbinCmp)
  {
    const unsigned char* compressed = reinterpret_cast<const unsigned char*>(void_in);
    const unsigned char* compressed_end = compressed + compressed_size;

    char value_1;
    char value_2;
    if (encode_64bit)
    {
      value_1 = reinterpret_cast<const char*>(void_in)[8];
      value_2 = reinterpret_cast<const char*>(void_in)[9];
      compressed += 10;
    }
    else
    {
      value_1 = reinterpret_cast<const char*>(void_in)[4];
      value_2 = reinterpret_cast<const char*>(void_in)[5];
      compressed += 6;
    }

    unsigned char current_length = 0;
    bool change_value = false;

    char current_value = value_1;
    bool is_value_1 = true;

    char* raw = reinterpret_cast<char*>(void_out);
    char* raw_end = raw + long_product (dim);
    while (raw != raw_end)
    {
      if (current_length == 0)
      {
        if (change_value)
        {
          is_value_1 = !is_value_1;
          current_value = is_value_1 ? value_1 : value_2;
        }
        n88_assert (compressed < compressed_end);
        current_length = *compressed;
        if (current_length == 255)
        {
          current_length = 254;
          change_value = false;
        }
        else
        {
          change_value = true;
        }
        ++compressed;
      }
      n88_assert (current_length);
      *raw = current_value;
      --current_length;
      ++raw;
    }
    n88_assert (compressed == compressed_end);
  }

  else if (type == AIMFILE_TYPE_D1Tchar)
  {
    memcpy (void_out, void_in, long_product(dim) * sizeof(char));
  }

  else if (type == AIMFILE_TYPE_D1Tshort)
  {
    const short* in = reinterpret_cast<const short*>(void_in);
    short* out = reinterpret_cast<short*>(void_out);
    size_t N = long_product(dim);
    for (size_t i=0; i<N; ++i)
    {
      out[i] = little_to_native (in[i]);
    }
  }

  else if (type == AIMFILE_TYPE_D1Tfloat)
  {
    const float* in = reinterpret_cast<const float*>(void_in);
    float* out = reinterpret_cast<float*>(void_out);
    size_t N = long_product(dim);
    for (size_t i=0; i<N; ++i)
    {
      out[i] = vms_to_native (in[i]);
    }
  }

  else
  {
    throw_aimio_exception ("Unrecognized AIM data type.");
  }
}


void RestoreOffset
  (
  char* out,
  const char* in,
  Tuple<3,int> dim,
  Tuple<3,int> off
  )
{
  // Zero output
  size_t N = long_product (dim);
  for (size_t i=0; i<N; ++i)
    { out[i] = 0; }

  for (size_t k=off[2]; k<dim[2]-off[2]; ++k)
    for (size_t j=off[1]; j<dim[1]-off[1]; ++j)
      for (size_t i=off[0]; i<dim[0]-off[0]; ++i)
      {
        out[(k*dim[1]+j)*dim[0]+i] = *in;
        ++in;
      }
}


void Compress
  (
  std::ostream& out,
  const void* void_in,
  aim_storage_format_t type,
  Tuple<3,int> dim,
  bool encode_64bit
  )
{

  if (type == AIMFILE_TYPE_D3Tbit8)
  {
    Tuple<3,int> c_dim = (dim + 1)/2;

    const char* raw  = reinterpret_cast<const char*>(void_in);
    std::vector<unsigned char> buffer (long_product(c_dim)+1);
    for (size_t i=0; i<buffer.size(); ++i)
      { buffer[i] = 0; }
    unsigned char* compressed = &(buffer[0]);

    char value = 0;
    for (size_t k_r=0; k_r<dim[2]; ++k_r)
    {
      size_t k_c = k_r/2;
      for (size_t j_r=0; j_r<dim[1]; ++j_r)
      {
        size_t j_c = j_r/2;
        for (size_t i_r=0; i_r<dim[0]; ++i_r)
        {
          size_t i_c = i_r/2;
          if (*raw)
          {
            unsigned char* c = compressed + i_c + c_dim[0]*(j_c + c_dim[1]*k_c);
            int bit_pos = (k_r%2)*4 + (j_r%2)*2 + (i_r%2);
            *c += 1<<bit_pos;
            value = *raw;
          }
          ++raw;
        }
      }
    }
    compressed[long_product(c_dim)] = value;
    out.write (reinterpret_cast<char*>(&(buffer[0])), buffer.size());
  }

  else if (type == AIMFILE_TYPE_D1TcharCmp)
  {

    char current_value = 0;
    unsigned char current_length = 0;
    size_t count = 1;

    const char* raw = reinterpret_cast<const char*>(void_in);
    const char* raw_end = raw + long_product (dim);
    while (raw != raw_end)
    {
      if (*raw == current_value)
      {
        if (current_length == 255)
        {
          ++count;
          current_length = 1;
        }
        else
        {
          ++current_length;
        }
      }
      else
      {
        current_value = *raw;
        current_length = 1;
        ++count;
      }
      ++raw;
    }

    size_t mem_size = 0;
    if (encode_64bit)
    {
      mem_size = count*2 + 8;
    }
    else
    {
      mem_size = count*2 + 4;
      if (mem_size >= (1<<31))
      {
        throw_aimio_exception ("Data size exceeds version 2 limit.");
      }
    }

    if (encode_64bit)
    {
      size_t ms = mem_size;
      native_to_little_inplace(ms);
      out.write (reinterpret_cast<char*>(&ms), sizeof(size_t));
    }
    else
    {
	    boost::int32_t ms = mem_size;
      native_to_little_inplace(ms);
      out.write (reinterpret_cast<char*>(&ms), sizeof(boost::int32_t));
    }

    D1charCmp_t current_field (0,0);
    raw = reinterpret_cast<const char*>(void_in);
    while (raw != raw_end)
    {
      if (*raw == current_field.value)
      {
        if (current_field.length == 255)
        {
          out.write (reinterpret_cast<char*>(&current_field), sizeof(D1charCmp_t));
          current_field.length = 1;
        }
        else
        {
          ++(current_field.length);
        }
      }
      else
      {
        out.write (reinterpret_cast<char*>(&current_field), sizeof(D1charCmp_t));
        current_field.value = *raw;
        current_field.length = 1;
      }
      ++raw;
    }

    out.write (reinterpret_cast<char*>(&current_field), sizeof(D1charCmp_t));
  }

  else if (type == AIMFILE_TYPE_D1TbinCmp)
  {
    // Determine the two values.
    const char* raw  = reinterpret_cast<const char*>(void_in);
    char value_1 = raw[0];
    bool value_2_found = false;
    char value_2 = 0;

    const char* raw_end = raw + long_product (dim);
    while (raw != raw_end)
    {
      if (*raw != value_1)
      {
        if (!value_2_found)
        {
          value_2 = *raw;
          value_2_found = true;
        }
        else if (*raw != value_2)
        {
          throw_aimio_exception ("D1TbinCmp compression only supports 2 values. 3 or more values in image.");
        }
      }
      ++raw;
    }

    //  Count fields

    char current_value = value_1;
    unsigned char current_length = 0;
    size_t count = 1;
    raw = reinterpret_cast<const char*>(void_in);
    while (raw != raw_end)
    {
      if (*raw == current_value)
      {
        if (current_length == 254)
        {
          ++count;
          current_length = 1; 
        }
        else
        {
          ++current_length;
        }
      }
      else
      {
        current_value = *raw;
        current_length = 1;
        ++count;
      }
      ++raw;
    }

    size_t mem_size = 0;
    if (encode_64bit)
    {
      mem_size = count + 8 + 2;
    }
    else
    {
      mem_size = count + 4 + 2;
      if (mem_size >= (1<<31))
      {
        throw_aimio_exception ("Data size exceeds version 2 limit.");
      }
    }
    
    if (encode_64bit)
    {
      boost::int64_t ms = mem_size;
      native_to_little_inplace(ms);
      out.write (reinterpret_cast<char*>(&ms), sizeof(boost::int64_t));
      out.write (&value_1, sizeof(char));
      out.write (&value_2, sizeof(char));
    }
    else
    {
      boost::int32_t ms = mem_size;
      native_to_little_inplace(ms);
      out.write (reinterpret_cast<char*>(&ms), sizeof(boost::int32_t));
      out.write (&value_1, sizeof(char));
      out.write (&value_2, sizeof(char));
    }
    current_value = value_1;
    current_length = 0;

    raw = reinterpret_cast<const char*>(void_in);
    while (raw != raw_end)
    {
      if (*raw == current_value)
      {
        if (current_length == 254)
        {              
          unsigned char x = 255; /* 255 means 254 and don't change value */
          out.write (reinterpret_cast<char*>(&x), sizeof(unsigned char));
          current_length = 1;
        }
        else
        {
          ++current_length;
        }
      }
      else
      {
        out.write (reinterpret_cast<char*>(&current_length), sizeof(unsigned char));
        current_value = *raw;
        current_length = 1;
      }
      ++raw;
    }

    out.write (reinterpret_cast<char*>(&current_length), sizeof(unsigned char));
  }

  else if (type == AIMFILE_TYPE_D1Tchar)
  {
    out.write (reinterpret_cast<const char*>(void_in),
               long_product(dim) * sizeof(char));
  }

  else if (type == AIMFILE_TYPE_D1Tshort)
  {
    const short* in = reinterpret_cast<const short*>(void_in);
    size_t N = long_product(dim);
    for (size_t i=0; i<N; ++i)
    {
      short x = native_to_little(in[i]);
      out.write (reinterpret_cast<char*>(&x), sizeof(short));
    }
  }

  else if (type == AIMFILE_TYPE_D1Tfloat)
  {
    const float* in = reinterpret_cast<const float*>(void_in);
    size_t N = long_product(dim);
    for (size_t i=0; i<N; ++i)
    {
      float x = native_to_vms(in[i]);
      out.write (reinterpret_cast<char*>(&x), sizeof(float));
    }
  }

  else
  {
    throw_aimio_exception ("Unrecognized AIM data type.");
  }

}

}  // namespace
