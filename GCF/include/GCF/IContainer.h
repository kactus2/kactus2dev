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

#ifndef I_CONTAINER_H
#define I_CONTAINER_H

#include <QString>
#include <QObject>

class QObject;
class IContainer
{
public:
    virtual QObject*        containerObject() = 0;
};

#ifdef Q_CC_MSVC
#pragma warning(disable:4250)
#endif

// Q_DECLARE_INTERFACE(IContainer, "com.vcreatelogic.IContainer/1.0")

#endif
