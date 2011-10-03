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

#ifndef I_ABOUT_DATA_H
#define I_ABOUT_DATA_H

#include "IContainer.h"
#include <QImage>

namespace GCF
{
	class AboutPerson
	{
	public:
		AboutPerson() { }
		AboutPerson(QString name, QString email, QString url, QString task) {
			this->Name = name;
			this->EMail = email;
			this->URL = url;
			this->Task = task;
		}
		QString Name;
		QString EMail;
		QString URL;
		QString Task;
	};
}

class IAboutData : virtual public IContainer
{
public:
    virtual QString productName() const = 0;
    virtual QString organization() const = 0;
    virtual QImage programLogo() const = 0;
    virtual QString version() const = 0;
    virtual QString shortDescription() const = 0;
    virtual QString homepage() const = 0;
    virtual QString bugAddress() const = 0;
    virtual const QList<GCF::AboutPerson> authors() const = 0;
    virtual const QList<GCF::AboutPerson> credits() const = 0;
    virtual QString license() const = 0;
    virtual QString copyrightStatement() const = 0;
};

Q_DECLARE_INTERFACE(IAboutData, "com.vcreatelogic.IAboutData/1.0");

#endif

