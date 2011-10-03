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

#ifndef I_QUERYABLE_H
#define I_QUERYABLE_H

#include <QString>
#include <QObject>

class IQueryable
{
public:
    virtual bool            queryInterface(QString ifaceName, void** ifacePtr) = 0;
    virtual bool            destroySelf() = 0;
};

#define GCF_DECLARE_QUERYABLE \
public: \
bool queryInterface(QString ifaceName, void** ifacePtr); \
bool destroySelf();

#define GCF_DEFINE_QUERY_TABLE(ClassName) \
bool ClassName::queryInterface(QString ifaceName, void** ifacePtr) \
{ \
    if(!ifacePtr) \
        return false; \
    *ifacePtr = 0; \
    if(ifaceName == #ClassName) \
        *ifacePtr = static_cast<ClassName*>(this);

#define GCF_IMPLEMENTS(IFaceName) \
    if(ifaceName == #IFaceName) \
        *ifacePtr = static_cast<IFaceName*>(this);

#define GCF_END_QUERY_TABLE(ClassName) \
    return *ifacePtr != 0; \
} \
bool ClassName::destroySelf() \
{ \
    delete this; \
    return true; \
}

#define GCF_BEGIN_QUERY_TABLE(ClassName) GCF_DEFINE_QUERY_TABLE(ClassName)

#ifdef Q_CC_MSVC
#pragma warning(disable:4250)
#endif

#endif
