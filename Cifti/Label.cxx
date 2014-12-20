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

#include <sstream>

#include "Label.h"

#include <limits>

using namespace cifti;

const int32_t Label::s_invalidLabelKey =  std::numeric_limits<int32_t>::min(); 

/**
 * Constructor.
 *
 * @param key - key of the label.
 * @param name - name of label.
 *
 */
Label::Label(
                   const int32_t key,
                   const AString& name)
{
    this->initializeMembersLabel();
    this->key = key;
    this->name = name;
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param red - red color component, zero to one.
 * @param green - green color component, zero to one.
 * @param blue - blue color component, zero to one.
 * @param alpha - alpha color component, zero to one.
 *
 */
Label::Label(
                   const int32_t key,
                   const AString& name,
                   const float red,
                   const float green,
                   const float blue,
                   const float alpha)
{
    this->initializeMembersLabel();
    this->key = key;
    this->name = name;
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param red - red color component, zero to one.
 * @param green - green color component, zero to one.
 * @param blue - blue color component, zero to one.
 * @param alpha - alpha color component, zero to one.
 *
 */
Label::Label(const int32_t key,
                       const AString& name,
                       const float red,
                       const float green,
                       const float blue,
                       const float alpha,
                       const float x,
                       const float y,
                       const float z)
{
    this->initializeMembersLabel();
    this->key = key;
    this->name = name;
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
    this->x = x;
    this->y = y;
    this->z = z;
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param red - red color component, zero to one.
 * @param green - green color component, zero to one.
 * @param blue - blue color component, zero to one.
 * @param alpha - alpha color component, zero to one.
 *
 */
Label::Label(
                       const int32_t key,
                       const AString& name,
                       const double red,
                       const double green,
                       const double blue,
                       const double alpha)
{
    this->initializeMembersLabel();
    this->key = key;
    this->name = name;
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param rgba - red, green, blue, alpha color componenents, zero to one.
 *
 */
Label::Label(
                   const int32_t key,
                   const AString& name,
                   const float rgba[])
{
    this->initializeMembersLabel();
    this->key = key;
    this->name = name;
    this->red = rgba[0];
    this->green = rgba[1];
    this->blue = rgba[2];
    this->alpha = rgba[3];
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param red - red color component, zero to two-fifty-five.
 * @param green - green color component, zero to two-fifty-five.
 * @param blue - blue color component, zero to two-fifty-five.
 * @param alpha - alpha color component, zero to two-fifty-five.
 *
 */
Label::Label(
                   const int32_t key,
                   const AString& name,
                   const int32_t red,
                   const int32_t green,
                   const int32_t blue,
                   const int32_t alpha)
{
    this->initializeMembersLabel();
    this->key = key;
    this->name = name;
    this->red = red / 255.0;
    this->green = green / 255.0;
    this->blue = blue / 255.0;
    this->alpha = alpha / 255.0;
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param rgba - red, green, blue, alpha color componenents, zero to 255.
 *
 */
Label::Label(
                   const int32_t key,
                   const AString& name,
                   const int32_t rgba[])
{
    this->initializeMembersLabel();
    this->key = key;
    this->name = name;
    this->red = rgba[0] / 255.0;
    this->green = rgba[1] / 255.0;
    this->blue = rgba[2] / 255.0;
    this->alpha = rgba[3] / 255.0;
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 *
 */
Label::Label(
                   const int32_t key)
{
    this->initializeMembersLabel();
    this->key = key;
    if (this->key == 0) {
        this->name = "???";    
    }
    else {
        std::stringstream str;
        str << "???" << this->key;
        this->name = str.str().c_str();
    }
}

/**
 * Destructor
 */
Label::~Label()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
Label::Label(const Label& o)
{
    this->initializeMembersLabel();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
Label&
Label::operator=(const Label& o)
{
    if (this != &o) {
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
Label::copyHelper(const Label& gl)
{
    this->initializeMembersLabel();
    this->name = gl.name;
    this->key = gl.key;
    this->selected = gl.selected;
    this->red = gl.red;
    this->green = gl.green;    
    this->blue = gl.blue;    
    this->alpha = gl.alpha;
    this->x = gl.x;
    this->y = gl.y;
    this->z = gl.z;
}

/**
 * Initialize data members.
 */
void
Label::initializeMembersLabel() 
{
    this->name = "";
    this->key = s_invalidLabelKey;
    this->selected = true;
    this->red = 1.0;
    this->green = 1.0;    
    this->blue = 1.0;    
    this->alpha = 1.0;
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
}

/**
 * Determine if two objects are equal.  Two Labels are equal if they 
 * have the same "key".
 * @param obj Object for comparison.
 * @return true if equal, else false.
 *
 */
bool
Label::equals(const Label& gl)
{
    return (this->key == gl.key);
}

/**
 * Compare this label to another label using the indices of the labels.
 * @param gl - Compare to this Label.
 * @return negative if "this" is less, positive if "this" is greater,
 * else zero.
 *
 */
int32_t
Label::operator<(const Label& gl)
{
    return (this->key < gl.key);
}

/**
 * Get the key of this label.
 * @return key of the label.
 *
 */
int32_t
Label::getKey() const
{
    return this->key;
}

/**
 * Set the key of this label.  DO NOT call this method on a label
 * retrieved from the label table.
 * 
 * @param key - New key for this label.
 *
 */
void
Label::setKey(const int32_t key)
{
    this->key = key;
}

/**
 * Get the name.
 * @return Name of label.
 *
 */
AString
Label::getName() const
{
    return this->name;
}

/**
 * Set the name.
 * @param name - new name for label.
 *
 */
void
Label::setName(const AString& name)
{
    this->name = name;
}

/**
 * Is this label selected (for display)?
 *
 * @return  true if label selected for display, else false.
 *
 */
bool
Label::isSelected() const
{
    return this->selected;
}

/**
 * Set the label selected (for display).
 *
 * @param selected - new selection status.
 *
 */
void
Label::setSelected(const bool selected)
{
    this->selected = selected;
}

/**
 * Get the color components.
 *
 * @return  A four-dimensional array of floats containing the red, green,
 * blue, and alpha components with values ranging from 0.0 to 1.0.
 * User MUST delete[] returned array.
 *
 */
float*
Label::getColor() const
{
    float* rgba = new float[4];
    rgba[0] = this->red;
    rgba[1] = this->green;
    rgba[2] = this->blue;
    rgba[3] = this->alpha;
    return rgba;
}

/**
 * Get the color components.
 * @param rgbaOut four dimensional array into which are loaded,
 * red, green, blue, and alpha components ranging 0.0. to 1.0.
 *
 */
void
Label::getColor(float rgbaOut[]) const
{
    rgbaOut[0] = this->red;
    rgbaOut[1] = this->green;
    rgbaOut[2] = this->blue;
    rgbaOut[3] = this->alpha;
}

/**
 * Set the color components.
 *
 * @param rgba - A four-dimensional array of floats containing the red,
 * green, blue, and alpha components with values ranging from 0.0 to 1.0.
 *
 */
void
Label::setColor(const float rgba[])
{
    this->red = rgba[0];
    this->green = rgba[1];
    this->blue = rgba[2];
    this->alpha = rgba[3];
}

/**
 * Get the colors as integers ranging 0 to 255.
 * @return  Four-dimensional array containing color components.
 * User must delete[] the returned array.
 *
 */
int32_t*
Label::getColorInt() const
{
    int32_t* rgbaOut = new int32_t[4];
    rgbaOut[0] = static_cast<int32_t>(this->red * 255);
    rgbaOut[1] = static_cast<int32_t>(this->green * 255);
    rgbaOut[2] = static_cast<int32_t>(this->blue * 255);
    rgbaOut[3] = static_cast<int32_t>(this->alpha * 255);
    return rgbaOut;
}

/**
 * Set the colors with integers ranging 0 to 255.
 * @param rgba - four-dimensional array with colors ranging 0 to 255.
 *
 */
void
Label::setColorInt(const int32_t rgba[])
{
    this->red = rgba[0] / 255.0;
    this->green = rgba[1] / 255.0;
    this->blue = rgba[2] / 255.0;
    this->alpha = rgba[3] / 255.0;
}

/**
 * Get the default color.
 *
 * @param Output with a four-dimensional array of floats
 * containing the red, green, blue, and alpha components with values
 * ranging from 0.0 to 1.0.
 */
void
Label::getDefaultColor(float rgbaOut[4])
{
    rgbaOut[0] = 1.0;
    rgbaOut[1] = 1.0;
    rgbaOut[2] = 1.0;
    rgbaOut[3] = 1.0;
}

/**
 * Get the red color component for this label.
 * @return red color component.
 *
 */
float
Label::getRed() const
{
    return this->red;
}

/**
 * Get the green color component for this label.
 * @return green color component.
 *
 */
float
Label::getGreen() const
{
    return this->green;
}

/**
 * Get the blue color component for this label.
 * @return blue color component.
 *
 */
float
Label::getBlue() const
{
    return this->blue;
}

/**
 * Get the alpha color component for this label.
 * @return alpha color component.
 *
 */
float
Label::getAlpha() const
{
    return this->alpha;
}

/**
 * Get the X-Coordinate.
 * @return
 *    The X-coordinate.
 */
float 
Label::getX() const 
{ 
    return this->x; 
}

/**
 * Get the Y-Coordinate.
 * @return
 *    The Y-coordinate.
 */
float 
Label::getY() const 
{ return this->y; 
}

/**
 * Get the Z-Coordinate.
 * @return
 *    The Z-coordinate.
 */
float 
Label::getZ() const 
{ 
    return this->z; 
}

/**
 * Get the XYZ coordiantes.
 * @param  xyz
 *   Array into which coordinates are loaded.
 */
void 
Label::getXYZ(float xyz[3]) const
{
    xyz[0] = this->x;
    xyz[1] = this->y;
    xyz[2] = this->z;
}

/**
 * Set the X-coordinate.
 * @param x
 *    New value for X-coordinate.
 */
void 
Label::setX(const float x)
{
    this->x = x;
}

/**
 * Set the Y-coordinate.
 * @param y
 *    New value for Y-coordinate.
 */
void 
Label::setY(const float y)
{
    this->y = y;
}

/**
 * Set the Z-coordinate.
 * @param z
 *    New value for Z-coordinate.
 */
void 
Label::setZ(const float z)
{
    this->z = z;
}

/**
 * Set the XYZ coordinates.
 * @param xyz
 *   Array containing XYZ coordiantes.
 */
void 
Label::setXYZ(const float xyz[3])
{
    this->x = xyz[0];
    this->y = xyz[1];
    this->z = xyz[2];
}

bool
Label::matches(const Label& rhs, const bool checkColor, const bool checkCoord) const
{
    if (key != rhs.key) return false;
    if (name != rhs.name) return false;
    if (checkColor)
    {
        if (red != rhs.red) return false;
        if (green != rhs.green) return false;
        if (blue != rhs.blue) return false;
        if (alpha != rhs.alpha) return false;
    }
    if (checkCoord)
    {
        if (x != rhs.x) return false;
        if (y != rhs.y) return false;
        if (z != rhs.z) return false;
    }
    return true;
}
