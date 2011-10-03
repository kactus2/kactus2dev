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

#ifndef I_EDITABLE_PROPERTIES_H
#define I_EDITABLE_PROPERTIES_H

#include <GCF/IContainer.h>
#include <QString>
#include <QVariant>

class IEditableProperties : virtual public IContainer
{
public:
    virtual int                     propertyCount() = 0;
    virtual QString                 propertyName(int index) = 0;
    virtual QString                 propertyGroup(int index) = 0;
    virtual QVariant                propertyValue(int index) = 0;
    virtual void                    setPropertyValue(int index, QVariant value) = 0;

    // Editor related stuff
    virtual bool                    hasEditor(int index) = 0;
    virtual QWidget*                createEditor(int index) = 0;
    virtual void                    setEditorValue(int index, QWidget* widget, QVariant value) = 0;
    virtual QVariant                editorValue(int index, QWidget* widget) = 0;
    virtual void                    connectEditor(int index, QWidget* widget, QObject* receiver, const char* member) = 0;
    virtual QString                 propertyValueString(int index) = 0;
    virtual QVariant                propertyValueFromString(int index, QString strValue) = 0;

    // must be implemented as signals
    virtual void                    propertyConfigurationChanged() = 0;
};
Q_DECLARE_INTERFACE(IEditableProperties, "com.vcreatelogic.IEditableProperties/1.0");

// Added to support selective load/save of editable properties
class IEditablePropertiesExt : virtual public IContainer
{
public:
    virtual bool                    canLoadSaveProperty(int index) = 0;
};
Q_DECLARE_INTERFACE(IEditablePropertiesExt , "com.vcreatelogic.IEditablePropertiesExt/1.0");

#endif
