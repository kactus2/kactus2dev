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

#include <QHash>
#include <QSharedPointer>

class NameGroup;

class IPXACTMODELS_EXPORT HierarchicalValidator
{
public:	

    HierarchicalValidator() = default;

    /*!
     *	Check and mark validity of child/sibling items. Checks for name uniqueness.
     *  
     *    @param [in] children     The items to be validated.
     *	    
     * 	    @return True, if all child items are valid amongst themselves, false otherwise.
     */
    virtual bool childrenHaveUniqueNames(QSharedPointer<QList<QSharedPointer<NameGroup> > > children);

    /*!
     *	Set the validator to be used when validating child/sibling items.
     *  
     *    @param [in] childValidator     The validator to set.
     */
    void setChildValidator(QSharedPointer<HierarchicalValidator> childValidator);

    /*!
     *	Checks if the item is valid amongst its siblings. The item validity can be set by the parent validator
     *  on a higher level in the hierarchy and is stored in the childItemValidities map.
     *  
     *    @param [in] currentChildItem     The item to check.
     *	    
     * 	    @return True, if the item is valid, otherwise false.
     */
    bool validComparedToSiblings(QSharedPointer<NameGroup> currentChildItem) const;

    /*!
     *	Set the validitiy of an item. Called by a higher level validator.
     *  
     *    @param [in] childItem     The item whose validity to set.
     *    @param [in] isValid       The validity status to set.
     */
    void setChildItemValidity(QSharedPointer<NameGroup> childItem, bool isValid);

    /*!
     *	Clear the item validities.
     */
    void clearChildItemValidities();

protected:

    //! The validity of the sibling items of the current validator. E.g. for MemoryMapValidator these 
    //  are the validities of the other memory maps.
    QHash<QSharedPointer<NameGroup>, bool> childItemValidities_;

    //! The validator of the next level in the hierarchy. 
    QSharedPointer<HierarchicalValidator > childValidator_;
};


#endif // HIERARCHICALVALIDATOR_H
