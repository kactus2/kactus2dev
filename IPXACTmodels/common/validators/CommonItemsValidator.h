//-----------------------------------------------------------------------------
// File: CommonItemsValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.05.2023
//
// Description:
// Functions for validating common items in IPXACT models.
//-----------------------------------------------------------------------------

#ifndef COMMONITEMSVALIDATOR_H
#define COMMONITEMSVALIDATOR_H

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>

class ModeReference;
class RegisterBase;
class AccessPolicy;

namespace CommonItemsValidator
{
    /*!
     * Validates the given name.
     *
     *     @param [in] name The name to validate.
     *
     *     @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name);

    /*!
      * Validates the given isPresent expression.
      *
      *     @param [in] isPresent   The expression to validate.
      *     @param [in] parser      The expression parser to use.
      *
      *     @return True, if the isPresent is valid, otherwise false.
      */
    bool hasValidIsPresent(QString const& isPresent, QSharedPointer<ExpressionParser> parser);

    bool isValidExpression(QString const& expression, QSharedPointer<ExpressionParser> parser);

    /*!
     *	Check if given mode references are valid. Also look for duplicate values when compared with 
     *  previously checked references.
     *  
     *      @param [in] modeRefs              The mode references to check.
     *	    
     * 	    @return True, if the given mode references are valid, otherwise false.
     */
    bool hasValidModeRefs(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs);

    /*!
     *	Finds errors in mode references within in a parent element. Mode reference values and priorities must 
     *  be unique not only within the mode references of the parent element, but also the parent's sibling elements.
     *  For instance, all mode references inside access policy elements of a register must be unique.
     *  
     *      @param [in] errors                          The list of found errors.
     *      @param [in] modeRefs                        The mode references to check.
     *      @param [in] context                         Context to help locate the error.
     *      @param [in] checkedRefs                     Already checked mode reference values.
     *      @param [in] checkedPriorities               Already checked mode reference priorities.
     *      @param [in] duplicateRefErrorIssued         Flag indicating if an error for duplicate references has been issued.
     *      @param [in] duplicatePriorityErrorIssued    Flag indicating if an error for duplicate priorities has been issued.
     */
    void findErrorsInModeRefs(QStringList& errors, QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs,
        QString const& context, QStringList& checkedRefs, QList<unsigned int>& checkedPriorities, 
        bool* duplicateRefErrorIssued, bool* duplicatePriorityErrorIssued);
    
    /*!
     *	Check if a single mode reference is valid. Provide all mode references in containing element to check 
     *  against for duplicates.
     *  
     *      @param [in] modeRefsInContainingElem     All other mode references in the containing element.
     *      @param [in] modeReferenceToCheck         The mode reference to check.
     *      @param [in] isRemap                      Flag indicating if given mode ref is within a remap or not.
     *	    
     * 	    @return True, if mode reference is valid, otherwise false.
     */
    IPXACTMODELS_EXPORT bool singleModeReferenceIsValid(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefsInContainingElem,
        QSharedPointer<ModeReference> modeReferenceToCheck, bool isRemap = false);
    
    /*!
     *	Check if the priority of a single mode reference is valid. Provide all mode references in containing 
     *  element to check against for duplicates (if not within memory remap).
     *
     *      @param [in] modeRefsInContainingElem     All other mode references in the containing element.
     *      @param [in] modeReferenceToCheck         The mode reference whose priority to check.
     *      @param [in] isRemap                      Flag indicating if given mode ref is within a remap or not.
     *
     * 	    @return True, if the mode reference priority is valid, otherwise false.
     */
    IPXACTMODELS_EXPORT bool modeReferencePriorityIsValid(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefsInContainingElem,
        QSharedPointer<ModeReference> modeReferenceToCheck, bool isRemap = false);

    /*!
     *	Check if the reference value of a single mode reference is valid. Provide all mode references in containing
     *  element to check against for duplicates.
     *
     *      @param [in] modeRefsInContainingElem     All other mode references in the containing element.
     *      @param [in] modeReferenceToCheck         The mode reference whose reference value to check.
     *
     * 	    @return True, if the mode reference value is valid, otherwise false.
     */
    IPXACTMODELS_EXPORT bool modeReferenceValueIsValid(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefsInContainingElem,
        QSharedPointer<ModeReference> modeReferenceToCheck, QStringList const& availableModes);

    /*!
     *	Validate given access policies.
     *  
     *      @param [in] accessPolicies     The access policies to validate.
     *	    
     * 	    @return True, if the access policies are valid, otherwise false.
     */
    IPXACTMODELS_EXPORT bool hasValidAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies);
};


#endif // COMMONITEMSVALIDATOR_H
