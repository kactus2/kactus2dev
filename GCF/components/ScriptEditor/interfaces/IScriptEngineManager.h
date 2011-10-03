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

#ifndef I_SCRIPT_ENGINE_MANAGER_H
#define I_SCRIPT_ENGINE_MANAGER_H

#include <GCF/IContainer.h>

class IScriptDataType;
class QScriptEngine;
class IScriptEngineManager : virtual public IContainer
{
public:
    // Adding / Remove objects from the script environment.
    virtual void addScriptableObject(QObject* object, QString name, bool excludeChildren=true) = 0;
    virtual void removeScriptableObject(QObject* object) = 0;
    virtual void removeScriptableObject(QString name) = 0;
    virtual void changeScriptableObjectName(QObject* object, QString newName) = 0;

    // Engine associations.
    virtual QScriptEngine* engine() = 0;
    virtual void resetEngine() = 0;

    // Script associations.
    virtual void setScript(QString script) = 0;
    virtual QString script() = 0;
    virtual void executeScript() = 0;

    // Adding custom types.
    virtual void addScriptDataType(IScriptDataType* type) = 0;
    virtual void removeScriptDataType(IScriptDataType* type) = 0;
    virtual int scriptDataTypeCount() = 0;
    virtual IScriptDataType* scriptDataType(int index) = 0;
};

Q_DECLARE_INTERFACE(IScriptEngineManager, "com.vcreatelogic.IScriptEngineManager/1.0");

#endif

