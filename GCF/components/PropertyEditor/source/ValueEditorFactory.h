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

#ifndef VALUE_EDITOR_FACTORY_H
#define VALUE_EDITOR_FACTORY_H


#include "IValueEditorFactory.h"

namespace GCF
{
	namespace Components
	{

		struct ValueEditorFactoryData;
		class ValueEditorFactory : virtual public IValueEditorFactory
		{
		public:
			ValueEditorFactory();
			~ValueEditorFactory();

			// IQueryable implementation
			bool queryInterface(QString iface, void** ifacePtr);
			bool destroySelf();

			// IValueEditorFactory implementation
			void setEnumCreator(IEnumValueEditorCreator* creator);
			IEnumValueEditorCreator* enumCreator();
			void registerCreator(IValueEditorCreator* creator);
			void unregisterCreator(IValueEditorCreator* creator);
			int creatorCount();
			IValueEditorCreator* creator(int index);
			IValueEditorCreator* creator(const char* typeName);
			QWidget* createEditor(const char* typeName, QWidget* parent, IValueEditorCreator** creator);
			QWidget* createEnumEditor(const char* typeName, QObject* object, QWidget* parent, IEnumValueEditorCreator** creator);

		private:
			ValueEditorFactoryData* d;
		};
	}
}

#endif

