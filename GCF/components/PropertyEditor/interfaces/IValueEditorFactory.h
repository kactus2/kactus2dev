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

#ifndef I_VALUE_EDITOR_FACTORY_H
#define I_VALUE_EDITOR_FACTORY_H

#include <GCF/IQueryable.h>

class QWidget;
class QObject;
class IValueEditorCreator;
class IEnumValueEditorCreator;
class IValueEditorFactory : virtual public IQueryable
{
public:
	virtual void				setEnumCreator(IEnumValueEditorCreator* creator) = 0;
	virtual IEnumValueEditorCreator* enumCreator() = 0;

	virtual void				registerCreator(IValueEditorCreator* creator) = 0;
	virtual void				unregisterCreator(IValueEditorCreator* creator) = 0;
	virtual int					creatorCount() = 0;
	virtual IValueEditorCreator* creator(int index) = 0;
	virtual IValueEditorCreator* creator(const char* typeName) = 0;

	virtual QWidget*			createEditor(const char* typeName, QWidget* parent, IValueEditorCreator** creator) = 0;
	virtual QWidget*			createEnumEditor(const char* typeName, QObject* object, QWidget* parent, IEnumValueEditorCreator** creator) = 0;
};

Q_DECLARE_INTERFACE(IValueEditorFactory, "com.vcreatelogic.IValueEditorFactory/1.0")

#endif

