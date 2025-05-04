# AimIO

AimIO is a simple C++ class for reading and writing Scanco AIM image files and reading Scanco ISQ image files (cannot write ISQ). 

[![Build Status](https://dev.azure.com/babesler/n88/_apis/build/status/Numerics88.AimIO?branchName=master)](https://dev.azure.com/babesler/n88/_build/latest?definitionId=8&branchName=master)
[![Anaconda-Server Badge](https://anaconda.org/numerics88/aimio/badges/installer/conda.svg)](https://anaconda.org/Numerics88/aimio)

## Compiling and linking

AimIO requires the following:

  * CMake: www.cmake.org
  * Boost: www.boost.org
  * n88util
  * Google test: https://github.com/google/googletest

To use AimIO, it is sufficient to just include the files AimIO.cxx, IsqIO.cxx
and Compression.cxx in your project. Compiling it as a library is possible too.

To build and run the tests with cmake, on linux or OS X, something like the
following sequence of commands is required:

```sh
mkdir build
cd build
ccmake ..
make
ctest -V
```

On Windows the procedure is a rather different: refer to CMake documentation.

For the tests, a compressed archive of test data files will automatically
be downloaded from numerics88.com . If you don't want this and you have access
to the archive aimio_testdata.tar.gz, just put that file in the data directory,
and no download will occur.


## Usage

### Reading an AIM file

Here is a simple example of reading an AIM file, where you know in advance that the
data will be type char.

```C++
// Create AimFile object.
AimIO::AimFile reader;

// Read header.
reader.filename = "myfile.aim";
reader.ReadImageInfo();

// Examine some header values.
std::cout << "The dimensions are " << reader.dimensions << "\n";
std::cout << "The position is " << reader.position << "\n";
std::cout << "The processing log is:\n" << reader.processing_log << "\n";

// Create a buffer for the image data.
assert (reader.buffer_type == AimIO::AimFile::AIMFILE_TYPE_CHAR);
size_t size = long_product (reader.dimensions);
std::vector<char> image_data (size);

// Read the image data.
reader.ReadImageData (image_data.data(), size);
```

For more details, refer to the header file AimIO.h .

For a complete working example, have a look at the test code in tests/AimIOTests.cxx .


### Writing an AIM file

Here is an example of writing an AIM file:

```C++
// Create some data. This is type char, but short and float are also possible
n88::tuplet<3,int> dim (25,25,25);
// std::vector is not required: any way you use to store image data is OK,
// so long as you can get a pointer to it.
std::vector<char> image_data (long_product(dim));

// Here you would actually set the data to something interesting!

// Create AimFile object.
AimIO::AimFile writer ("myfile.aim");

// Set some parameters.
writer.position = n88::tuplet<3,int>(195,212,50);
writer.dimensions = dim;
writer.element_size = n88::tuplet<3,float>(0.034,0.034,0.034);
writer.processing_log = "Processed by MI5. Approved by K. Philby";

// Write the file.
writer.WriteImageData (image_data.data());  // The argument is a pointer to const char (or short or float)
```

By default, a version 3 AIM file will be created. If you want a version 2 file,
add this line before writing the image:

```C++
writer.version = AimIO::AIMFILE_VERSION_20;
```

For more details, refer to the header file AimIO.h .

### Reading an ISQ file

Here is a simple example of reading an ISQ file. The data type is always short.

```C++
// Create IsqFile object.
AimIO::IsqFile reader;

// Read header.
reader.filename = "myfile.isq";
reader.ReadImageInfo();

// Examine some header values.
std::cout << "The dimensions are " << reader.dimensions_p_ << "\n";
std::cout << "The size in number of bytes is " << reader.nr_of_bytes << "\n";
std::cout << "The data offset is " << reader.data_offset_ << "\n";

// Create a buffer for the image data.
assert (reader.buffer_type == AimIO::IsqFile::ISQFILE_TYPE_SHORT);
size_t size = long_product (reader.dimensions_p_);
std::vector<short> image_data (size);

// Read the image data.
reader.ReadImageData (image_data.data(), size);
```

For more details, refer to the header file IsqIO.h .

For a complete working example, have a look at the test code in tests/AimIOTests.cxx .

## Limitations

* Endianess is handled automatically on all platforms (via boost::endian). However,
  this only applies to integers. Currently floating point values are always
  translated to the IEEE binary format used on x86 processors, regardless of platform.
  This is obviously not
  portable. This may not be a serious limitation in your case. Version 3 AIM
  files do not use floating point values in the header at all, and previous
  versions use floating point values in the header only for the element size.
  And although it is possible for the image data itself to be floating point,
  this is rare, possibly even exceedingly rare. In principle it would be
  possible to handle floats on different hardware simply by adding a bunch of
  conditional compiler defines in the file PlatformFloat.h . No other changes to
  the code would be necessary.

* "Associated data" is neither read nor written. A future version could support this,
  if there were demand for such a feature.

* Because of a historical bug with non-zero offset and some compression schemes,
  AimIO will not use compression when writing a file with non-zero
  offset. If you explicitly choose an aim type with compression in this case,
  an exception will be thrown.

## Authors and Contributors

AimIO was developed by Eric Nodwell (eric.nodwell@numerics88.com) at Numerics88
Solutions Ltd.


## Licence

AimIO is licensed under a MIT-style open source license. See the file LICENSE.
