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

#ifndef PROPERTY_EDITOR_H
#define PROPERTY_EDITOR_H

#include <GCF/Common.h>
#include "IPropertyEditor.h"
#include "IQObjectLoadSave.h"
#include "IDefaultPropertyManager.h"

#include <QTreeWidget>


namespace GCF
{
	namespace Components
	{
		struct PropertyEditorData;
		class PropertyEditor : public QTreeWidget,
								virtual public IPropertyEditor,
								virtual public IDefaultPropertyManager,
								virtual public IQObjectLoadSave
		{
			Q_OBJECT
			Q_INTERFACES(IPropertyEditor IDefaultPropertyManager IQObjectLoadSave)

		public:
			PropertyEditor(QWidget* parent=0);
			~PropertyEditor();

			// IContainer implementation
			QObject* containerObject();

			// IPropertyEditor implementation
			Q_INVOKABLE void setObject(QObject* object, bool recursive=false);
			QObject* object();
			int propertyCount();
			IProperty* property(int index);
			void setEditorFactory(IValueEditorFactory* factory);
			IValueEditorFactory* editorFactory();

		signals:
			void propertyValueChanged(QObject* obj, QString propName, QVariant oldVal, QVariant newVal);

		protected slots:
			void on_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* old);
			void on_editor_valueEdited();
            void on_editor_destroyed();
			void on_propertyChanged(const QString& name);
			void refresh();

		protected:
			bool eventFilter(QObject* object, QEvent* event);
			void addPropertiesFrom(QObject* object);
			void drawBranches(QPainter* p, const QRect & rect, const QModelIndex &) const;
			void mousePressEvent(QMouseEvent* me);

		public:
			// IDefaultPropertyManager implementation
			void setDefaultProperty(QObject* object, IDefaultProperty* prop);
			IDefaultProperty* defaultProperty(QObject* object);

		protected slots:
			void on_object_destroyed(QObject* obj);
			void on_object_propertyConfigurationChanged();

		public:
			// IQObjectLoadSave
			bool saveProperties(QObject* object, QDomDocument doc, QDomElement parentE);
			bool loadProperties(QObject* object, QDomDocument doc, QDomElement parentE);

		private:
			PropertyEditorData* d;
		};

		struct DefaultPropertyData;
		class DefaultProperty : virtual public IDefaultProperty
		{
		public:
			DefaultProperty(QObject* object);
			~DefaultProperty();

			void updateDefaultValues();

			// IQueryable implementation
			bool queryInterface(QString ifaceName, void** ifacePtr);
			bool destroySelf();

			// IDefaultProperty implementation
			QObject* object();
			bool hasDefaultValue(QString propName);
			QVariant defaultValue(QString propName);

		private:
			DefaultPropertyData* d;
		};
	}
}

#endif
