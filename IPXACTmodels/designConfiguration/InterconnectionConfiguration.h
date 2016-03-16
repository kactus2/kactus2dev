//-----------------------------------------------------------------------------
// File: InterconnectionConfiguration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.08.2015
//
// Description:
// Matches the ipxact:interconnectionConfiguration element.
//-----------------------------------------------------------------------------

#ifndef INTERCONNECTIONCONFIGURATION_H
#define INTERCONNECTIONCONFIGURATION_H

#include "MultipleAbstractorInstances.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Matches the ipxact:interconnectionConfiguration element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InterconnectionConfiguration
{
public:

    /*!
     *  The constructor.
     */
    InterconnectionConfiguration();

    /*!
     *  The copy constructor.
     */
    InterconnectionConfiguration(const InterconnectionConfiguration& other);

    /*!
     *  The destructor.
     */
    ~InterconnectionConfiguration();

    /*!
     *  Get the isPresent value.
     *
     *      @return The isPresent value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the isPresent value.
     *
     *      @param [in] newIsPresent    The new value for the isPresent.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get the referenced interconnection.
     *
     *      @return The name of the referenced interconnection.
     */
    QString getInterconnectionReference() const;

    /*!
     *  Set the referenced interconnection.
     *
     *      @param [in] newInterconnecionReference  The name of the referenced interconnection.
     */
    void setInterconnectionReference(QString const& newInterconnecionReference);

    /*!
     *  Get a list of multipleAbstractorInstances.
     *
     *      @return A list of multipleAbstractorInstances.
     */
    QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > getAbstractorInstances() const;

    /*!
     *  Set the multipleAbstractorInstances.
     *
     *      @param [in] newAbstractorInstances  A new list of multipleAbstractorInstances.
     */
    void setAbstractorInstances(
        QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > newAbstractorInstances);

    /*!
     *  Get the VLNVs used in this element.
     *
     *      @return A list of the used VLNV elements.
     */
    QList<VLNV> getDependantVLNVs() const;

private:

    //! Defines whether the element is present in the document (default 1).
    QString isPresent_;

    //! A reference to the name of a design interconnection.
    QString interconnectionRef_;

    //! List of multiple abstractor elements.
    QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > abstractorInstances_;
};

#endif // INTERCONNECTIONCONFIGURATION_H
