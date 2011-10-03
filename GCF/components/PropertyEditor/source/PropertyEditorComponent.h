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

#ifndef PROPERTY_EDITOR_COMPONENT_H
#define PROPERTY_EDITOR_COMPONENT_H

#include <GCF/AbstractComponent.h>
#include <GCF/IAboutData.h>

namespace GCF
{
	namespace Components
	{

		struct PropertyEditorComponentData;
		class PropertyEditorComponent : public AbstractComponent, virtual public IAboutData
		{
			Q_OBJECT
			Q_INTERFACES(IAboutData)
			GCF_DECLARE_STD_COMPONENT(PropertyEditorComponent)

		public:
			static PropertyEditorComponent & instance();
			~PropertyEditorComponent();

		protected:
			PropertyEditorComponent();
			QObject* fetchObject(const QString& completeName) const;
			QWidget* fetchWidget(const QString& completeName) const;
			void addChildWidget(QWidget* parent, QWidget* child, const QString& hint);
			void initializeComponent();
			void finalizeComponent();
			QWidget* createWidget(const QString& completeName) const;

		public:
			// IContainer implementation
			QObject* containerObject();

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

		private:
			PropertyEditorComponentData* d;
		};
	}
}

#endif
