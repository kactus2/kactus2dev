//-----------------------------------------------------------------------------
// File: ClearboxElement.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.06.2026
//
// Description:
// Describes the ipxact:clearboxElement/whiteboxElement elements.
//-----------------------------------------------------------------------------

#pragma once

#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/BooleanValue.h>
#include <IPXACTmodels/common/Parameter.h>

class ClearboxElement : public NameGroup, public Extendable
{
public:

    enum class Type
    {
        PIN = 0,
        SIGNAL,
        INTERFACE,
        UKNOWN
    };
    
    explicit ClearboxElement(QString const& name = QString());

    ClearboxElement(ClearboxElement const& other);
    ClearboxElement& operator=(ClearboxElement const& other);

    ~ClearboxElement() = default;

    QString getIsPresent() const;
    void setIsPresent(QString const& newIsPresent);

    Type getType() const;
    void setType(Type type);

    QString getDriveable() const;
    void setDriveable(bool value);

    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters);

    static QString typeToString(Type type);
    static Type stringToType(QString const& typeStr);

private:

    QString isPresent_;
    
    Type type_;

    BooleanValue driveable_;

    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_ =
        QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> >());
};
