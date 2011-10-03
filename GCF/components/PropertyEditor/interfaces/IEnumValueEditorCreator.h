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

#ifndef I_ENUM_VALUE_EDITOR_CREATOR_H
#define I_ENUM_VALUE_EDITOR_CREATOR_H

#include "IValueEditorCreator.h"

#include <QMetaEnum>
class IEnumValueEditorCreator : virtual public IValueEditorCreator
{
public:
	virtual void			setCurrentEnum(QMetaEnum enumStruct) = 0;
	virtual void 			initialize(QWidget* editor, QMetaEnum enumStruct) = 0;
};

Q_DECLARE_INTERFACE(IEnumValueEditorCreator, "com.vcreatelogic.IEnumValueEditorCreator/1.0")

#endif

