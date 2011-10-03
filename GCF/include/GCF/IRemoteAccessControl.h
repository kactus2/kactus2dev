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

#ifndef I_REMOTE_ACCESS_CONTROL_H
#define I_REMOTE_ACCESS_CONTROL_H

#include "IContainer.h"
#include <QMetaObject>

namespace GCF { class RemoteApp; }

// This interface can be implemented by IComponent subclasses to control
// access to objects contained in it.
class IComponentRemoteAccessControl : virtual public IContainer
{
public:
    // Control global component access. The master on/off switch
    virtual bool isAllowComponentAccess(GCF::RemoteApp* remoteApp) const = 0;

    // Control per-object access
    virtual bool isAllowObjectAccess(QObject* object, GCF::RemoteApp* remoteApp) const = 0;
};
Q_DECLARE_INTERFACE(IComponentRemoteAccessControl, "com.vcreatelogic.IComponentRemoteAccessControl/1.0");


// This interface can be implemented by any exposed QObject to help figure out what
// aspects of the object can be remotely accessed
class IObjectRemoteAccessControl : virtual public IContainer
{
public:
    // Control per-object access
    virtual bool isAllowObjectAccess(GCF::RemoteApp* remoteApp) const = 0;

    // Control per-object method access
    virtual bool isAllowObjectMethodInvoke(int methodIdx, GCF::RemoteApp* remoteApp) const = 0;

    // Control per-object property access
    virtual bool isAllowObjectSetProperty(int propIdx, GCF::RemoteApp* remoteApp) const = 0;
    virtual bool isAllowObjectGetProperty(int propIdx, GCF::RemoteApp* remoteApp) const = 0;

    // Control per-object signal access
    virtual bool isAllowObjectSignalConnect(int signalIdx, GCF::RemoteApp* remoteApp) const = 0;
    virtual bool isAllowObjectSignalDelivery(int signalIdx, GCF::RemoteApp* remoteApp) const = 0;
};
Q_DECLARE_INTERFACE(IObjectRemoteAccessControl, "com.vcreatelogic.IObjectRemoteAccessControl/1.0");

#endif

