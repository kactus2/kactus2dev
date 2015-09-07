//-----------------------------------------------------------------------------
// File: DesignInstantiation.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2015
//
// Description:
// Describes the ipxact:designInstantiation element.
//-----------------------------------------------------------------------------

#ifndef DESIGNINSTANTIATION_H
#define DESIGNINSTANTIATION_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Describes the ipxact:designInstantiation element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignInstantiation : public NameGroup, public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            Name of the instantiation.
     *      @param [in] displayName     Display name of the instantiation.
     *      @param [in] description     Description of the instantiation.
     */
    DesignInstantiation(QString const& name = QString(), QString const& displayName = QString(),
        QString const& description = QString());

    /*!
     *  The copy constructor.
     */
    DesignInstantiation(const DesignInstantiation& other);

    /*!
     *  The assignment operator.
     */
    DesignInstantiation& operator=(const DesignInstantiation& other);

    /*!
     *  The destructor.
     */
    ~DesignInstantiation();

    /*!
     *  Get the design reference.
     *
     *      @return Pointer to the design referenced by the instantiation.
     */
    QSharedPointer<ConfigurableVLNVReference> getDesignReference() const;

    /*!
     *  Set the design reference.
     *
     *      @param [in] newDesignReference  The referenced design.
     */
    void setDesignReference(QSharedPointer<ConfigurableVLNVReference> newDesignReference);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design referenced by this instantitation.
    QSharedPointer<ConfigurableVLNVReference> designRef_;
};

#endif // DESIGNINSTANTIATION_H