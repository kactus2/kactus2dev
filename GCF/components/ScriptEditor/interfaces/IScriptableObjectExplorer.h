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

#ifndef I_SCRIPTABLE_OBJECT_EXPLORER_H
#define I_SCRIPTABLE_OBJECT_EXPLORER_H

#include <GCF/IContainer.h>
#include <QPixmap>

class IScriptEngineManager;
class IScriptableObjectExplorer : virtual public IContainer
{
public:
    virtual IScriptEngineManager* scriptEngine() = 0;
    virtual void updateExplorer() = 0;
    virtual void highlightScriptableObject(QObject* object) = 0;

    // must be implemented as signals
    virtual void objectDoubleClicked(QObject* object, QString objectName) = 0;
    virtual void signalDoubleClicked(QObject* object, QString objectName, QString event, QString signal) = 0;
    virtual void propertyDoubleClicked(QObject* object, QString objectName, QString propertyName, QString propDataType) = 0;
    virtual void methodDoubleClicked(QObject* object, QString objectName, QString method, QString methodSignature) = 0;
};

Q_DECLARE_INTERFACE(IScriptableObjectExplorer, "com.vcreatelogic.IScriptableObjectExplorer/1.0");

#endif

