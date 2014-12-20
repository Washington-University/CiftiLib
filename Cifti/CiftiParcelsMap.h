#ifndef __CIFTI_PARCELS_MAP_H__
#define __CIFTI_PARCELS_MAP_H__

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

#include "CiftiMappingType.h"

#include "Compact3DLookup.h"
#include "StructureEnum.h"
#include "VolumeSpace.h"
#include "VoxelIJK.h"

#include <map>
#include <set>
#include <vector>

namespace cifti
{
    class CiftiParcelsMap : public CiftiMappingType
    {
    public:
        struct Parcel
        {
            std::map<StructureEnum::Enum, std::set<int64_t> > m_surfaceNodes;
            std::set<VoxelIJK> m_voxelIndices;
            AString m_name;
            bool operator==(const Parcel& rhs) const;
            bool operator!=(const Parcel& rhs) const { return !((*this) == rhs); }
            bool approximateMatch(const Parcel& rhs) const;
        };
        bool hasVolumeData() const;
        bool hasSurfaceData(const StructureEnum::Enum& structure) const;
        const VolumeSpace& getVolumeSpace() const;
        int64_t getSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const;
        int64_t getIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const;
        int64_t getIndexForVoxel(const int64_t* ijk) const;
        int64_t getIndexForVoxel(const int64_t& i, const int64_t& j, const int64_t& k) const;
        std::vector<StructureEnum::Enum> getParcelSurfaceStructures() const;
        const std::vector<Parcel>& getParcels() const { return m_parcels; }
        
        CiftiParcelsMap() { m_haveVolumeSpace = false; m_ignoreVolSpace = false; }
        void addSurface(const int& numberOfNodes, const StructureEnum::Enum& structure);
        void setVolumeSpace(const VolumeSpace& space);
        void addParcel(const Parcel& parcel);
        void clear();
        
        CiftiMappingType* clone() const { return new CiftiParcelsMap(*this); }
        MappingType getType() const { return PARCELS; }
        int64_t getLength() const { return m_parcels.size(); }
        bool operator==(const CiftiMappingType& rhs) const;
        bool approximateMatch(const CiftiMappingType& rhs) const;
        void readXML1(XmlReader& xml);
        void readXML2(XmlReader& xml);
        void writeXML1(XmlWriter& xml) const;
        void writeXML2(XmlWriter& xml) const;
    private:
        std::vector<Parcel> m_parcels;
        VolumeSpace m_volSpace;
        bool m_haveVolumeSpace, m_ignoreVolSpace;//second is needed for parsing cifti-1;
        struct SurfaceInfo
        {
            int64_t m_numNodes;
            std::vector<int64_t> m_lookup;
        };
        Compact3DLookup<int64_t> m_volLookup;
        std::map<StructureEnum::Enum, SurfaceInfo> m_surfInfo;
        static Parcel readParcel1(XmlReader& xml);
        static Parcel readParcel2(XmlReader& xml);
        static std::vector<int64_t> readIndexArray(XmlReader& xml);
    };
}

#endif //__CIFTI_PARCELS_MAP_H__
