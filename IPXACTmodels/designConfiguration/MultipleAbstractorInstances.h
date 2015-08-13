//-----------------------------------------------------------------------------
// File: MultipleAbstractorInstances.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Matches the ipxact:abstractorInstances element.
//-----------------------------------------------------------------------------

#ifndef MULTIPLEABSTRACTORINSTANCES_H
#define MULTIPLEABSTRACTORINSTANCES_H

#include "AbstractorInstance.h"
#include "InterfaceRef.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Matches the ipxact:abstractorInstances element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MultipleAbstractorInstances
{
public:

    /*!
     *  The constructor.
     */
    MultipleAbstractorInstances();

    /*!
     *  Copy constructor.
     */
    MultipleAbstractorInstances(const MultipleAbstractorInstances& other);

    /*!
     *  The destructor.
     */
    ~MultipleAbstractorInstances();

    /*!
     *  Get the value for the isPresent.
     *
     *      @return The value of the isPresent.
     */
    QString getIsPresent() const;

    /*!
     *  Set the value for the isPresent.
     *
     *      @param [in] newIsPresent    The new value for the isPresent.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get interface references.
     *
     *      @return A list of pointers to the interface references.
     */
    QSharedPointer<QList<QSharedPointer<InterfaceRef> > > getInterfaceReferences() const;

    /*!
     *  Set the interface references.
     *
     *      @param [in] newInterfaceReferences  A list of pointers to the interface references.
     */
    void setInterfaceReferences(QSharedPointer<QList<QSharedPointer<InterfaceRef> > > newInterfaceReferences);

    /*!
     *  Get the abstractor instances.
     *
     *      @return A list of pointers to the abstractor instances.
     */
    QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > getAbstractorInstances() const;

    /*!
     *  Set the abstractor instances.
     *
     *      @param [in] newAbstractorInstances  A list of pointers to the abstractor instances.
     */
    void setAbstractorInstances(QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > newAbstractorInstances);

    /*!
     *  Get the VLNVs used in the contained elements.
     *
     *      @return A list of used VLNVs.
     */
    QList<VLNV> getDependantVLNVs() const;

private:

    //! Defines the presence of this element.
    QString isPresent_;

    //! A list of interface references.
    QSharedPointer<QList<QSharedPointer<InterfaceRef> > > interfaceReferences_;

    //! A list of abstractor instances.
    QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > abstractorInstances_;
};

#endif // MULTIPLEABSTRACTORINSTANCES_H
