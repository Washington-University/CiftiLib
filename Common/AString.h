#ifndef __ASTRING_H__
#define __ASTRING_H__

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

#include <string>
#include <vector>

#include "stdint.h"

#ifdef __ASTRING_H_HAVE_IMPL__
#undef __ASTRING_H_HAVE_IMPL__
#endif

#ifdef CIFTILIB_USE_QT
#define __ASTRING_H_HAVE_IMPL__
#include <QString>
namespace cifti
{
    typedef QString AString;
#define ASTRING_TO_CSTR(mystr) ((mystr).toLocal8Bit().constData())
#define ASTRING_UTF8_RAW(mystr) ((mystr).toUtf8().constData())
    inline std::string AString_to_std_string(const AString& mystr)
    {
        QByteArray temparray = mystr.toLocal8Bit();
        return std::string(temparray.constData(), temparray.size());
    }
    inline AString AString_from_latin1(const char* data, const int& size)
    {
        return QString::fromLatin1(data, size);
    }
    inline AString AString_substr(const AString& mystr, const int& first, const int& count = -1)
    {
        return mystr.mid(first, count);
    }
    template <typename T>
    AString AString_number(const T& num)
    {
        return QString::number(num);
    }
    template <typename T>
    AString AString_number_fixed(const T& num, const int& numDecimals)
    {
        return QString::number(num, 'f', numDecimals);
    }
}
#endif //CIFTILIB_USE_QT

#ifdef CIFTILIB_USE_XMLPP
#define __ASTRING_H_HAVE_IMPL__
#include "glibmm/convert.h"
#include "glibmm/ustring.h"
#include <iomanip>
namespace cifti
{
    typedef Glib::ustring AString;
#define ASTRING_TO_CSTR(mystr) (Glib::locale_from_utf8((mystr)).c_str())
#define ASTRING_UTF8_RAW(mystr) ((mystr).data())
    inline std::string AString_to_std_string(const AString& mystr)
    {
        return Glib::locale_from_utf8(mystr);
    }
    inline AString AString_from_latin1(const char* data, const int& size)
    {
        return Glib::convert(std::string(data, size), "UTF-8", "ISO-8859-1");
    }
    inline AString AString_substr(const AString& mystr, const Glib::ustring::size_type& first, const Glib::ustring::size_type& count = std::string::npos)
    {//HACK: Glib::ustring::npos is undefined at link time with glibmm 2.4 for unknown reasons, but the header says it is equal to std::string's, so use it instead
        return mystr.substr(first, count);
    }
    template <typename T>
    AString AString_number(const T& num)
    {
        return Glib::ustring::format(num);
    }
    template <typename T>
    AString AString_number_fixed(const T& num, const int& numDecimals)
    {
        return Glib::ustring::format(std::fixed, std::setprecision(numDecimals), num);
    }
}
#endif //CIFTILIB_USE_XMLPP

#ifndef __ASTRING_H_HAVE_IMPL__
#error "you must define either CIFTILIB_USE_QT or CIFTILIB_USE_XMLPP to select what unicode string implementation to use"
#endif

namespace cifti
{
    //more helper functions
    std::vector<AString> AString_split(const AString& input, const char& delim);
    std::vector<AString> AString_split_whitespace(const AString& input);
    int64_t AString_toInt(const AString& input, bool& ok);
    float AString_toFloat(const AString& input, bool& ok);
}

#endif //__ASTRING_H__
