//-----------------------------------------------------------------------------
// File: Qualifier.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------

#ifndef QUALIFIER_H
#define QUALIFIER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Qualifier 
{
public:

    enum Type
    {
        Address,
        Data,
        Clock,
        Reset,
        Valid,
        Interrupt,
        ClockEnable,
        PowerEnable,
        Opcode,
        Protection,
        FlowControl,
        User,
        Request,
        Response,
        Any
    };

    static const QMap<Qualifier::Type, QString> QUALIFIER_TYPE_STRING;

    Qualifier();

    Qualifier(Qualifier const& other);

    ~Qualifier() = default;

    bool isSet() const;

    void clear();

    bool hasType(Type type) const;

    void setType(Type type);

    QSharedPointer<QList<Type> > getTypes() const;

    void setResetLevel(QString const& level);

    QString getResetLevel() const;

    void setClockEnableLevel(QString const& level);

    QString getClockEnableLevel() const;

    void setPowerEnableLevel(QString const& level);

    QString getPowerEnableLevel() const;

    void setPowerDomainRef(QString const& reference);

    QString getPowerDomainRef() const;

    void setFlowType(QString const& flowType);

    QString getFlowType() const;

    void setUserFlowType(QString const& userFlowType);

    QString getUserFlowType() const;

    void setUserDefined(QString const& userDefined);

    QString getUserDefined() const;

    bool operator==(Qualifier const& other);

    bool operator!=(Qualifier const& other);

    static QString typeToString(Type type);
    
    static Type stringToType(QString const& typeString);

    static QString typeToXMLElementName(Type type);

private:

    QSharedPointer<QList<Type> > types_ = QSharedPointer<QList<Type> >(new QList<Type>());

    QString resetLevel_;

    QString clockEnableLevel_;

    QString powerEnableLevel_;

    QString powerDomainRef_;

    QString flowType_;

    QString userFlowType_;

    QString userDefined_;
    
};

#endif // QUALIFIER_H
