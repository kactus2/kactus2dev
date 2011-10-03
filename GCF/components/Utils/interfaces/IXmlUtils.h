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

#ifndef I_XML_UTILS_H
#define I_XML_UTILS_H

#include <GCF/IContainer.h>
#include <QDomDocument>
#include <QDomElement>
#include <QPixmap>
#include <QList>

class IXmlUtils : virtual public IContainer
{
public:
	virtual QList<QDomElement> findElements(QDomElement parentE, QString tagName) = 0;
	virtual QDomElement findElement(QDomElement parentE, QString tagName) = 0;
	virtual bool savePixmap(QDomDocument doc, QDomElement element, QPixmap pixmap) = 0;
	virtual bool loadPixmap(QDomElement element, QPixmap& pixmap) = 0;
	virtual bool parseDom(QDomDocument doc, QStringList tagNames, QMap<QString, QList<QDomElement> > & elementMap) = 0;
    virtual bool saveData(QDomDocument doc, QDomElement element, QByteArray array) = 0;
    virtual bool loadData(QDomElement element, QByteArray& array) = 0;
};

Q_DECLARE_INTERFACE(IXmlUtils, "com.vcreatelogic.IXmlUtils/1.0");

#endif

