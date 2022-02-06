# flbwt [![build](https://github.com/mevid93/flbwt/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/mevid93/flbwt/actions/workflows/build.yml) [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
Fast Lightweight Burrows-Wheeler Transform

## Motivation
In 2009, Daisuke Okanohara and Kunihiko Sadakane presented a linear time BWT algorithm using induced sorting. One implementation of this algorithm existed, but it had a input file limitation of 4GB. This project was created to bring support for larger input files (up to 2^58 bits).

## Features
* Space efficient Burrows-Wheeler Transform

## Code Example
```cpp

#include <iostream>
#include "flbwt.hpp"


int main(int argc, char *argv[]) 
{
    /* validate arguments */
    if (argc != 3) {
        std::cout << "Wrong number of arguments!" << std::endl;
        std::cout << "How to use:" << std::endl;
        std::cout << "    <program_executable> <input_file> <output_file>" << std::endl;
        return -1;
    }

    /* call the BWT-function for the input file (argv[1]) and write result to output file (argv[2]) */
    flbwt::bwt_file(argv[1], argv[2]);

    return 0;
}

```

## How to build?
1. Clone the project repository. 
```console
git clone --recursive git@github.com:mevid93/flbwt.git
```
2. Navigate to the project root folder and create new folder called build. Navigate inside the build folder.
```console
cd ../../flbwt
mkdir build
cd build
```
3. Use the following commands to build the project. In case you do not wish to build tests, you can leave out the "-DBUILD_TESTS=ON" option.
```console
cmake -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release ..
make
```


## How to run tests?
Navigate inside the test folder (inside the build folder).
```console
cd ../flbwt/build/test
```
To run all unit tests, use the following command inside the tests folder.
```console
./UnitTests
```
To run single unit test, use the following command inside the tests folder.
```console
./UnitTests --gtest_filter=<test_name>
```

## Credits
* Kunihiko Sadakane
* Yuta Mori
* José Salavert Torres (https://github.com/josator/gnu-bwt-aligner)
