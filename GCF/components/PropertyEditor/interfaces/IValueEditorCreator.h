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

#ifndef I_VALUE_EDITOR_CREATOR_H
#define I_VALUE_EDITOR_CREATOR_H

#include <GCF/IQueryable.h>
#include <QVariant>

class QWidget;
class QObject;
class QPainter;
class IValueEditorCreator : virtual public IQueryable
{
public:
	virtual const char*						typeName() = 0;
	virtual QWidget*						createEditor(QWidget* parent) = 0;
	virtual void							connectEditor(QWidget* editor, QObject* receiver, const char* slot) = 0;
	virtual bool							setEditorValue(QWidget* editor, QVariant value) = 0;
	virtual QVariant						editorValue(QWidget* editor) = 0;
	virtual void							closeEditor(QWidget* editor) = 0;
	virtual QString							toString(QVariant value) = 0;
	virtual QVariant						fromString(QString value) = 0;
	virtual bool							paintValue(QPainter* paint, QRect rect, QVariant value) = 0;
};

Q_DECLARE_INTERFACE(IValueEditorCreator, "com.vcreatelogic.IValueEditorCreator/1.0")

#endif

