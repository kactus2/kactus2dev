//-----------------------------------------------------------------------------
// File: PowerDomainValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 6.10.2023
//
// Description:
// Validator for the ipxact:powerDomain.
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINVALIDTAOR_H
#define POWERDOMAINVALIDTAOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/validators/HierarchicalValidator.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class PowerDomain;
class Segment;
class MemoryReserve;

class MemoryMapBaseValidator;
class ParameterValidator;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:PowerDomain.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PowerDomainValidator : public HierarchicalValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] expressionParser        The parser to use for solving expressions.
     *    @param [in] parameterValidator      Validator used for parameters.
	 */
    PowerDomainValidator(QSharedPointer<QList<QSharedPointer<PowerDomain> > > availableDomains,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterValidator> parameterValidator);

	//! The destructor.
	~PowerDomainValidator();

	// Disable copying.
	PowerDomainValidator(PowerDomainValidator const& rhs) = delete;
	PowerDomainValidator& operator=(PowerDomainValidator const& rhs) = delete;

    /*!
     *  Validates the given power domain.
     *
     *    @param [in] powerDomain        The selected power domain to validate.
     *
     *    @return True, if the power domain is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<PowerDomain> powerDomain) const;

    /*!
     *  Check if the name is valid.
     *
     *    @param [in] name    The selected name.
     *
     *    @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Check if the always on expression is valid.
     *
     *    @param [in] name    The power domain to check.
     *
     *    @return True, if the expression is valid, otherwise false.
     */
    bool hasValidAlwaysOn(QSharedPointer<PowerDomain> powerDomain) const;

    /*!
     *  Check if the subdomain reference is valid.
     *
     *    @param [in] name    The power domain to check.
     *
     *    @return True, if the expression is valid, otherwise false.
     */
    bool hasValidSubDomainOf(QSharedPointer<PowerDomain> powerDomain) const;

    /*!
     *  Check if the power domain contains valid parameters.
     *
     *    @param [in] PowerDomain    The selected power domain.
     *
     *    @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<PowerDomain> PowerDomain) const;

    /*!
     *  Locate errors within an power domain.
     *
     *    @param [in] errors          List of found errors.
     *    @param [in] PowerDomain    The selected power domain.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<PowerDomain> PowerDomain,
        QString const& context) const;

    /*!
     *  Change the available power domains.
     *
     *    @param [in] powerDomains    The new available power domains.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<PowerDomain> > > powerDomains);

private:

    /*!
     *  Find errors within power domain name.
     *
     *    @param [in] errors          List of found errors.
     *    @param [in] powerDomain     The selected power domain.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<PowerDomain> PowerDomain,
        QString const& context) const;
        
    /*!
     *  Find errors within always on expression.
     *
     *    @param [in] errors          List of found errors.
     *    @param [in] powerDomain     The selected power domain.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsInAlwaysOn(QVector<QString>& errors, QSharedPointer<PowerDomain> powerDomain, 
        QString const& context) const;

    /*!
     *  Find errors within sub domain reference.
     *
     *    @param [in] errors          List of found errors.
     *    @param [in] powerDomain     The selected power domain.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsInSubDomainOf(QVector<QString>& errors, QSharedPointer<PowerDomain> powerDomain,
        QString const& context) const;

    /*!
     *  Find errors within power domain parameters.
     *
     *    @param [in] errors          List of found errors.
     *    @param [in] PowerDomain    The selected power domain.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsInParameters(QVector<QString>& errors, QSharedPointer<PowerDomain> PowerDomain,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available power domains.
    QSharedPointer<QList<QSharedPointer<PowerDomain> > > availableDomains_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator used for parameters.
    QSharedPointer<ParameterValidator> parameterValidator_;

};

#endif // POWERDOMAINVALIDTAOR_H
