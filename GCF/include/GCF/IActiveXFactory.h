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

#ifndef I_ACTIVEX_FACTORY_H
#define I_ACTIVEX_FACTORY_H

#include "Common.h"
#include "IContainer.h"

class IActiveXFactory : virtual public IContainer
{
public:
    virtual QAxWidget* createActiveXWidget(const QString& completeName, const QString& control) = 0;
	virtual void finalizeActiveXWidget(QAxWidget* widget, const QString& completeName, const QString& control) = 0;
    virtual void destroyActiveXWidget(QAxWidget* widget, const QString& completeName, const QString& control) = 0;
};
Q_DECLARE_INTERFACE(IActiveXFactory, "com.vcreatelogic.IActiveXFactory/1.0")

#endif
