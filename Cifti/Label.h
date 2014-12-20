#ifndef __LABEL_H__
#define __LABEL_H__

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

namespace cifti {
    
    class GroupAndNameHierarchyItem;
    
    class Label {
        
    public:
        Label(
                   const int32_t key,
                   const AString& name);
        
        explicit Label(
                            const int32_t key,
                            const AString& name,
                            const float red,
                            const float green,
                            const float blue,
                            const float alpha);
        
        explicit Label(
                            const int32_t key,
                            const AString& name,
                            const float red,
                            const float green,
                            const float blue,
                            const float alpha,
                            const float x,
                            const float y,
                            const float z);
        
        explicit Label(
                            const int32_t key,
                            const AString& name,
                            const double red,
                            const double green,
                            const double blue,
                            const double alpha);
        
        Label(
                   const int32_t key,
                   const AString& name,
                   const float rgba[]);
        
        explicit Label(
                            const int32_t key,
                            const AString& name,
                            const int32_t red,
                            const int32_t green,
                            const int32_t blue,
                            const int32_t alpha);
        
        Label(
                   const int32_t key,
                   const AString& name,
                   const int32_t rgba[]);
        
        Label(const int32_t key);
        
        Label(const Label& gl);
        
    public:
        Label& operator=(const Label& gl);
        
        virtual ~Label();
        
    private:
        void copyHelper(const Label& o);
        
        void initializeMembersLabel();
        
    public:
        int32_t hashCode();
        
        bool equals(const Label&);
        
        int32_t operator<(const Label& gl);
        
        int32_t getKey() const;
        
        void setKey(const int32_t key);
        
        AString getName() const;
        
        void setName(const AString& name);
        
        bool isSelected() const;
        
        void setSelected(const bool selected);
        
        float* getColor() const;
        
        void getColor(float rgbaOut[]) const;
        
        void setColor(const float rgba[]);
        
        int32_t* getColorInt() const;
        
        void setColorInt(const int32_t rgba[]);
        
        static void getDefaultColor(float rgbaOut[4]);
        
        float getRed() const;
        
        float getGreen() const;
        
        float getBlue() const;
        
        float getAlpha() const;
        
        float getX() const;
        
        float getY() const;
        
        float getZ() const;
        
        void getXYZ(float xyz[3]) const;
        
        void setX(const float x);
        
        void setY(const float y);
        
        void setZ(const float z);
        
        void setXYZ(const float xyz[3]);
        
        bool matches(const Label& rhs, const bool checkColor = false, const bool checkCoord = false) const;
        
        /**
         * @return The invalid label key.
         */
        static inline int32_t getInvalidLabelKey() { return s_invalidLabelKey; }
        
    private:
        AString name;
        
        int32_t key;
        
        bool selected;
        
        float red;
        
        float green;
        
        float blue;
        
        float alpha;
        
        float x;
        
        float y;
        
        float z;
        
        /** The invalid label key */
        const static int32_t s_invalidLabelKey;
    };
    
} // namespace

#endif // __LABEL_H__
