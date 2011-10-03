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

#ifndef I_PROJECT_EXT_H
#define I_PROJECT_EXT_H

#include <GCF/IContainer.h>

#include <QStringList>

class IProjectExt : virtual public IContainer
{
public:
    virtual QString currentProjectFileName() const = 0;
    virtual QString currentProjectFilePath() const = 0;
    virtual QStringList recentFilesList() const = 0;
    virtual QString defaultProjectPath() const = 0;
    virtual QString projectRootTagName() const = 0;
    virtual QString projectFileSuffix() const = 0;

    // must be implemented as signals
    virtual void currentProjectFileNameChanged(const QString& newName) = 0;
    virtual void projectLoadBegin() = 0;
    virtual void projectLoadEnd() = 0;
    virtual void projectCleared() = 0;
};

Q_DECLARE_INTERFACE(IProjectExt, "com.vcreatelogic.IProjectExt/1.0");


#endif


