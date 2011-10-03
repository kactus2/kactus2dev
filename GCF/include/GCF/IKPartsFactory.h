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

#ifndef I_KPARTS_FACTORY_H
#define I_KPARTS_FACTORY_H

#include "Common.h"
#include "IContainer.h"

class IKPartsFactory : virtual public IContainer
{
public:
    enum PartCreationType
    {
        UnknownType,
        MimeType,
        ServiceType,
        LibraryName
    };

    virtual KParts::Part* createPart(const QString& completeName, 
                                     PartCreationType type, 
                                     const QString& typeStr, 
                                     bool readOnly=true) = 0;

    virtual void finalizePart(KParts::Part* part, 
                              const QString& completeName, 
                              PartCreationType type, 
                              const QString& typeStr, 
                              bool readOnly=true) = 0;

    virtual void destroyPart(KParts::Part* part, 
                             const QString& completeName, 
                             PartCreationType type, 
                             const QString& typeStr, 
                             bool readOnly=true) = 0;
};
Q_DECLARE_INTERFACE(IKPartsFactory, "com.vcreatelogic.IKPartsFactory/1.0")

#endif
