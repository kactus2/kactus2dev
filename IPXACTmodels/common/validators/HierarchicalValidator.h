//-----------------------------------------------------------------------------
// File: HierarchicalValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 31.07.2024
//
// Description:
// Parent validator for validators used in component editor to be able to mark child items as invalid.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICALVALIDATOR_H
#define HIERARCHICALVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QMap>
#include <QSharedPointer>

class NameGroup;

class IPXACTMODELS_EXPORT HierarchicalValidator
{
public:	

    HierarchicalValidator() = default;

    /*!
     *	Check and mark validity of child items. Checks for name uniqueness.
     *  
     *      @param [in] children     The items to be validated.
     *	    
     * 	    @return True, if all child items are valid amongst themselves, false otherwise.
     */
    virtual bool childrenHaveUniqueNames(QSharedPointer<QList<QSharedPointer<NameGroup> > > children);

    void setChildValidator(QSharedPointer<HierarchicalValidator> childValidator);

    bool validComparedToSiblings(QSharedPointer<NameGroup> currentChildItem) const;

    void setChildItemValidity(QSharedPointer<NameGroup> childItem, bool isValid);

    bool childItemIsValid(QSharedPointer<NameGroup> childItem) const;

    void clearChildItemValidities();

protected:

    //! The validity of the child items to the current validator. E.g. for MemoryMapValidator these 
    //  are the address block or subspace map validities.
    QMap<QSharedPointer<NameGroup>, bool> childItemValidities_;

    //! The validator of the next level in the hierarchy. 
    QSharedPointer<HierarchicalValidator > childValidator_;
};


#endif // HIERARCHICALVALIDATOR_H
