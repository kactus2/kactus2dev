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
    
    /*!
     *  Default constructor.
     */
    AbstractionType();

    /*!
     *  The destructor.
     */
    virtual ~AbstractionType();

    /*!
     *  Copy constructor.
     */
    AbstractionType(const AbstractionType &other);

    /*!
     *  Assignment operator.
     */
    AbstractionType &operator=(const AbstractionType &other);

    /*!
     *  Set the view references.
     *
     *      @param [in] newViewReferences   List containing the new view references.
     */
    void setViewReferences(QSharedPointer<QStringList> newViewReferences);

    /*!
     *  Get the view references.
     *
     *      @return List of the referenced views.
     */
    QSharedPointer<QStringList> getViewReferences() const;

    /*!
     *  Set the abstraction reference.
     *
     *      @param [in] abstractionRef  The referenced abstraction definition.
     */
    void setAbstractionRef(QSharedPointer<ConfigurableVLNVReference> abstractionRef);

    /*!
     *  Get the referenced abstraction definition.
     *
     *      @return The referenced abstraction definition.
     */
    QSharedPointer<ConfigurableVLNVReference> getAbstractionRef() const;

    /*!
     *  Get the port maps.
     *
     *      @return A list of port maps.
     */
    QSharedPointer<QList<QSharedPointer<PortMap> > > getPortMaps() const;

private:

    void copyViewReferences(QSharedPointer<QStringList> newViewReferences);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Specifies views where the abstraction type applies.
    QSharedPointer<QStringList> viewReferences_;

    //! Reference to an abstraction description for this abstractor instance.
    QSharedPointer<ConfigurableVLNVReference> abstractionRef_;

    //! Describes the mapping between the abstraction definition's logical ports and components physical ports.
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps_;
};

#endif // ABSTRACTION_TYPE_H
