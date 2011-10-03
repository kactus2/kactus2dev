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

#ifndef PROPERTY_H
#define PROPERTY_H

#include "IProperty.h"
#include <QMetaEnum>

class IValueEditorCreator;
class IPropertyEditorNotifications;

namespace GCF
{
	namespace Components
	{
		struct PropertyData;
		class Property : virtual public IProperty
		{
		public:
			Property(QObject* object, QString propName);
			~Property();

            IPropertyEditorNotifications* editorNotifications();

			void setValueEditorCreator(IValueEditorCreator* creator);
			IValueEditorCreator* valueEditorCreator();

			void setEnumStruct(QMetaEnum enumStruct);
			bool isEnum();
			QMetaEnum enumStruct();

			void setEditableProperties(IEditableProperties* editProps);
			void setEditbalePropertyIndex(int index);

			QString propertyNameQString();

			// IQueryable implementation
			bool queryInterface(QString iface, void** ifacePtr);
			bool destroySelf();

			// IProperty implementation
			QObject* propertyObject();
			const char* propertyName();
			QVariant propertyValue();
			bool setPropertyValue(QVariant value);
			IEditableProperties* editableProperties();
			int editablePropertyIndex();

		private:
			PropertyData* d;
		};
	}
}

Q_DECLARE_METATYPE(GCF::Components::Property*)

#endif

