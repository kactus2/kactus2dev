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

#ifndef I_PROPERTY_H
#define I_PROPERTY_H

#include <GCF/IQueryable.h>
#include <QVariant>

class IEditableProperties;

class QObject;
class IProperty : virtual public IQueryable
{
public:
    virtual QObject*                propertyObject() = 0;
    virtual const char*             propertyName() = 0;
    virtual QVariant                propertyValue() = 0;
    virtual bool                    setPropertyValue(QVariant value) = 0;
    virtual IEditableProperties*    editableProperties() = 0;
    virtual int                     editablePropertyIndex() = 0;
};

Q_DECLARE_INTERFACE(IProperty, "com.vcreatelogic.IProperty/1.0");

#endif

