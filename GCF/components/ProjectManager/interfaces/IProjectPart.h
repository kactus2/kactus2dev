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

#ifndef I_PROJECT_PART_H
#define I_PROJECT_PART_H

#include <GCF/IContainer.h>
#include <QDomElement>
#include <QDomDocument>

class IProjectPart : virtual public IContainer
{
public:
    virtual QString projectPartID() = 0;

    // Basic project management stuff
    virtual bool newProjectPart() = 0;
    virtual bool loadProjectPart(QDomDocument doc, QDomElement partE) = 0;
    virtual bool initializeProjectPart() = 0; // called after all project parts are loaded.
    virtual bool saveProjectPart(QDomDocument doc, QDomElement partParentE) = 0;
    virtual bool closeProjectPart() = 0;

    // must be implemented as signals.
    virtual void projectPartModified() = 0;
};

Q_DECLARE_INTERFACE(IProjectPart, "com.vcreatelogic.IProjectPart/1.0");

#endif
