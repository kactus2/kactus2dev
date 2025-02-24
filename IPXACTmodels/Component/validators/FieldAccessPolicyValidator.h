//-----------------------------------------------------------------------------
// File: FieldAccessPolicyValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 3.8.2023
//
// Description:
// Validator for ipxact:fieldAccessPolicy.
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICYVALIDATOR_H
#define FIELDACCESSPOLICYVALIDATOR_H

#include <IPXACTmodels/Component/FieldAccessPolicy.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class ExpressionParser;
class Mode;

class IPXACTMODELS_EXPORT FieldAccessPolicyValidator
{
public:

    explicit FieldAccessPolicyValidator(QSharedPointer<ExpressionParser> expressionParser);

    ~FieldAccessPolicyValidator() = default;

    FieldAccessPolicyValidator(FieldAccessPolicyValidator& other) = delete;
    FieldAccessPolicyValidator& operator=(FieldAccessPolicyValidator& other) = delete;
    
    /*!
     *	Validates the given field access policy.
     *  
     *    @param [in] fieldAccessPolicy     The field access policy to validate
     *	    
     * 	    @return True, if the field access policy is valid, otherwise false.
     */
    bool validate(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QSharedPointer<QList<QSharedPointer<Mode> > > availableModes) const;

    /*!
     *	Locate errors within a field access policy.
     *  
     *    @param [in] errors                List of found errors.
     *    @param [in] fieldAccessPolicy     The selected field access policy.
     *    @param [in] context               Context to help locate the error.
     */
    void findErrorsIn(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QString const& context, QSharedPointer<QList<QSharedPointer<Mode> > > availableModes) const;
    
    /*!
     *  Check if the field access policy has a valid access value.
     *
     *    @param [in] fieldAccessPolicy   The selected field access policy.
     *
     *    @return True, if the access is valid, otherwise false.
     */
    bool hasValidAccess(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const;

    /*!
     *  Check if the field access policy has a valid write value constraint.
     *
     *    @param [in] fieldAccessPolicy   The selected field access policy.
     *
     *    @return True, if the write value constraint is valid, otherwise false.
     */
    bool hasValidWriteValueConstraint(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const;

    /*!
     *  Check if the field access policy has a valid read response.
     *
     *    @param [in] fieldAccessPolicy   The selected field access policy.
     *
     *    @return True, if the write read response is valid, otherwise false.
     */
    bool hasValidReadResponse(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const;

    /*!
     *  Check if the field access policy has valid broadcasts.
     *
     *    @param [in] fieldAccessPolicy   The selected field access policy.
     *
     *    @return True, if the broadcasts are valid, otherwise false.
     */
    bool hasValidBroadcasts(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const;

    /*!
     *  Check if the field access policy has a valid reserved value.
     *
     *    @param [in] fieldAccessPolicy   The selected field access policy.
     *
     *    @return True, if the broadcasts are valid, otherwise false.
     */
    bool hasValidReserved(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const;

private:

    /*!
     *  Check if the field access policy has a valid field access policy definition reference.
     *
     *    @param [in] fieldAccessPolicy   The selected field access policy.
     *
     *    @return True, if the field access policy definition reference is valid, otherwise false.
     */
    bool hasValidDefinitionRef(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const;

    /*!
     *  Check if the access restrictions of the field access policy are valid.
     *
     *    @param [in] fieldAccessPolicy   The selected field access policy.
     *
     *    @return True, if the access restrictions are valid, otherwise false.
     */
    bool hasValidAccessRestrictions(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QSharedPointer<QList<QSharedPointer<Mode> > > availableModes) const;

    /*!
     *  Find errors within the field access policy definition reference.
     *
     *    @param [in] errors                  List of found errors.
     *    @param [in] fieldAccessPolicy       The selected field access policy.
     *    @param [in] context                 Context to help locate the error.
     */
    void finderrorsInDefinitionRef(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QString const& context) const;

    /*!
     *  Find errors within the access value.
     *
     *    @param [in] errors                  List of found errors.
     *    @param [in] fieldAccessPolicy       The selected field access policy.
     *    @param [in] context                 Context to help locate the error.
     */
    void findErrorsInAccess(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QString const& context) const;

    /*!
     *  Find errors within the write value constraint.
     *
     *    @param [in] errors                  List of found errors.
     *    @param [in] fieldAccessPolicy       The selected field access policy.
     *    @param [in] context                 Context to help locate the error.
     */
    void findErrorsInWriteValueConstraint(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QString const& context) const;

    /*!
     *  Find errors in the read response value.
     *
     *    @param [in] errors                  List of found errors.
     *    @param [in] fieldAccessPolicy       The selected field access policy.
     *    @param [in] context                 Context to help locate the error.
     */
    void findErrorsInReadResponse(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QString const& context) const;

    /*!
     *  Find errors in the broadcasts of the field access policy.
     *
     *    @param [in] errors                  List of found errors.
     *    @param [in] fieldAccessPolicy       The selected field access policy.
     *    @param [in] context                 Context to help locate the error.
     */
    void findErrorsInBroadcasts(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QString const& context) const;

    /*!
     *  Find errors in the access restrictions of the field access policy.
     *
     *    @param [in] errors                  List of found errors.
     *    @param [in] fieldAccessPolicy       The selected field access policy.
     *    @param [in] context                 Context to help locate the error.
     */
    void findErrorsInAccessRestrictions(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy,
        QString const& context, QSharedPointer<QList<QSharedPointer<Mode> > > availableModes) const;

    /*!
     *  Find errors in the reserved value.
     *
     *    @param [in] errors                  List of found errors.
     *    @param [in] fieldAccessPolicy       The selected field access policy.
     *    @param [in] context                 Context to help locate the error.
     */
    void findErrorsInReserved(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, 
        QString const& context) const;

    /*!
     *	Check if a given bit expression is valid.
     *  
     *    @param [in] expression     The given bit expression.
     *	    
     * 	    @return True, if the expression is valid, otherwise false.
     */
    bool isBitExpressionValid(QString const& expression) const;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // FIELDACCESSPOLICYVALIDATOR_H

