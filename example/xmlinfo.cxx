#include "CiftiException.h"
#include "CiftiFile.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace cifti;

/**\file xmlinfo.cxx
This program reads a Cifti file from argv[1], and prints out a summary of the XML.

\include xmlinfo.cxx
*/

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "this program requires one argument: an input cifti file" << endl;
        return 1;
    }
    try
    {
        CiftiFile inputFile(argv[1]);//on-disk reading by default, and we only need the XML header anyway
        const CiftiXML& myXML = inputFile.getCiftiXML();
        for (int whichDim = 0; whichDim < myXML.getNumberOfDimensions(); ++whichDim)
        {
            cout << "Dimension " << whichDim << ": ";
            switch (myXML.getMappingType(whichDim))
            {
                case CiftiMappingType::BRAIN_MODELS:
                {
                    const CiftiBrainModelsMap& myMap = myXML.getBrainModelsMap(whichDim);
                    cout << "Brain Models, length " << myMap.getLength() << endl;
                    vector<CiftiBrainModelsMap::ModelInfo> myInfo = myMap.getModelInfo();//this is only summary info, same order as the models are in the cifti indices
                    for (int i = 0; i < (int)myInfo.size(); ++i)//to get the lists of vertices/voxels for a model, see getSurfaceMap, getVolumeStructureMap, and getFullVolumeMap
                    {
                        switch (myInfo[i].m_type)
                        {
                            case CiftiBrainModelsMap::SURFACE:
                                cout << "   Surface " << StructureEnum::toName(myInfo[i].m_structure) << ": ";
                                cout << myInfo[i].m_indexCount << " out of " << myMap.getSurfaceNumberOfNodes(myInfo[i].m_structure) << " vertices" << endl;
                                break;
                            case CiftiBrainModelsMap::VOXELS:
                                cout << "   Voxels " << StructureEnum::toName(myInfo[i].m_structure) << ": ";
                                cout << myInfo[i].m_indexCount << " voxels" << endl;
                                break;
                        }
                    }
                    break;
                }
                case CiftiMappingType::LABELS:
                {
                    const CiftiLabelsMap& myMap = myXML.getLabelsMap(whichDim);
                    cout << "Labels, length " << myMap.getLength() << endl;
                    for (int i = 0; i < myMap.getLength(); ++i)
                    {
                        cout << "   Index " << i << ": " << myMap.getMapName(i) << endl;
                    }
                    break;
                }
                case CiftiMappingType::PARCELS:
                {
                    const CiftiParcelsMap& myMap = myXML.getParcelsMap(whichDim);
                    cout << "Parcels, length " << myMap.getLength() << endl;
                    const vector<CiftiParcelsMap::Parcel>& myParcels = myMap.getParcels();
                    for (int i = 0; i < (int)myParcels.size(); ++i)
                    {
                        cout << "   Index " << i << ", name '" << myParcels[i].m_name << "': ";
                        int numVerts = 0;
                        for (map<StructureEnum::Enum, set<int64_t> >::const_iterator iter = myParcels[i].m_surfaceNodes.begin(); iter != myParcels[i].m_surfaceNodes.end(); ++iter)
                        {
                            numVerts += iter->second.size();
                        }
                        cout << numVerts << " vertices, " << myParcels[i].m_voxelIndices.size() << " voxels" << endl;
                    }
                    break;
                }
                case CiftiMappingType::SCALARS:
                {
                    const CiftiScalarsMap& myMap = myXML.getScalarsMap(whichDim);
                    cout << "Scalars, length " << myMap.getLength() << endl;
                    for (int i = 0; i < myMap.getLength(); ++i)
                    {
                        cout << "   Index " << i << ": " << myMap.getMapName(i) << endl;
                    }
                    break;
                }
                case CiftiMappingType::SERIES:
                {
                    const CiftiSeriesMap& myMap = myXML.getSeriesMap(whichDim);
                    cout << "Series, length " << myMap.getLength() << endl;
                    cout << "   Start: " << myMap.getStart() << endl;
                    cout << "   Step: " << myMap.getStep() << endl;
                    cout << "   Unit: ";
                    switch (myMap.getUnit())
                    {
                        case CiftiSeriesMap::SECOND:
                            cout << "Seconds" << endl;
                            break;
                        case CiftiSeriesMap::HERTZ:
                            cout << "Hertz" << endl;
                            break;
                        case CiftiSeriesMap::METER:
                            cout << "Meters" << endl;
                            break;
                        case CiftiSeriesMap::RADIAN:
                            cout << "Radians" << endl;
                            break;
                    }
                    break;
                }
            }
            cout << endl;//extra line break between dimensions
        }
    } catch (CiftiException& e) {
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 1;
    }
    return 0;
}
