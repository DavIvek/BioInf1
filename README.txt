# LDCF implementation

This is an implementation of the Logarithmic Dynamic Cuckoo Filter in C++.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

- Git
- CMake
- Make
- A C++ compiler (like g++)

### Cloning

This project uses Google Test as a submodule. Therefore, you should clone the repository with the --recursive flag to ensure the submodule is also downloaded:

bash
git clone --recursive https://github.com/yourusername/yourrepository.git


### Building
To build the project, follow these steps:

1. Navigate to the project directory

2. Create a build directory:
    bash
    mkdir build
    

3. Navigate to the build directory:
    bash
    cd build
    

4. Generate the build files using CMake:
    bash
    cmake ..
    

5. Build the project using Make:
    bash
    make
    

Now you have successfully built the project!

### Run tests using ctest
bash
    ctest


### Running the Example Benchmarks
To run the example benchmarks, follow these steps:

1. Navigate to the project directory.
2. Create a build directory (if you haven't already):
    bash
    mkdir build
    
3. Navigate to the build directory:
    bash
    cd build
    
4. Generate the build files using CMake (if you haven't already):
    bash
    cmake ..
    
5. Build the project using Make (if you haven't already):
    bash
    make
    
6. Run the example benchmarks with the desired arguments:
    bash
    ./benchLDCF <string_length> <false_positive_rate> <expected_levels>
    
   Replace <string_length>, <false_positive_rate>, and <expected_levels> with the desired values.

The benchLDCF program reads from files that contain ecoli genomes and performs various operations on the Logarithmic Dynamic Cuckoo Filter. It calculates insertion time, membership test time, and false positive rate. It uses a substring length to determine which sublength of substrings to look for. It also creates false positive examples to test the effectiveness of the filter. The results are written to the result.txt file.

Make sure you have the necessary input files in the appropriate location before running the benchmarks (in default implementation they are in benchmarks folder).
