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

#ifndef I_CLIPBOARD_H
#define I_CLIPBOARD_H

#include <GCF/IContainer.h>
#include <QIcon>
#include <QVariant>

struct ClipboardData
{
    ClipboardData(const QIcon& i, const QVariant& d, const QString& desc)
        : Icon(i), Data(d), Description(desc) { }
    ClipboardData(const ClipboardData& other)
        : Icon(other.Icon), Data(other.Data), Description(other.Description) { }
    ClipboardData() { }

    QIcon Icon;
    QVariant Data;
    QString Description;
};

class QAbstractListModel;
class IClipboard : virtual public IContainer
{
public:
    // Methods to add/remove contexts
    virtual void addContext(const QString& contextName) = 0;
    virtual void removeContext(const QString& contextName) = 0;
    virtual QString context(int index) const = 0;
    virtual int contextCount() const = 0;

    // Methods to activate a context
    virtual void activateContext(const QString& name) = 0;
    virtual void activateContext(int contextIndex) = 0;
    virtual void activateDefaultContext() = 0;

    // Methods to figure out the current context
    virtual QString activeContextName() const = 0;
    virtual int activeContextIndex() const = 0;
    virtual bool isDefaultContextActive() const = 0;

    // Methods to push/pop data into the current context
    virtual void addData(const ClipboardData& data) = 0;
    virtual bool removeDataAt(int index) = 0;
    virtual bool removeAll() = 0;
    virtual int dataCount() const = 0;
    virtual ClipboardData data(int index) const = 0;

    // Convinience model for using in views
    virtual QAbstractListModel* activeClipboardModel() const = 0;

    // Must be implemented as signals
    virtual void activeContextChanged(const QString& ctxName) = 0;
    virtual void contextAdded(const QString& name) = 0;
    virtual void contextRemoved(const QString& name) = 0;
    virtual void dataAdded(const ClipboardData& data) = 0;
    virtual void dataRemoved(int index) = 0;
    virtual void dataCleared() = 0;
};

Q_DECLARE_INTERFACE(IClipboard, "com.vcreatelogic.IClipboard/1.0");

#include <QMetaType>
Q_DECLARE_METATYPE(ClipboardData)

#endif
