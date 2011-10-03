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

#ifndef I_PROJECT_H
#define I_PROJECT_H

#include <GCF/IContainer.h>

class IProjectPart;
class IProject : virtual public IContainer
{
public:
    // Basic project management stuff
    virtual bool newProject() = 0;
    virtual bool loadProject(QString projectFile) = 0;
    virtual bool saveProject(QString projectFile) = 0;
    virtual bool closeProject() = 0;
    virtual bool isModified() = 0;

    // Project part registration
    virtual void addProjectPart(IProjectPart* part) = 0;
    virtual void removeProjectPart(IProjectPart* part) = 0;
    virtual int projectPartCount() = 0;
    virtual IProjectPart* projectPart(int index) = 0;

    // Must be implemented as signals.
    virtual void projectModified(bool val) = 0;
};

Q_DECLARE_INTERFACE(IProject, "com.vcreatelogic.IProjectPart/1.0");

#endif
