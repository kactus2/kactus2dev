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
     *      @param [in] checkedReferences     A list of already checked mode reference values.
     *      @param [in] checkedPriorities     A list of already checked mode priority values.
     *	    
     * 	    @return True, if the given mode references are valid, otherwise false.
     */
    bool hasValidModeRefs(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs, 
        QStringList& checkedReferences, QStringList& checkedPriorities);

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
        QString const& context, QStringList& checkedRefs, QStringList& checkedPriorities, 
        bool* duplicateRefErrorIssued, bool* duplicatePriorityErrorIssued);
    
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
