#ifndef __CIFTI_MUTEX_H__
#define __CIFTI_MUTEX_H__

/*LICENSE_START*/ 
/*
 *  Copyright (c) 2016, Washington University School of Medicine
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

#ifdef _OPENMP
#define __CIFTI_MUTEX_H_HAVE_IMPL__

#include "omp.h"

namespace cifti
{
    class CiftiMutex
    {
        omp_lock_t m_lock;
    public:
        CiftiMutex(const CiftiMutex&) { omp_init_lock(&m_lock); };//allow copy, assign, but make them do nothing other than default construct
        CiftiMutex& operator=(const CiftiMutex&) { return *this; };
        CiftiMutex() { omp_init_lock(&m_lock); }
        ~CiftiMutex() { omp_destroy_lock(&m_lock); }
        friend class CiftiMutexLocker;
    };
    
    class CiftiMutexLocker
    {
        CiftiMutex* m_mutex;
        CiftiMutexLocker();//disallow default construction, assign
        CiftiMutexLocker& operator=(const CiftiMutexLocker& rhs);
    public:
        CiftiMutexLocker(CiftiMutex* mutex) { m_mutex = mutex; omp_set_lock(&(m_mutex->m_lock)); }
        ~CiftiMutexLocker() { omp_unset_lock(&(m_mutex->m_lock)); }
    };
}

#else //_OPENMP

#ifdef CIFTILIB_USE_QT
#define __CIFTI_MUTEX_H_HAVE_IMPL__

#include <QMutex>

namespace cifti
{
    typedef QMutex CiftiMutex;
    typedef QMutexLocker CiftiMutexLocker;
}

#endif //CIFTILIB_USE_QT

#ifdef CIFTILIB_USE_XMLPP
#define __CIFTI_MUTEX_H_HAVE_IMPL__

#include <glibmm/thread.h>

namespace cifti
{
    typedef Glib::Mutex CiftiMutex;
    
    //API difference: glib's locker class takes a reference, while QT's takes a pointer
    class CiftiMutexLocker
    {
        CiftiMutex* m_mutex;
        CiftiMutexLocker();//disallow default construction, assign
        CiftiMutexLocker& operator=(const CiftiMutexLocker& rhs);
    public:
        CiftiMutexLocker(CiftiMutex* mutex) { m_mutex = mutex; m_mutex->lock(); }
        ~CiftiMutexLocker() { m_mutex->unlock(); }
    };
}

#endif //CIFTILIB_USE_XMLPP

#endif //_OPENMP


#ifndef __CIFTI_MUTEX_H_HAVE_IMPL__
#error "you must have openmp support, or define either CIFTILIB_USE_QT or CIFTILIB_USE_XMLPP to select what mutex implementation to use"
#endif

#endif //__CIFTI_MUTEX_H__
