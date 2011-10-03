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

#ifndef I_HELP_SYSTEM_H
#define I_HELP_SYSTEM_H

#include <GCF/IContainer.h>
#include <QUrl>

class QHelpEngine;
class IHelpSystem : virtual public IContainer
{
public:
    virtual void setHelpCollectionFile(const QString& cf) = 0;
    virtual QString helpCollectionFile() const = 0;

    virtual void registerDocumentation(const QString& qchFile) = 0;
    virtual void unregisterDocumentation(const QString& namespaceName) = 0;
    virtual QStringList registeredDocumentations() = 0;

    virtual void showDocumentation(const QUrl& url) = 0;
    virtual QUrl currentDocumentationUrl() const = 0;
    virtual void showDocumentationForKeyword(const QString& keyword) = 0;

    virtual QHelpEngine* helpEngine() const = 0;
};

Q_DECLARE_INTERFACE(IHelpSystem, "com.vcreatelogic.IHelpSystem/1.0");

#endif


