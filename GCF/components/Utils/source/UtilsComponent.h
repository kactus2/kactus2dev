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

#ifndef UTILS_COMPONENT_H
#define UTILS_COMPONENT_H

#include "IXmlUtils.h"
#include "IMathUtils.h"
#include <GCF/AbstractComponent.h>
#include <GCF/IAboutData.h>

namespace GCF
{
	namespace Components
	{
		struct UtilsComponentData;
		class UtilsComponent : public AbstractComponent,
								virtual public IXmlUtils,
								virtual public IMathUtils,
								virtual public IAboutData
		{
			Q_OBJECT
			Q_INTERFACES(IXmlUtils IMathUtils IAboutData)
			GCF_DECLARE_STD_COMPONENT(UtilsComponent)

		public:
			static UtilsComponent& instance();
			~UtilsComponent();

			// IContainer implementation
			QObject* containerObject();

			// IXmlUtils
			QList<QDomElement> findElements(QDomElement parentE, QString tagName);
			QDomElement findElement(QDomElement parentE, QString tagName);
			bool savePixmap(QDomDocument doc, QDomElement element, QPixmap pixmap);
			bool loadPixmap(QDomElement element, QPixmap& pixmap);
			bool parseDom(QDomDocument doc, QStringList tagNames, QMap<QString, QList<QDomElement> > & elementMap);
			bool saveData(QDomDocument doc, QDomElement element, QByteArray array);
			bool loadData(QDomElement element, QByteArray& array);

			// IMathUtils
			double toRadians(double angle);
			double toDegrees(double radians);
			double sinAngle(double angle);
			double cosAngle(double angle);
			double tanAngle(double angle);
			double aSinAngle(double val);
			double aCosAngle(double val);
			double aTanAngle(double val);
			double randomNumber(double min, double max);

			// IAboutData implementation
			QString productName() const;
			QString organization() const;
			QImage programLogo() const;
			QString version() const;
			QString shortDescription() const;
			QString homepage() const;
			QString bugAddress() const;
			const QList<AboutPerson> authors() const;
			const QList<AboutPerson> credits() const;
			QString license() const;
			QString copyrightStatement() const;

		protected:
			UtilsComponent();
			QObject* fetchObject(const QString& completeName) const;
			void initializeComponent();
			void finalizeComponent();

		private:
			UtilsComponentData* d;
		};
	}
}

#endif

