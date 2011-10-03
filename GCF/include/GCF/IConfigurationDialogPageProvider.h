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

#ifndef I_CONFIGURATION_DIALOG_PAGE_PROVIDER_H
#define I_CONFIGURATION_DIALOG_PAGE_PROVIDER_H

#include "IContainer.h"
#include <QString>
#include <QObject>
#include <QIcon>

class IConfigurationDialogPageProvider : virtual public IContainer
{
public:
    virtual QString pageTitle() const = 0;
    virtual QIcon pageIcon() const = 0;
    virtual QWidget* createPage(QWidget* parent) = 0;
    virtual void savePage(QWidget* page) = 0;
};

Q_DECLARE_INTERFACE(IConfigurationDialogPageProvider, "com.vcreatelogic.IConfigurationDialogPageProvider/1.0")

#endif
