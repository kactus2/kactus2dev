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

#ifndef I_PROPERTY_EDITOR_H
#define I_PROPERTY_EDITOR_H

#include <GCF/IContainer.h>

class IValueEditorFactory;
class IProperty;
class IPropertyEditor : virtual public IContainer
{
public:
    // Establish which object to show the properties of.
    virtual void                setObject(QObject* object, bool recursive=false) = 0;
    virtual QObject*            object() = 0;

    // Methods to query property information
    virtual int                 propertyCount() = 0;
    virtual IProperty*            property(int index) = 0;

    // Establish the editor factory.
    virtual void                setEditorFactory(IValueEditorFactory* factory) = 0;
    virtual    IValueEditorFactory* editorFactory() = 0;

    // Must be implemented as a signal.
    virtual void                propertyValueChanged(QObject* obj, QString propName, QVariant oldVal, QVariant newVal) = 0;
};

Q_DECLARE_INTERFACE(IPropertyEditor, "com.vcreatelogic.IPropertyEditor/1.0")

#endif
