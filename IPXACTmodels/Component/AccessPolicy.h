//-----------------------------------------------------------------------------
// File: AccessPolicy.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Describes the ipxact:accessPolicy element.
//-----------------------------------------------------------------------------

#ifndef ACCESSPOLICY_H
#define ACCESSPOLICY_H

#include "ModeReference.h"

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/AccessTypes.h>
#include <IPXACTmodels/common/Extendable.h>

class IPXACTMODELS_EXPORT AccessPolicy : public Extendable
{
public:

    AccessPolicy();

    AccessPolicy(AccessPolicy const& other);
    
    virtual ~AccessPolicy() = default;

    /*!
     *	Get the mode references of the field access policy.
     *	    
     * 	    @return A pointer to the list of mode references.
     */
    QSharedPointer<QList<QSharedPointer<ModeReference> > > getModeReferences() const;
    
    /*!
     *	Set the mode references of the field access policy.
     *  
     *      @param [in] modeRefs     The mode references to set.
     */
    void setModeReferences(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs);

    /*!
     *	Get the field access policy access value.
     *  
     * 	    @return The access value.
     */
    AccessTypes::Access getAccess() const;

    /*!
     *	Set the access value.
     *  
     *      @param [in] newAccess     The new access value.
     */
    void setAccess(AccessTypes::Access newAccess);

private:

    //! The operating modes for which this field access policy is active.
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeReferences_ =
        QSharedPointer<QList<QSharedPointer<ModeReference> > >(new QList<QSharedPointer<ModeReference> >());

    //! Contains the access type of the field.
    AccessTypes::Access access_ = AccessTypes::ACCESS_COUNT;

};

#endif // ACCESSPOLICY_H
