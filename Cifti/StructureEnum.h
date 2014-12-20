#ifndef __STRUCTURE_ENUM__H_
#define __STRUCTURE_ENUM__H_

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

#include "AString.h"

#include <stdint.h>
#include <vector>

namespace cifti {

/**
 * \brief Enumerated type for a structure in a brain.
 *
 * Enumerated types for the individual structures in a brain.
 */
class StructureEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Invalid */
        INVALID,
        /** All Strucures */
        ALL,
        /** All white matter */
        ALL_WHITE_MATTER,
        /** All grey matter */
        ALL_GREY_MATTER,
        /** Left Nucleus Accumbens */
        ACCUMBENS_LEFT,
        /** Right Nucleus Accumbens */
        ACCUMBENS_RIGHT,
        /** Left Amygdala */
        AMYGDALA_LEFT,
        /** Right Amygdala */
        AMYGDALA_RIGHT,
        /** Brain Stem */
        BRAIN_STEM,
        /** Left Caudate */
        CAUDATE_LEFT,
        /** Right Caudate */
        CAUDATE_RIGHT,
        /** Cerebellar white matter left */
        CEREBELLAR_WHITE_MATTER_LEFT,
        /** Cerebellar white matter right */
        CEREBELLAR_WHITE_MATTER_RIGHT,
        /** Cerebellum */
        CEREBELLUM,
        /** Left Cerebellum */
        CEREBELLUM_LEFT,
        /** Right Cerebellum */
        CEREBELLUM_RIGHT,
        /** Cerebral white matter left */
        CEREBRAL_WHITE_MATTER_LEFT,
        /** Cerebral white matter right */
        CEREBRAL_WHITE_MATTER_RIGHT,
        /** Cortex not specified */
        CORTEX,
        /** Left Cerebral Cortex */
        CORTEX_LEFT,
        /** Right Cerebral Cortex*/
        CORTEX_RIGHT,
        /** Left Ventral Diencephalon */
        DIENCEPHALON_VENTRAL_LEFT,
        /** Right Ventral Diencephalon */
        DIENCEPHALON_VENTRAL_RIGHT,
        /** Left Hippocampus */
        HIPPOCAMPUS_LEFT,
        /** Right Hippocampus */
        HIPPOCAMPUS_RIGHT,
        /** Left Pallidum */
        PALLIDUM_LEFT,
        /** Right Pallidum */
        PALLIDUM_RIGHT,
        /** Other structure not specified */
        OTHER,
        /** Other grey matter */
        OTHER_GREY_MATTER,
        /** Other white matter */
        OTHER_WHITE_MATTER,
        /** Left Putamen */
        PUTAMEN_LEFT,
        /** Right Putamen */
        PUTAMEN_RIGHT,
        /** Left Thalamus */
        THALAMUS_LEFT,
        /** Right Thalamus */
        THALAMUS_RIGHT
    };


    ~StructureEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static AString toCiftiName(Enum enumValue);

    static Enum fromCiftiName(const AString& ciftiName, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static bool isRight(const Enum enumValue);
    
    static bool isLeft(const Enum enumValue);
    
    static bool isCortexContralateral(const Enum enumValueA,
                                      const Enum enumValueB);
    
    static Enum getContralateralStructure(const Enum enumValue);
    
private:
    StructureEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& guiName);

    static const StructureEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<StructureEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
};

} // namespace
#endif  //__STRUCTURE_ENUM__H_
