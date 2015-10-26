//-----------------------------------------------------------------------------
// File: AbstractionType.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.10.2015
//
// Description:
// Implementation of ipxact:abstractionType in bus interface.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTION_TYPE_H
#define ABSTRACTION_TYPE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QSharedPointer>

class ConfigurableVLNVReference;
class PortMap;

//-----------------------------------------------------------------------------
//! Implementation of ipxact:abstractionType in bus interface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractionType
{
public:
    //! Default constructor.
    AbstractionType();

    //! Copy constructor.
    AbstractionType(const AbstractionType &other);

    //! Assignment operator.
    AbstractionType &operator=(const AbstractionType &other);

    void setViewRef(QString const& viewName);

    QString getViewRef() const;

    void setAbstractionRef(QSharedPointer<ConfigurableVLNVReference> abstractionRef);

    QSharedPointer<ConfigurableVLNVReference> getAbstractionRef() const;

    QSharedPointer<QList<QSharedPointer<PortMap> > > getPortMaps() const;

private:

    //! Specifies views where the abstraction type applies.
    QString viewRef_;

    //! Reference to an abstraction description for this abstractor instance.
    QSharedPointer<ConfigurableVLNVReference> abstractionRef_;

    //! Describes the mapping between the abstraction definition's logical ports and components physical ports.
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps_;
};

#endif // ABSTRACTION_TYPE_H
