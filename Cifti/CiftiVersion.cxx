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

#include "CiftiVersion.h"

#include "CiftiException.h"

using namespace std;
using namespace cifti;

CiftiVersion::CiftiVersion()
{
    m_major = 2;
    m_minor = 0;
}

CiftiVersion::CiftiVersion(const int16_t& major, const int16_t& minor)
{
    m_major = major;
    m_minor = minor;
}

CiftiVersion::CiftiVersion(const AString& versionString)
{
    bool ok = false;
#ifdef CIFTILIB_USE_QT
    int result = versionString.indexOf('.');
    if (result < 0)
    {
        m_minor = 0;
        m_major = versionString.toShort(&ok);
        if (!ok) throw CiftiException("improperly formatted version string: " + versionString);
    } else {
        if (result == 0) throw CiftiException("improperly formatted version string: " + versionString);
        m_major = versionString.mid(0, result).toShort(&ok);
        if (!ok) throw CiftiException("improperly formatted version string: " + versionString);
        m_minor = versionString.mid(result + 1).toShort(&ok);
        if (!ok) throw CiftiException("improperly formatted version string: " + versionString);
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    size_t result = versionString.find('.');
    if (result == AString::npos)
    {
        m_minor = 0;
        m_major = (int16_t)AString_toInt(versionString, ok);
        if (!ok) throw CiftiException("improperly formatted version string: " + versionString);
    } else {
        if (result == 0) throw CiftiException("improperly formatted version string: " + versionString);
        m_major = (int16_t)AString_toInt(versionString.substr(0, result), ok);
        if (!ok) throw CiftiException("improperly formatted version string: " + versionString);
        m_minor = (int16_t)AString_toInt(versionString.substr(result + 1), ok);
        if (!ok) throw CiftiException("improperly formatted version string: " + versionString);
    }
#else
#error "not implemented"
#endif
#endif
}

bool CiftiVersion::hasReversedFirstDims() const
{
    if (m_major == 1 && m_minor == 0) return true;
    return false;
}

bool CiftiVersion::operator<(const CiftiVersion& rhs) const
{
    if (m_major < rhs.m_major) return true;
    if (m_major == rhs.m_major && m_minor < rhs.m_minor) return true;
    return false;
}

bool CiftiVersion::operator<=(const CiftiVersion& rhs) const
{
    if (m_major < rhs.m_major) return true;
    if (m_major == rhs.m_major && m_minor <= rhs.m_minor) return true;
    return false;
}

bool CiftiVersion::operator==(const CiftiVersion& rhs) const
{
    if (m_major == rhs.m_major && m_minor == rhs.m_minor) return true;
    return false;
}

bool CiftiVersion::operator!=(const CiftiVersion& rhs) const
{
    return !(*this == rhs);
}

bool CiftiVersion::operator>(const cifti::CiftiVersion& rhs) const
{
    if (m_major > rhs.m_major) return true;
    if (m_major == rhs.m_major && m_minor > rhs.m_minor) return true;
    return false;
}

bool CiftiVersion::operator>=(const cifti::CiftiVersion& rhs) const
{
    if (m_major > rhs.m_major) return true;
    if (m_major == rhs.m_major && m_minor >= rhs.m_minor) return true;
    return false;
}

AString CiftiVersion::toString() const
{
    AString ret = AString_number(m_major);
    if (m_minor != 0) ret += "." + AString_number(m_minor);
    return ret;
}
