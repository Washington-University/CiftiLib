#ifndef __CIFTI_MAPPING_TYPE_H__
#define __CIFTI_MAPPING_TYPE_H__

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

#include "stdint.h"

#include "XmlAdapter.h"

namespace cifti
{
    
    class CiftiMappingType
    {
    public:
        enum MappingType
        {
            BRAIN_MODELS = 1,//compatibility values with old XML enum, in case someone uses the wrong enum
            PARCELS = 3,//fibers doesn't exist in 2.0
            SERIES = 4,
            SCALARS = 5,
            LABELS = 6
        };
        virtual CiftiMappingType* clone() const = 0;//make a copy, preserving the actual type - NOTE: this returns a dynamic allocation that is not owned by anything
        virtual MappingType getType() const = 0;
        virtual int64_t getLength() const = 0;
        virtual bool operator==(const CiftiMappingType& rhs) const = 0;//used to check for merging mappings when writing the XML - must compare EVERYTHING that goes into the XML
        bool operator!=(const CiftiMappingType& rhs) const { return !((*this) == rhs); }
        virtual bool approximateMatch(const CiftiMappingType& rhs) const = 0;//check if things like doing index-wise math would make sense
        virtual void readXML1(XmlReader& xml) = 0;//mainly to shorten the type-specific code in CiftiXML
        virtual void readXML2(XmlReader& xml) = 0;
        virtual void writeXML1(XmlWriter& xml) const = 0;
        virtual void writeXML2(XmlWriter& xml) const = 0;
        virtual ~CiftiMappingType();
    };
}

#endif //__CIFTI_MAPPING_TYPE_H__
