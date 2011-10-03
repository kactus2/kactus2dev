/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#include <QtGlobal>
#include <QApplication>

#ifdef VCL_USE_STATIC_LIB

    #define EXPORT_CLASS

#else

    #ifdef VCL_MAKE_DLL
        #define GCF_EXPORT_CLASS Q_DECL_EXPORT
    #else
        #define GCF_EXPORT_CLASS Q_DECL_IMPORT
    #endif

    #ifdef VCL_MAKE_DLL2
        #define GCF_EXPORT_CLASS2 Q_DECL_EXPORT
    #else
        #define GCF_EXPORT_CLASS2 Q_DECL_IMPORT
    #endif

#endif

#define GCF_EXPORT_INTERFACE GCF_EXPORT_CLASS
#define GCF_EXPORT_METHOD GCF_EXPORT_CLASS

namespace GCF
{

    class GCF_EXPORT_INTERFACE AbstractSharedData
    {
    public:
        AbstractSharedData() : mRefCount(1), mValid(false) { }
        virtual ~AbstractSharedData() { }

        void ref() { ++mRefCount; }
        void deref() { --mRefCount; if(mRefCount <= 0) delete this; }
        int refCount() const { return mRefCount; }
        bool isValid() const { return mValid; }
        void setValid(bool val=true) { mValid = val; }

    private:
        int mRefCount;
        bool mValid;
    };

    class HourGlass
    {
    public:
        HourGlass() { QApplication::setOverrideCursor(Qt::WaitCursor); }
        ~HourGlass() { QApplication::restoreOverrideCursor(); }
    };

    class GCF_EXPORT_CLASS VersionInfo
    {
    public:
        static QString versionInfoString();
        static int     majorVersion();
        static int     minorVersion();
        static int     revisionNumber();
    };

}

// Specify whether the Build Key must be used or not. Comment the line
// below if you dont care much about build keys
#define GCF_USE_BUILD_KEY

#ifdef GCF_LOAD_BUILD_KEY
    #include "BuildKey.h"
#endif

// IF GCF Build Key is not specified in any of the #define MACROs, then
// use this as the build key. Alternatively, you can also edit the line
// below and use a single build key for all your products
#ifndef GCF_BUILD_KEY
    #define GCF_BUILD_KEY "T$%eV6Dk"
#endif

#include <QMetaType>

// August 12, 2008. @Akademy 2008, Akonadi/KOffice Den
// If ACTIVE_QT_AVAILABLE is defined, then you can use ActiveX merging
// with GCF applications. This is available only on Windows, obviously
#ifdef Q_OS_WIN
    #if (QT_EDITION & QT_MODULE_ACTIVEQT)
        #define GCF_ACTIVEQT_AVAILABLE
        class QAxWidget;
        Q_DECLARE_METATYPE(QAxWidget*)
    #endif
#endif

// August 12, 2008. @Akademy 2008, Akonadi/KOffice Den
// Check for KPARTS_AVAILABLE macro here. This #define is set
// from the config.pri file
#ifdef GCF_KPARTS_AVAILABLE
    namespace KParts { class Part; }
    class KXMLGUIClient;
    Q_DECLARE_METATYPE(KParts::Part*)
    Q_DECLARE_METATYPE(KXMLGUIClient*)
#endif

#endif

