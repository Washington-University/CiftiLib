#ifndef __XML_ADAPTER_H__
#define __XML_ADAPTER_H__

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
#include "CiftiException.h"

#include <map>
#include <vector>

#ifdef __XML_ADAPTER_H_HAVE_IMPL__
#undef __XML_ADAPTER_H_HAVE_IMPL__
#endif

#ifdef CIFTILIB_USE_QT
#define __XML_ADAPTER_H_HAVE_IMPL__
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
namespace cifti
{
    typedef QXmlStreamReader XmlReader;
    typedef QXmlStreamWriter XmlWriter;
}
#endif //CIFTILIB_USE_QT

#ifdef CIFTILIB_USE_XMLPP
#define __XML_ADAPTER_H_HAVE_IMPL__
#include "CiftiAssert.h"
#include "CiftiException.h"
#include "libxml++/libxml++.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlstring.h"
namespace cifti
{
    typedef xmlpp::TextReader XmlReader;
    class XmlWriter
    {//write our own wrapper for the C writing API, as libxml++ doesn't wrap it
        xmlTextWriterPtr m_xmlPtr;
        xmlBufferPtr m_bufPtr;
        std::vector<AString> m_elementStack;//track element names for better error messages
    public:
        XmlWriter()
        {//only support writing to memory
            m_bufPtr = xmlBufferCreate();
            if (m_bufPtr == NULL) throw CiftiException("error creating xml buffer");
            m_xmlPtr = xmlNewTextWriterMemory(m_bufPtr, 0);
            if (m_xmlPtr == NULL)
            {
                xmlBufferFree(m_bufPtr);
                throw CiftiException("error creating xml writer");
            }
            if (xmlTextWriterSetIndent(m_xmlPtr, 1) != 0 || xmlTextWriterSetIndentString(m_xmlPtr, BAD_CAST "    ") != 0)
            {
                throw CiftiException("error setting xml writer indentation");
            }
        }
        ~XmlWriter()
        {
            xmlFreeTextWriter(m_xmlPtr);
            xmlBufferFree(m_bufPtr);
        }
        void writeStartDocument()//copy a subset of the QXmlStreamWriter interface, so we don't have to rewrite much (any?) xml writing code
        {
            if (xmlTextWriterStartDocument(m_xmlPtr, NULL, NULL, NULL) == -1) throw CiftiException("error writing document start");
        }
        void writeEndDocument()
        {
            if (xmlTextWriterEndDocument(m_xmlPtr) == -1) throw CiftiException("error writing document end");
            m_elementStack.clear();
        }
        void writeStartElement(const AString& name)
        {
            if (xmlTextWriterStartElement(m_xmlPtr, BAD_CAST ASTRING_UTF8_RAW(name)) == -1) throw CiftiException("error writing " + name + " element");
            m_elementStack.push_back(name);
        }
        void writeEndElement()
        {
            CiftiAssert(m_elementStack.size() > 0);
            if (xmlTextWriterEndElement(m_xmlPtr) == -1) throw CiftiException("error writing end element for " + m_elementStack.back());
            m_elementStack.pop_back();
        }
        void writeCharacters(const AString& text)
        {
            if (xmlTextWriterWriteString(m_xmlPtr, BAD_CAST ASTRING_UTF8_RAW(text)) == -1) throw CiftiException("error writing element text");
        }
        void writeTextElement(const AString& name, const AString& text)
        {
            if (xmlTextWriterWriteElement(m_xmlPtr, BAD_CAST ASTRING_UTF8_RAW(name), BAD_CAST ASTRING_UTF8_RAW(text)) == -1)
            {
                throw CiftiException("error writing " + name + " element");
            }
        }
        void writeAttribute(const AString& name, const AString& text)
        {
            CiftiAssert(m_elementStack.size() > 0);
            if (xmlTextWriterWriteAttribute(m_xmlPtr, BAD_CAST ASTRING_UTF8_RAW(name), BAD_CAST ASTRING_UTF8_RAW(text)) == -1)
            {
                throw CiftiException("error writing " + name + " attribute of " + m_elementStack.back() + " element");
            }
        }
        std::vector<char> getXmlData() const
        {
            std::vector<char> ret(m_bufPtr->use);//this includes the null terminator?
            for (unsigned int i = 0; i < m_bufPtr->use; ++i)
            {
                ret[i] = m_bufPtr->content[i];
            }
            return ret;
        }
    };
    
}
#endif //CIFTILIB_USE_XMLPP

#ifndef __XML_ADAPTER_H_HAVE_IMPL__
#error "you must define either CIFTILIB_USE_QT or CIFTILIB_USE_XMLPP to select what XML implementation to use"
#endif

namespace cifti
{
    //helper functions that exist for all xml libraries
    struct XmlAttributesResult
    {
        struct OptionalStatus
        {
            OptionalStatus() { present = false; }
            bool present;
            AString value;
        };
        std::vector<AString> mandatoryVals;
        std::vector<OptionalStatus> optionalVals;
    };
    
    AString XmlReader_readElementText(XmlReader& xml);
    bool XmlReader_readNextStartElement(XmlReader& xml);
    AString XmlReader_elementName(XmlReader& xml);
    XmlAttributesResult XmlReader_parseAttributes(XmlReader& xml, const std::vector<AString>& mandatoryNames, const std::vector<AString>& optionalNames = std::vector<AString>());
    bool XmlReader_checkEndElement(XmlReader& xml, const AString& elementName);//for use in asserts at end of element parsing functions
}

#endif //__XML_ADAPTER_H__
