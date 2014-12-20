#include "CiftiException.h"
#include "CiftiFile.h"
#include "MultiDimIterator.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace cifti;

//this program reads a Cifti file from argv[1], and writes it out to argv[2] with a second CiftiFile object
//it uses on-disk reading and writing, so DO NOT have both filenames point to the same file!
//CiftiFile truncates without any warning when told to write to an existing file

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "this program requires two arguments: an input cifti file, and an output filename to write it to" << endl;
        return 1;
    }
    try
    {
        CiftiFile inputFile(argv[1]);//on-disk reading by default
        //inputFile.convertToInMemory();//if you want to read it into memory first
        CiftiFile outputFile;
        outputFile.setWritingFile(argv[2]);//sets up on-disk writing with default writing version, from CiftiVersion's default constructor
        outputFile.setCiftiXML(inputFile.getCiftiXML());
        const vector<int64_t>& dims = inputFile.getDimensions();
        vector<float> scratchRow(dims[0]);//read/write a row at a time
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(dims.begin() + 1, dims.end())); !iter.atEnd(); ++iter)
        {//helper class to iterate over 2D and 3D cifti with the same code - the "+ 1" is to drop the first dimension (row length)
            inputFile.getRow(scratchRow.data(), *iter);
            outputFile.setRow(scratchRow.data(), *iter);
        }
        //outputFile.writeFile(argv[2]);//you don't need to call writeFile afterwards if writing on-disk, but it won't make it do anything strange
        //NOTE: if you call writeFile with a different writing version (takes its default from CiftiVersion constructor) than setWritingFile, it will rewrite the entire file after reading it into memory
    } catch (CiftiException& e) {
        cout << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 1;
    }
    return 0;
}
