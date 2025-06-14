#!/bin/bash

# Configuration
TAG="ana478_nocdbtag_v002"
RUNNUMBER=53513
SEGMENT_START=10
SEGMENT_END=100

# Compile-time C++ source file that includes the function
CPP_FILE="run_make_filelist_dah.C"
EXECUTABLE="run_make_filelist_dah"

# Generate temporary C++ file to call the function
cat << EOF > $CPP_FILE
#include <vector>
#include <string>
#include <iostream>

// Declare your function here or include the header that defines it
std::vector<std::string> make_filelist_dah(const std::string& tag, const int runnumber, const int segment, const std::string& output_filename = "");

// Provide a main function for each segment call
int main(int argc, char** argv) {
    std::string tag = argv[1];
    int runnumber = std::stoi(argv[2]);
    int segment = std::stoi(argv[3]);

    std::vector<std::string> result = make_filelist_dah(tag, runnumber, segment);
    return 0;
}
EOF

# Run for segment range
for (( SEG=$SEGMENT_START; SEG<=$SEGMENT_END; SEG++ ))
do
  echo "Running for segment $SEG"
  root.exe -l -b << EOF
         .x make_filelist_dah.C("$TAG",$RUNNUMBER,$SEG)
EOF
done

# Clean up
rm -f $CPP_FILE $EXECUTABLE

