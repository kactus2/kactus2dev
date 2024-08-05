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

/*
 *  Data necessary for validating child items. Can and should be subclassed for validators needing more data for 
 *  validating its children.
 */
struct IPXACTMODELS_EXPORT ValidationData
{
    virtual ~ValidationData() = default;
    QSharedPointer<QList<QSharedPointer<NameGroup> > > children_;
};

class IPXACTMODELS_EXPORT HierarchicalValidator
{
public:	

    HierarchicalValidator() = default;

    /*!
     *	Check and mark validity of child items, e.g. the address blocks of a memory map. Base implementation can 
     *  be used if only name uniqueness needs to be checked. Check more things by overriding and 
     *  subclassing ValidationData.
     *  
     *      @param [in] validationData     Necessary data for validation. Contains at least the items to be validated.
     *	    
     * 	    @return True, if all child items are valid amongst themselves, false otherwise.
     */
    virtual bool checkChildren(QSharedPointer<ValidationData> validationData);

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
