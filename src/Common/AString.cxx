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

#ifdef CIFTILIB_USE_QT
#include <QStringList>
#endif

#ifdef CIFTILIB_USE_XMLPP
#include "boost/lexical_cast.hpp"
using namespace boost;
#endif

using namespace std;
using namespace cifti;

vector<AString> cifti::AString_split(const AString& input, const char& delim)
{
    vector<AString> ret;
#ifdef CIFTILIB_USE_QT
    QStringList temp = input.split(delim);
    int listSize = temp.size();//yes, QT uses int...
    ret.resize(listSize);
    for (int i = 0; i < listSize; ++i)
    {
        ret[i] = temp[i];
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    size_t start = 0, end = input.find(delim);
    while (end != AString::npos)
    {
        ret.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(delim, start);
    }
    ret.push_back(input.substr(start));
#else
#error "not implemented"
#endif
#endif
    return ret;
}

vector<AString> cifti::AString_split_whitespace(const AString& input)
{
    vector<AString> ret;
#ifdef CIFTILIB_USE_QT
    QStringList temp = input.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    int listSize = temp.size();//yes, QT uses int...
    ret.resize(listSize);
    for (int i = 0; i < listSize; ++i)
    {
        ret[i] = temp[i];
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    AString::const_iterator iter = input.begin(), end = input.end();
    while (iter != end)
    {
        while (iter != end && g_unichar_isspace(*iter)) ++iter;//skip spaces, including at start of input
        if (iter == end) break;//ignore spaces on end of input
        AString::const_iterator start = iter;
        while (iter != end && !g_unichar_isspace(*iter)) ++iter;//continue to space or end
        ret.push_back(AString(start, iter));
    }
#else
#error "not implemented"
#endif
#endif
    return ret;
}

int64_t cifti::AString_toInt(const AString& input, bool& ok)
{
#ifdef CIFTILIB_USE_QT
    return input.toLongLong(&ok);
#else
#ifdef CIFTILIB_USE_XMLPP
#ifdef CIFTILIB_BOOST_NO_TRY_LEXICAL
    try
    {
        ok = true;
        return lexical_cast<int64_t>(input);
    } catch (...) {
        ok = false;
        return 0;
    }
#else
    int64_t ret;
    ok = conversion::try_lexical_convert(input, ret);
    if (!ok) ret = 0;
    return ret;
#endif
#else
#error "not implemented"
#endif
#endif
}

float cifti::AString_toFloat(const AString& input, bool& ok)
{
#ifdef CIFTILIB_USE_QT
    return input.toFloat(&ok);
#else
#ifdef CIFTILIB_USE_XMLPP
#ifdef CIFTILIB_BOOST_NO_TRY_LEXICAL
    try
    {
        ok = true;
        return lexical_cast<float>(input);
    } catch (...) {
        ok = false;
        return 0.0f;
    }
#else
    float ret;
    ok = conversion::try_lexical_convert(input, ret);
    if (!ok) ret = 0.0f;
    return ret;
#endif
#else
#error "not implemented"
#endif
#endif
}
