/*LICENSE_START*/ 
/*
 *  Copyright (c) 2014, Washington University School of Medicine
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CiftiBrainModelsMap.h"

#include "CiftiException.h"

#include <algorithm>

using namespace std;
using namespace cifti;

void CiftiBrainModelsMap::addSurfaceModel(const int64_t& numberOfNodes, const StructureEnum::Enum& structure, const float* roi)
{
    vector<int64_t> tempVector;//pass-through to the other addSurfaceModel after converting roi to vector of indices
    tempVector.reserve(numberOfNodes);//to make it allocate only once
    for (int64_t i = 0; i < numberOfNodes; ++i)
    {
        if (roi == NULL || roi[i] > 0.0f)
        {
            tempVector.push_back(i);
        }
    }
    addSurfaceModel(numberOfNodes, structure, tempVector);
}

void CiftiBrainModelsMap::addSurfaceModel(const int64_t& numberOfNodes, const StructureEnum::Enum& structure, const vector<int64_t>& nodeList)
{
    if (m_surfUsed.find(structure) != m_surfUsed.end())
    {
        throw CiftiException("surface structures cannot be repeated in a brain models map");
    }
    BrainModelPriv myModel;
    myModel.m_type = SURFACE;
    myModel.m_brainStructure = structure;
    myModel.m_surfaceNumberOfNodes = numberOfNodes;
    myModel.m_nodeIndices = nodeList;
    myModel.setupSurface(getNextStart());//do internal setup - also does error checking
    m_modelsInfo.push_back(myModel);
    m_surfUsed[structure] = m_modelsInfo.size() - 1;
}

void CiftiBrainModelsMap::BrainModelPriv::setupSurface(const int64_t& start)
{
    if (m_surfaceNumberOfNodes < 1)
    {
        throw CiftiException("surface must have at least 1 vertex");
    }
    m_modelStart = start;
    int64_t listSize = (int64_t)m_nodeIndices.size();
    if (listSize == 0)
    {
        throw CiftiException("vertex list must have nonzero length");//NOTE: technically not required by Cifti-1, would need some rewriting to support
    }
    m_modelEnd = start + listSize;//one after last
    vector<bool> used(m_surfaceNumberOfNodes, false);
    m_nodeToIndexLookup = vector<int64_t>(m_surfaceNumberOfNodes, -1);//reset all to -1 to start
    for (int64_t i = 0; i < listSize; ++i)
    {
        if (m_nodeIndices[i] < 0)
        {
            throw CiftiException("vertex list contains negative index");
        }
        if (m_nodeIndices[i] >= m_surfaceNumberOfNodes)
        {
            throw CiftiException("vertex list contains an index that don't exist in the surface");
        }
        if (used[m_nodeIndices[i]])
        {
            throw CiftiException("vertex list contains reused index");
        }
        used[m_nodeIndices[i]] = true;
        m_nodeToIndexLookup[m_nodeIndices[i]] = start + i;
    }
}

void CiftiBrainModelsMap::addVolumeModel(const StructureEnum::Enum& structure, const vector<int64_t>& ijkList)
{
    if (m_volUsed.find(structure) != m_volUsed.end())
    {
        throw CiftiException("volume structures cannot be repeated in a brain models map");
    }
    int64_t listSize = (int64_t)ijkList.size();
    if (listSize == 0)
    {
        throw CiftiException("voxel list must have nonzero length");//NOTE: technically not required by Cifti-1, would need some rewriting to support
    }
    if (listSize % 3 != 0)
    {
        throw CiftiException("voxel list must have a length that is a multiple of 3");
    }
    int64_t numElems = listSize / 3;
    const int64_t* dims = NULL;
    if (!m_ignoreVolSpace)
    {
        if (!m_haveVolumeSpace)
        {
            throw CiftiException("you must set the volume space before adding volume models");
        }
        dims = m_volSpace.getDims();
    }
    Compact3DLookup<std::pair<int64_t, StructureEnum::Enum> > tempLookup = m_voxelToIndexLookup;//a copy of the lookup should be faster than other methods of checking for overlap and repeat
    int64_t nextStart = getNextStart();
    for (int64_t index = 0; index < numElems; ++index)//do all error checking before adding to lookup
    {
        int64_t index3 = index * 3;
        if (ijkList[index3] < 0 || ijkList[index3 + 1] < 0 || ijkList[index3 + 2] < 0)
        {
            throw CiftiException("found negative index in voxel list");
        }
        if (!m_ignoreVolSpace && (ijkList[index3] >= dims[0] ||
                                    ijkList[index3 + 1] >= dims[1] ||
                                    ijkList[index3 + 2] >= dims[2]))
        {
            throw CiftiException("found invalid index triple in voxel list: (" + AString_number(ijkList[index3]) + ", "
                                  + AString_number(ijkList[index3 + 1]) + ", " + AString_number(ijkList[index3 + 2]) + ")");
        }
        if (tempLookup.find(ijkList[index3], ijkList[index3 + 1], ijkList[index3 + 2]) != NULL)
        {
            throw CiftiException("volume models may not reuse voxels, either internally or from other structures");
        }
        tempLookup.at(ijkList[index3], ijkList[index3 + 1], ijkList[index3 + 2]) = pair<int64_t, StructureEnum::Enum>(nextStart + index, structure);
    }
    m_voxelToIndexLookup = tempLookup;
    BrainModelPriv myModel;
    myModel.m_type = VOXELS;
    myModel.m_brainStructure = structure;
    myModel.m_voxelIndicesIJK = ijkList;
    myModel.m_modelStart = nextStart;
    myModel.m_modelEnd = nextStart + numElems;//one after last
    m_modelsInfo.push_back(myModel);
    m_volUsed[structure] = m_modelsInfo.size() - 1;
}

void CiftiBrainModelsMap::clear()
{
    m_modelsInfo.clear();
    m_haveVolumeSpace = false;
    m_ignoreVolSpace = false;
    m_voxelToIndexLookup.clear();
    m_surfUsed.clear();
    m_volUsed.clear();
}

int64_t CiftiBrainModelsMap::getIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const
{
    CiftiAssert(node >= 0);
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    if (iter == m_surfUsed.end())
    {
        return -1;
    }
    CiftiAssertVectorIndex(m_modelsInfo, iter->second);
    const BrainModelPriv& myModel = m_modelsInfo[iter->second];
    if (node >= myModel.m_surfaceNumberOfNodes) return -1;
    CiftiAssertVectorIndex(myModel.m_nodeToIndexLookup, node);
    return myModel.m_nodeToIndexLookup[node];
}

int64_t CiftiBrainModelsMap::getIndexForVoxel(const int64_t* ijk, StructureEnum::Enum* structureOut) const
{
    return getIndexForVoxel(ijk[0], ijk[1], ijk[2], structureOut);
}

int64_t CiftiBrainModelsMap::getIndexForVoxel(const int64_t& i, const int64_t& j, const int64_t& k, StructureEnum::Enum* structureOut) const
{
    const pair<int64_t, StructureEnum::Enum>* iter = m_voxelToIndexLookup.find(i, j, k);//the lookup tolerates weirdness like negatives
    if (iter == NULL) return -1;
    if (structureOut != NULL) *structureOut = iter->second;
    return iter->first;
}

CiftiBrainModelsMap::IndexInfo CiftiBrainModelsMap::getInfoForIndex(const int64_t index) const
{
    CiftiAssert(index >= 0 && index < getLength());
    IndexInfo ret;
    int numModels = (int)m_modelsInfo.size();
    int low = 0, high = numModels - 1;//bisection search
    while (low != high)
    {
        int guess = (low + high) / 2;
        if (m_modelsInfo[guess].m_modelEnd > index)//modelEnd is 1 after last valid index, equal to next start if there is a next
        {
            if (m_modelsInfo[guess].m_modelStart > index)
            {
                high = guess - 1;
            } else {
                high = guess;
                low = guess;
            }
        } else {
            low = guess + 1;
        }
    }
    CiftiAssert(index >= m_modelsInfo[low].m_modelStart && index < m_modelsInfo[low].m_modelEnd);//otherwise we have a broken invariant
    ret.m_structure = m_modelsInfo[low].m_brainStructure;
    ret.m_type = m_modelsInfo[low].m_type;
    if (ret.m_type == SURFACE)
    {
        ret.m_surfaceNode = m_modelsInfo[low].m_nodeIndices[index - m_modelsInfo[low].m_modelStart];
    } else {
        int64_t baseIndex = 3 * (index - m_modelsInfo[low].m_modelStart);
        ret.m_ijk[0] = m_modelsInfo[low].m_voxelIndicesIJK[baseIndex];
        ret.m_ijk[1] = m_modelsInfo[low].m_voxelIndicesIJK[baseIndex + 1];
        ret.m_ijk[2] = m_modelsInfo[low].m_voxelIndicesIJK[baseIndex + 2];
    }
    return ret;
}

int64_t CiftiBrainModelsMap::getLength() const
{
    return getNextStart();
}

vector<CiftiBrainModelsMap::ModelInfo> CiftiBrainModelsMap::getModelInfo() const
{
    vector<ModelInfo> ret;
    int numModels = (int)m_modelsInfo.size();
    ret.resize(numModels);
    for (int i = 0; i < numModels; ++i)
    {
        ret[i].m_structure = m_modelsInfo[i].m_brainStructure;
        ret[i].m_type = m_modelsInfo[i].m_type;
        ret[i].m_indexStart = m_modelsInfo[i].m_modelStart;
        ret[i].m_indexCount = m_modelsInfo[i].m_modelEnd - m_modelsInfo[i].m_modelStart;
    }
    return ret;
}

int64_t CiftiBrainModelsMap::getNextStart() const
{
    if (m_modelsInfo.size() == 0) return 0;
    return m_modelsInfo.back().m_modelEnd;//NOTE: the models are sorted by their index range, so this works
}

const vector<int64_t>& CiftiBrainModelsMap::getNodeList(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    if (iter == m_surfUsed.end())
    {
        throw CiftiException("getNodeList called for nonexistant structure");//throw if it doesn't exist, because we don't have a reference to return - things should identify which structures exist before calling this
    }
    CiftiAssertVectorIndex(m_modelsInfo, iter->second);
    return m_modelsInfo[iter->second].m_nodeIndices;
}

vector<CiftiBrainModelsMap::SurfaceMap> CiftiBrainModelsMap::getSurfaceMap(const StructureEnum::Enum& structure) const
{
    vector<SurfaceMap> ret;
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    if (iter == m_surfUsed.end())
    {
        throw CiftiException("getSurfaceMap called for nonexistant structure");//also throw, for consistency
    }
    CiftiAssertVectorIndex(m_modelsInfo, iter->second);
    const BrainModelPriv& myModel = m_modelsInfo[iter->second];
    int64_t numUsed = (int64_t)myModel.m_nodeIndices.size();
    ret.resize(numUsed);
    for (int64_t i = 0; i < numUsed; ++i)
    {
        ret[i].m_ciftiIndex = myModel.m_modelStart + i;
        ret[i].m_surfaceNode = myModel.m_nodeIndices[i];
    }
    return ret;
}

int64_t CiftiBrainModelsMap::getSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    if (iter == m_surfUsed.end())
    {
        return -1;
    }
    CiftiAssertVectorIndex(m_modelsInfo, iter->second);
    const BrainModelPriv& myModel = m_modelsInfo[iter->second];
    return myModel.m_surfaceNumberOfNodes;
}

vector<StructureEnum::Enum> CiftiBrainModelsMap::getSurfaceStructureList() const
{
    vector<StructureEnum::Enum> ret;
    ret.reserve(m_surfUsed.size());//we can use this to tell us how many there are, but it has reordered them
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)//we need them in the order they occur in
    {
        if (m_modelsInfo[i].m_type == SURFACE)
        {
            ret.push_back(m_modelsInfo[i].m_brainStructure);
        }
    }
    return ret;
}

bool CiftiBrainModelsMap::hasSurfaceData(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    return (iter != m_surfUsed.end());
}

vector<CiftiBrainModelsMap::VolumeMap> CiftiBrainModelsMap::getFullVolumeMap() const
{
    vector<VolumeMap> ret;
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)
    {
        if (m_modelsInfo[i].m_type == VOXELS)
        {
            const BrainModelPriv& myModel = m_modelsInfo[i];
            int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
            CiftiAssert(listSize % 3 == 0);
            int64_t numUsed = listSize / 3;
            if (ret.size() == 0) ret.reserve(numUsed);//keep it from doing multiple expansion copies on the first model
            for (int64_t i = 0; i < numUsed; ++i)
            {
                int64_t i3 = i * 3;
                VolumeMap temp;
                temp.m_ciftiIndex = myModel.m_modelStart + i;
                temp.m_ijk[0] = myModel.m_voxelIndicesIJK[i3];
                temp.m_ijk[1] = myModel.m_voxelIndicesIJK[i3 + 1];
                temp.m_ijk[2] = myModel.m_voxelIndicesIJK[i3 + 2];
                ret.push_back(temp);
            }
        }
    }
    return ret;
}

const VolumeSpace& CiftiBrainModelsMap::getVolumeSpace() const
{
    CiftiAssert(!m_ignoreVolSpace);
    if (!m_haveVolumeSpace)
    {
        throw CiftiException("getVolumeSpace called when no volume space exists");
    }
    return m_volSpace;
}

vector<StructureEnum::Enum> CiftiBrainModelsMap::getVolumeStructureList() const
{
    vector<StructureEnum::Enum> ret;
    ret.reserve(m_volUsed.size());//we can use this to tell us how many there are, but it has reordered them
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)//we need them in the order they occur in
    {
        if (m_modelsInfo[i].m_type == VOXELS)
        {
            ret.push_back(m_modelsInfo[i].m_brainStructure);
        }
    }
    return ret;
}

vector<CiftiBrainModelsMap::VolumeMap> CiftiBrainModelsMap::getVolumeStructureMap(const StructureEnum::Enum& structure) const
{
    vector<VolumeMap> ret;
    map<StructureEnum::Enum, int>::const_iterator iter = m_volUsed.find(structure);
    if (iter == m_volUsed.end())
    {
        throw CiftiException("getVolumeStructureMap called for nonexistant structure");//also throw, for consistency
    }
    CiftiAssertVectorIndex(m_modelsInfo, iter->second);
    const BrainModelPriv& myModel = m_modelsInfo[iter->second];
    int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
    CiftiAssert(listSize % 3 == 0);
    int64_t numUsed = listSize / 3;
    ret.resize(numUsed);
    for (int64_t i = 0; i < numUsed; ++i)
    {
        int64_t i3 = i * 3;
        ret[i].m_ciftiIndex = myModel.m_modelStart + i;
        ret[i].m_ijk[0] = myModel.m_voxelIndicesIJK[i3];
        ret[i].m_ijk[1] = myModel.m_voxelIndicesIJK[i3 + 1];
        ret[i].m_ijk[2] = myModel.m_voxelIndicesIJK[i3 + 2];
    }
    return ret;
}

const vector<int64_t>& CiftiBrainModelsMap::getVoxelList(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_volUsed.find(structure);
    if (iter == m_volUsed.end())
    {
        throw CiftiException("getVoxelList called for nonexistant structure");//throw if it doesn't exist, because we don't have a reference to return - things should identify which structures exist before calling this
    }
    CiftiAssertVectorIndex(m_modelsInfo, iter->second);
    return m_modelsInfo[iter->second].m_voxelIndicesIJK;
}

bool CiftiBrainModelsMap::hasVolumeData() const
{
    return (m_volUsed.size() != 0);
}

bool CiftiBrainModelsMap::hasVolumeData(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_volUsed.find(structure);
    return (iter != m_volUsed.end());
}

void CiftiBrainModelsMap::setVolumeSpace(const VolumeSpace& space)
{
    for (map<StructureEnum::Enum, int>::const_iterator iter = m_volUsed.begin(); iter != m_volUsed.end(); ++iter)//the main time this loop isn't empty is parsing cifti-1
    {
        CiftiAssertVectorIndex(m_modelsInfo, iter->second);
        const BrainModelPriv& myModel = m_modelsInfo[iter->second];
        int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
        CiftiAssert(listSize % 3 == 0);
        for (int64_t i3 = 0; i3 < listSize; i3 += 3)
        {
            if (!space.indexValid(myModel.m_voxelIndicesIJK[i3], myModel.m_voxelIndicesIJK[i3 + 1], myModel.m_voxelIndicesIJK[i3 + 2]))
            {
                throw CiftiException("invalid voxel found for volume space");
            }
        }
    }
    m_ignoreVolSpace = false;
    m_haveVolumeSpace = true;
    m_volSpace = space;
}

bool CiftiBrainModelsMap::operator==(const CiftiMappingType& rhs) const
{
    if (rhs.getType() != getType()) return false;
    const CiftiBrainModelsMap& myrhs = dynamic_cast<const CiftiBrainModelsMap&>(rhs);
    CiftiAssert(!m_ignoreVolSpace && !myrhs.m_ignoreVolSpace);//these should only be true while in the process of parsing cifti-1, never otherwise
    if (m_haveVolumeSpace != myrhs.m_haveVolumeSpace) return false;
    if (m_haveVolumeSpace && (m_volSpace != myrhs.m_volSpace)) return false;
    return (m_modelsInfo == myrhs.m_modelsInfo);//NOTE: these are sorted by index range, so this works
}

bool CiftiBrainModelsMap::approximateMatch(const CiftiMappingType& rhs) const
{
    return (*this) == rhs;//there is no user-specified metadata, and it cannot match other types, so use ==
}

bool CiftiBrainModelsMap::BrainModelPriv::operator==(const BrainModelPriv& rhs) const
{
    if (m_brainStructure != rhs.m_brainStructure) return false;
    if (m_type != rhs.m_type) return false;
    if (m_modelStart != rhs.m_modelStart) return false;
    if (m_modelEnd != rhs.m_modelEnd) return false;
    if (m_type == SURFACE)
    {
        if (m_surfaceNumberOfNodes != rhs.m_surfaceNumberOfNodes) return false;
        int64_t listSize = (int64_t)m_nodeIndices.size();
        CiftiAssert((int64_t)rhs.m_nodeIndices.size() == listSize);//this should already be checked by start/end above
        for (int64_t i = 0; i < listSize; ++i)
        {
            if (m_nodeIndices[i] != rhs.m_nodeIndices[i]) return false;
        }
    } else {
        int64_t listSize = (int64_t)m_voxelIndicesIJK.size();
        CiftiAssert((int64_t)rhs.m_voxelIndicesIJK.size() == listSize);//this should already be checked by start/end above
        for (int64_t i = 0; i < listSize; ++i)
        {
            if (m_voxelIndicesIJK[i] != rhs.m_voxelIndicesIJK[i]) return false;
        }
    }
    return true;
}

void CiftiBrainModelsMap::readXML1(XmlReader& xml)
{
    clear();
    m_ignoreVolSpace = true;//because in cifti-1, the volume space is not in this element - so, we rely on CiftiXML to check for volume data, and set the volume space afterwards
    vector<ParseHelperModel> parsedModels;
#ifdef CIFTILIB_USE_QT
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name != "BrainModel")
                {
                    throw CiftiException("unexpected element in brain models map: " + name.toString());
                }
                ParseHelperModel thisModel;
                thisModel.parseBrainModel1(xml);
                if (xml.hasError()) return;
                parsedModels.push_back(thisModel);
                break;//the readNext in the for will remove the BrainModel end element
            }
            default:
                break;
        }
    }
    if (xml.hasError()) return;
#else
#ifdef CIFTILIB_USE_XMLPP
    bool done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while(!done && xml.read())
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "BrainModel")
                {
                    ParseHelperModel thisModel;
                    thisModel.parseBrainModel1(xml);
                    parsedModels.push_back(thisModel);
                } else {
                    throw CiftiException("unexpected element in brain models map: " + name);
                }
                break;
            }
            case XmlReader::EndElement:
                done = true;
                break;
            default:
                break;
        }
    }
#else
#error "not implemented"
#endif
#endif
    CiftiAssert(XmlReader_checkEndElement(xml, "MatrixIndicesMap"));
    sort(parsedModels.begin(), parsedModels.end());
    int64_t numModels = (int64_t)parsedModels.size();//because we haven't checked them for unique values of BrainStructure yet...yeah, its paranoid
    int64_t curOffset = 0;
    for (int64_t i = 0; i < numModels; ++i)
    {
        if (parsedModels[i].m_offset != curOffset)
        {
            if (parsedModels[i].m_offset < curOffset)
            {
                throw CiftiException("models overlap at index " + AString_number(parsedModels[i].m_offset) + ", model " + AString_number(i));
            } else {
                throw CiftiException("index " + AString_number(curOffset) + " is not assigned to any model");
            }
        }
        curOffset += parsedModels[i].m_count;
    }
    for (int64_t i = 0; i < numModels; ++i)
    {
        if (parsedModels[i].m_type == SURFACE)
        {
            addSurfaceModel(parsedModels[i].m_surfaceNumberOfNodes,
                            parsedModels[i].m_brainStructure,
                            parsedModels[i].m_nodeIndices);
        } else {
            addVolumeModel(parsedModels[i].m_brainStructure,
                           parsedModels[i].m_voxelIndicesIJK);
        }
    }
    m_ignoreVolSpace = false;//in case there are no voxels, but some will be added later
}

void CiftiBrainModelsMap::readXML2(XmlReader& xml)
{
    clear();
    vector<ParseHelperModel> parsedModels;
#ifdef CIFTILIB_USE_QT
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name == "BrainModel")
                {
                    ParseHelperModel thisModel;
                    thisModel.parseBrainModel2(xml);
                    if (xml.hasError()) break;
                    parsedModels.push_back(thisModel);
                } else if (name == "Volume") {
                    if (m_haveVolumeSpace)
                    {
                        throw CiftiException("Volume specified more than once in Brain Models mapping type");
                    } else {
                        m_volSpace.readCiftiXML2(xml);
                        if (xml.hasError()) return;
                        m_haveVolumeSpace = true;
                    }
                } else {
                    throw CiftiException("unexpected element in brain models map: " + name.toString());
                }
                break;//the readNext in the for will remove the BrainModel or Volume end element
            }
            default:
                break;
        }
    }
    if (xml.hasError()) return;
#else
#ifdef CIFTILIB_USE_XMLPP
    bool done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while(!done && xml.read())
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "BrainModel")
                {
                    ParseHelperModel thisModel;
                    thisModel.parseBrainModel2(xml);
                    parsedModels.push_back(thisModel);
                } else if (name == "Volume") {
                    if (m_haveVolumeSpace)
                    {
                        throw CiftiException("Volume specified more than once in Brain Models mapping type");
                    } else {
                        m_volSpace.readCiftiXML2(xml);
                        m_haveVolumeSpace = true;
                    }
                } else {
                    throw CiftiException("unexpected element in brain models map: " + name);
                }
                break;
            }
            case XmlReader::EndElement:
                done = true;
                break;
            default:
                break;
        }
    }
#else
#error "not implemented"
#endif
#endif
    CiftiAssert(XmlReader_checkEndElement(xml, "MatrixIndicesMap"));
    sort(parsedModels.begin(), parsedModels.end());
    int64_t numModels = (int64_t)parsedModels.size();//because we haven't checked them for unique values of BrainStructure yet...yeah, its paranoid
    int64_t curOffset = 0;
    for (int64_t i = 0; i < numModels; ++i)
    {
        if (parsedModels[i].m_offset != curOffset)
        {
            if (parsedModels[i].m_offset < curOffset)
            {
                throw CiftiException("models overlap at index " + AString_number(parsedModels[i].m_offset) + ", model " + AString_number(i));
            } else {
                throw CiftiException("index " + AString_number(curOffset) + " is not assigned to any model");
            }
        }
        curOffset += parsedModels[i].m_count;
    }
    for (int64_t i = 0; i < numModels; ++i)
    {
        if (parsedModels[i].m_type == SURFACE)
        {
            addSurfaceModel(parsedModels[i].m_surfaceNumberOfNodes,
                            parsedModels[i].m_brainStructure,
                            parsedModels[i].m_nodeIndices);
        } else {
            addVolumeModel(parsedModels[i].m_brainStructure,
                           parsedModels[i].m_voxelIndicesIJK);
        }
    }
}

void CiftiBrainModelsMap::ParseHelperModel::parseBrainModel1(XmlReader& xml)
{
    vector<AString> mandAttrs(4), optAttrs(1, "SurfaceNumberOfNodes");
    mandAttrs[0] = "ModelType";
    mandAttrs[1] = "BrainStructure";
    mandAttrs[2] = "IndexOffset";
    mandAttrs[3] = "IndexCount";
    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs, optAttrs);
    if (myAttrs.mandatoryVals[0] == "CIFTI_MODEL_TYPE_SURFACE")
    {
        m_type = SURFACE;
    } else if (myAttrs.mandatoryVals[0] == "CIFTI_MODEL_TYPE_VOXELS") {
        m_type = VOXELS;
    } else {
        throw CiftiException("invalid value for ModelType: " + myAttrs.mandatoryVals[0]);
    }
    bool ok = false;
    m_brainStructure = StructureEnum::fromCiftiName(myAttrs.mandatoryVals[1], &ok);
    if (!ok)
    {
        throw CiftiException("invalid value for BrainStructure: " + myAttrs.mandatoryVals[1]);
    }
    m_offset = AString_toInt(myAttrs.mandatoryVals[2], ok);
    if (!ok || m_offset < 0)
    {
        throw CiftiException("IndexOffset must be a non-negative integer");
    }
    m_count = AString_toInt(myAttrs.mandatoryVals[3], ok);
    if (!ok || m_count < 1)//NOTE: not technically required by cifti-1, would need some rewriting to support empty brainmodels
    {
        throw CiftiException("IndexCount must be a positive integer");
    }
    if (m_type == SURFACE)
    {
        if (!myAttrs.optionalVals[0].present)//actually conditionally required, not optional
        {
            throw CiftiException("BrainModel missing required attribute SurfaceNumberOfNodes");
        }
        m_surfaceNumberOfNodes = AString_toInt(myAttrs.optionalVals[0].value, ok);
        if (!ok || m_surfaceNumberOfNodes < 1)
        {
            throw CiftiException("SurfaceNumberOfNodes must be a positive integer");
        }
#ifdef CIFTILIB_USE_QT
        if (!xml.readNextStartElement())//special case in cifti-1
        {
            m_nodeIndices.resize(m_count);
            for (int64_t i = 0; i < m_count; ++i)
            {
                m_nodeIndices[i] = i;
            }
        } else {
            if (xml.name() != "NodeIndices")
            {
                throw CiftiException("unexpected element in BrainModel of SURFACE type: " + xml.name().toString());
            }
            m_nodeIndices = readIndexArray(xml);
            xml.readNext();//remove the end element of NodeIndices
        }
        if (xml.hasError()) return;
#else
#ifdef CIFTILIB_USE_XMLPP
        bool haveNodeIndices = false, done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
        while(!done && xml.read())
        {
            switch (xml.get_node_type())
            {
                case XmlReader::Element:
                {
                    AString name = xml.get_local_name();
                    if (name == "NodeIndices")
                    {
                        if (haveNodeIndices)
                        {
                            throw CiftiException("NodeIndices element may only be specified once");
                        }
                        m_nodeIndices = readIndexArray(xml);
                        haveNodeIndices = true;
                    } else {
                        throw CiftiException("unexpected element in BrainModel: " + name);
                    }
                    break;
                }
                case XmlReader::EndElement:
                    done = true;
                    break;
                default:
                    break;
            }
        }
        if (!haveNodeIndices)//special case in cifti-1
        {
            m_nodeIndices.resize(m_count);
            for (int64_t i = 0; i < m_count; ++i)
            {
                m_nodeIndices[i] = i;
            }
        }
#else
#error "not implemented"
#endif
#endif
        if ((int64_t)m_nodeIndices.size() != m_count)
        {
            throw CiftiException("number of vertex indices does not match IndexCount");
        }
    } else {
#ifdef CIFTILIB_USE_QT
        if (!xml.readNextStartElement())
        {
            throw CiftiException("BrainModel requires a child element");
        }
        if (xml.name() != "VoxelIndicesIJK")
        {
            throw CiftiException("unexpected element in BrainModel of VOXELS type: " + xml.name().toString());
        }
        m_voxelIndicesIJK = readIndexArray(xml);
        if (xml.hasError()) return;
        xml.readNext();//remove the end element of VoxelIndicesIJK
#else
#ifdef CIFTILIB_USE_XMLPP
        bool haveVoxelIndices = false, done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
        while(!done && xml.read())
        {
            switch (xml.get_node_type())
            {
                case XmlReader::Element:
                {
                    AString name = xml.get_local_name();
                    if (name == "VoxelIndicesIJK")
                    {
                        if (haveVoxelIndices)
                        {
                            throw CiftiException("VoxelIndicesIJK element may only be specified once");
                        }
                        m_voxelIndicesIJK = readIndexArray(xml);
                        haveVoxelIndices = true;
                    } else {
                        throw CiftiException("unexpected element in BrainModel: " + name);
                    }
                    break;
                }
                case XmlReader::EndElement:
                    done = true;
                    break;
                default:
                    break;
            }
        }
        if (!haveVoxelIndices)//special case in cifti-1
        {
            throw CiftiException("BrainModel requires a child element");
        }
#else
#error "not implemented"
#endif
#endif
        if (m_voxelIndicesIJK.size() % 3 != 0)
        {
            throw CiftiException("number of voxel indices is not a multiple of 3");
        }
        if ((int64_t)m_voxelIndicesIJK.size() != m_count * 3)
        {
            throw CiftiException("number of voxel indices does not match IndexCount");
        }
    }
#ifdef CIFTILIB_USE_QT
    while (!xml.atEnd() && !xml.isEndElement())//locate the end element of BrainModel
    {
        switch(xml.readNext())
        {
            case QXmlStreamReader::StartElement:
                throw CiftiException("unexpected second element in BrainModel: " + xml.name().toString());
            default:
                break;
        }
    }
#endif
    CiftiAssert(XmlReader_checkEndElement(xml, "BrainModel"));
}

void CiftiBrainModelsMap::ParseHelperModel::parseBrainModel2(XmlReader& xml)
{
    vector<AString> mandAttrs(4), optAttrs(1, "SurfaceNumberOfVertices");
    mandAttrs[0] = "ModelType";
    mandAttrs[1] = "BrainStructure";
    mandAttrs[2] = "IndexOffset";
    mandAttrs[3] = "IndexCount";
    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs, optAttrs);
    if (myAttrs.mandatoryVals[0] == "CIFTI_MODEL_TYPE_SURFACE")
    {
        m_type = SURFACE;
    } else if (myAttrs.mandatoryVals[0] == "CIFTI_MODEL_TYPE_VOXELS") {
        m_type = VOXELS;
    } else {
        throw CiftiException("invalid value for ModelType: " + myAttrs.mandatoryVals[0]);
    }
    bool ok = false;
    m_brainStructure = StructureEnum::fromCiftiName(myAttrs.mandatoryVals[1], &ok);
    if (!ok)
    {
        throw CiftiException("invalid value for BrainStructure: " + myAttrs.mandatoryVals[1]);
    }
    m_offset = AString_toInt(myAttrs.mandatoryVals[2], ok);
    if (!ok || m_offset < 0)
    {
        throw CiftiException("IndexOffset must be a non-negative integer");
    }
    m_count = AString_toInt(myAttrs.mandatoryVals[3], ok);
    if (!ok || m_count < 1)
    {
        throw CiftiException("IndexCount must be a positive integer");
    }
    if (m_type == SURFACE)
    {
        if (!myAttrs.optionalVals[0].present)//actually conditionally required, not optional
        {
            throw CiftiException("BrainModel missing required attribute SurfaceNumberOfVertices");
        }
        m_surfaceNumberOfNodes = AString_toInt(myAttrs.optionalVals[0].value, ok);
        if (!ok || m_surfaceNumberOfNodes < 1)
        {
            throw CiftiException("SurfaceNumberOfVertices must be a positive integer");
        }
#ifdef CIFTILIB_USE_QT
        if (!xml.readNextStartElement())
        {
            throw CiftiException("BrainModel requires a child element");
        }
        if (xml.name() != "VertexIndices")
        {
            throw CiftiException("unexpected element in BrainModel of SURFACE type: " + xml.name().toString());
        }
        m_nodeIndices = readIndexArray(xml);
        xml.readNext();//remove the end element of NodeIndices
        if (xml.hasError()) return;
#else
#ifdef CIFTILIB_USE_XMLPP
        bool haveVertexIndices = false, done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
        while(!done && xml.read())
        {
            switch (xml.get_node_type())
            {
                case XmlReader::Element:
                {
                    AString name = xml.get_local_name();
                    if (name == "VertexIndices")
                    {
                        if (haveVertexIndices)
                        {
                            throw CiftiException("VertexIndices element may only be specified once");
                        }
                        m_nodeIndices = readIndexArray(xml);
                        haveVertexIndices = true;
                    } else {
                        throw CiftiException("unexpected element in BrainModel: " + name);
                    }
                    break;
                }
                case XmlReader::EndElement:
                    done = true;
                    break;
                default:
                    break;
            }
        }
        if (!haveVertexIndices) throw CiftiException("VertexIndices element is missing");
#else
#error "not implemented"
#endif
#endif
        if ((int64_t)m_nodeIndices.size() != m_count)
        {
            throw CiftiException("number of vertex indices does not match IndexCount");
        }
    } else {
#ifdef CIFTILIB_USE_QT
        if (!xml.readNextStartElement())
        {
            throw CiftiException("BrainModel requires a child element");
        }
        if (xml.name() != "VoxelIndicesIJK")
        {
            throw CiftiException("unexpected element in BrainModel of VOXELS type: " + xml.name().toString());
        }
        m_voxelIndicesIJK = readIndexArray(xml);
        xml.readNext();//remove the end element of VoxelIndicesIJK
        if (xml.hasError()) return;
#else
#ifdef CIFTILIB_USE_XMLPP
        bool haveVoxelIndices = false, done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
        while(!done && xml.read())
        {
            switch (xml.get_node_type())
            {
                case XmlReader::Element:
                {
                    AString name = xml.get_local_name();
                    if (name == "VoxelIndicesIJK")
                    {
                        if (haveVoxelIndices)
                        {
                            throw CiftiException("VoxelIndicesIJK element may only be specified once");
                        }
                        m_voxelIndicesIJK = readIndexArray(xml);
                        haveVoxelIndices = true;
                    } else {
                        throw CiftiException("unexpected element in BrainModel: " + name);
                    }
                    break;
                }
                case XmlReader::EndElement:
                    done = true;
                    break;
                default:
                    break;
            }
        }
        if (!haveVoxelIndices)//special case in cifti-1
        {
            throw CiftiException("BrainModel requires a child element");
        }
#else
#error "not implemented"
#endif
#endif
        if (m_voxelIndicesIJK.size() % 3 != 0)
        {
            throw CiftiException("number of voxel indices is not a multiple of 3");
        }
        if ((int64_t)m_voxelIndicesIJK.size() != m_count * 3)
        {
            throw CiftiException("number of voxel indices does not match IndexCount");
        }
    }
#ifdef CIFTILIB_USE_QT
    while (!xml.atEnd() && !xml.isEndElement())//locate the end element of BrainModel
    {
        switch(xml.readNext())
        {
            case QXmlStreamReader::StartElement:
                throw CiftiException("unexpected second element in BrainModel: " + xml.name().toString());
            default:
                break;
        }
    }
#endif
    CiftiAssert(XmlReader_checkEndElement(xml, "BrainModel"));
}

vector<int64_t> CiftiBrainModelsMap::ParseHelperModel::readIndexArray(XmlReader& xml)
{
    vector<int64_t> ret;
    AString text = XmlReader_readElementText(xml);//throws if it encounters a start element
#ifdef CIFTILIB_USE_QT
    if (xml.hasError()) return ret;
#endif
    vector<AString> separated = AString_split_whitespace(text);
    size_t numElems = separated.size();
    ret.reserve(numElems);
    for (size_t i = 0; i < numElems; ++i)
    {
        bool ok = false;
        ret.push_back(AString_toInt(separated[i], ok));
        if (!ok)
        {
            throw CiftiException("found noninteger in index array: " + separated[i]);
        }
        if (ret.back() < 0)
        {
            throw CiftiException("found negative integer in index array: " + separated[i]);
        }
    }
    return ret;
}

void CiftiBrainModelsMap::writeXML1(XmlWriter& xml) const
{
    CiftiAssert(!m_ignoreVolSpace);
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_BRAIN_MODELS");
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)
    {
        const BrainModelPriv& myModel = m_modelsInfo[i];
        xml.writeStartElement("BrainModel");
        xml.writeAttribute("IndexOffset", AString_number(myModel.m_modelStart));
        xml.writeAttribute("IndexCount", AString_number(myModel.m_modelEnd - myModel.m_modelStart));
        xml.writeAttribute("BrainStructure", StructureEnum::toCiftiName(myModel.m_brainStructure));
        if (myModel.m_type == SURFACE)
        {
            xml.writeAttribute("ModelType", "CIFTI_MODEL_TYPE_SURFACE");
            xml.writeAttribute("SurfaceNumberOfNodes", AString_number(myModel.m_surfaceNumberOfNodes));
            xml.writeStartElement("NodeIndices");
            AString text = "";
            int64_t numNodes = (int64_t)myModel.m_nodeIndices.size();
            for (int64_t j = 0; j < numNodes; ++j)
            {
                if (j != 0) text += " ";
                text += AString_number(myModel.m_nodeIndices[j]);
            }
            xml.writeCharacters(text);
            xml.writeEndElement();
        } else {
            xml.writeAttribute("ModelType", "CIFTI_MODEL_TYPE_VOXELS");
            xml.writeStartElement("VoxelIndicesIJK");
            AString text = "";
            int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
            CiftiAssert(listSize % 3 == 0);
            for (int64_t j = 0; j < listSize; j += 3)
            {
                text += AString_number(myModel.m_voxelIndicesIJK[j]) + " " + AString_number(myModel.m_voxelIndicesIJK[j + 1]) + " " + AString_number(myModel.m_voxelIndicesIJK[j + 2]) + "\n";
            }
            xml.writeCharacters(text);
            xml.writeEndElement();
        }
        xml.writeEndElement();
    }
}

void CiftiBrainModelsMap::writeXML2(XmlWriter& xml) const
{
    CiftiAssert(!m_ignoreVolSpace);
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_BRAIN_MODELS");
    if (hasVolumeData())//could be m_haveVolumeSpace if we want to be able to write a volspace without having voxels, but that seems silly
    {
        m_volSpace.writeCiftiXML2(xml);
    }
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)
    {
        const BrainModelPriv& myModel = m_modelsInfo[i];
        xml.writeStartElement("BrainModel");
        xml.writeAttribute("IndexOffset", AString_number(myModel.m_modelStart));
        xml.writeAttribute("IndexCount", AString_number(myModel.m_modelEnd - myModel.m_modelStart));
        xml.writeAttribute("BrainStructure", StructureEnum::toCiftiName(myModel.m_brainStructure));
        if (myModel.m_type == SURFACE)
        {
            xml.writeAttribute("ModelType", "CIFTI_MODEL_TYPE_SURFACE");
            xml.writeAttribute("SurfaceNumberOfVertices", AString_number(myModel.m_surfaceNumberOfNodes));
            xml.writeStartElement("VertexIndices");
            AString text = "";
            int64_t numNodes = (int64_t)myModel.m_nodeIndices.size();
            for (int64_t j = 0; j < numNodes; ++j)
            {
                if (j != 0) text += " ";
                text += AString_number(myModel.m_nodeIndices[j]);
            }
            xml.writeCharacters(text);
            xml.writeEndElement();
        } else {
            xml.writeAttribute("ModelType", "CIFTI_MODEL_TYPE_VOXELS");
            xml.writeStartElement("VoxelIndicesIJK");
            AString text = "";
            int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
            CiftiAssert(listSize % 3 == 0);
            for (int64_t j = 0; j < listSize; j += 3)
            {
                text += AString_number(myModel.m_voxelIndicesIJK[j]) + " " + AString_number(myModel.m_voxelIndicesIJK[j + 1]) + " " + AString_number(myModel.m_voxelIndicesIJK[j + 2]) + "\n";
            }
            xml.writeCharacters(text);
            xml.writeEndElement();
        }
        xml.writeEndElement();
    }
}
