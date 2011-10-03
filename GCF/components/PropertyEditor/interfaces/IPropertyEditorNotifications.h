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

#ifndef I_PROPERTY_EDITOR_NOTIFICATIONS_H
#define I_PROPERTY_EDITOR_NOTIFICATIONS_H

#include <GCF/IContainer>

class IProperty;
class QWidget;

class IPropertyEditorNotifications : virtual public IContainer
{
public:
    virtual void editorCreatedNotification(IProperty* property, QWidget* editor) = 0;
    virtual void editorEditedNotification(IProperty* property, QWidget* editor) = 0;
    virtual void editorAboutToDestroyNotification(IProperty* property, QWidget* editor) = 0;
};
Q_DECLARE_INTERFACE(IPropertyEditorNotifications, "com.vcreatelogic.IPropertyEditorNotifications/1.0")

#endif
