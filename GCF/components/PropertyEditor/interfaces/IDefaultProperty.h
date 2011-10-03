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

#ifndef I_DEFAULT_PROPERTY_H
#define I_DEFAULT_PROPERTY_H

#include <GCF/IQueryable.h>
#include <QString>
#include <QVariant>

class IDefaultProperty : virtual public IQueryable
{
public:
    virtual QObject* object() = 0;
    virtual bool hasDefaultValue(QString propName) = 0;
    virtual QVariant defaultValue(QString propName) = 0;
};

#endif


