#include "CiftiFile.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace cifti;

/**\file datatype.cxx
This program reads a Cifti file from argv[1], and writes it out to argv[2] using 8-bit unsigned integer and data scaling.
It uses a single CiftiFile object to do this, for simplicity - to see how to do something similar with two objects,
which is more relevant for how you would do processing on cifti files, see rewrite.cxx.

\include datatype.cxx
*/

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "usage: " << argv[0] << " <input cifti> <output cifti>" << endl;
        cout << "  rewrite the input cifti file to the output filename, using uint8 and data scaling, little-endian." << endl;
        return 1;
    }
    try
    {
        CiftiFile inputFile(argv[1]);//on-disk reading by default
        inputFile.setWritingDataTypeAndScaling(NIFTI_TYPE_UINT8, -1.0, 6.0);//tells it to use this datatype to best represent this specified range of values [-1.0, 6.0] whenever this instance is written
        inputFile.writeFile(argv[2], CiftiVersion(), CiftiFile::LITTLE);//if this is the same filename as the input, CiftiFile actually detects this and reads the input into memory first
        //otherwise, it will read and write one row at a time, using very little memory
        //inputFile.setWritingDataTypeNoScaling(NIFTI_TYPE_FLOAT32);//this is how you would revert back to writing as float32 without rescaling
    } catch (CiftiException& e) {
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 1;
    }
    return 0;
}
